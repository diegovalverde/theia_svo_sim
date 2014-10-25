#include "../include/CMemory.h"
#include "../include/Common.h"
#include <bitset>

//---------------------------------------------------------------------------------------------
CMemory::CMemory()
{
	//mValidateCacheData = true;
	

	mParameter["cache-enabled"] = 1;
	mParameter["validate-cache-data"] = 1;
	mParameter["cache-lines-per-way"] = (8*8);
	mParameter["cache-blocks-per-line"] = 64;
}
//---------------------------------------------------------------------------------------------
void CMemory::Initialize( int aTreeDepth, int aLinesPerCache )
{
	mCache_L1.clear();

	for (int i = 0; i < aTreeDepth; i++)
	{
		CCache Cache;
		int LineCount = 0;
		switch(i)
		{
			case 0:  LineCount = 8; //Level zero stores 8 octants
				break;
			case 1:  LineCount = (8*8); //Level zero stores 8*8 octants
				break;		 
			default: LineCount = aLinesPerCache;
		}
		
		Cache.Initialize(LineCount, mParameter["cache-blocks-per-line"] );
		mCache_L1.push_back(Cache);
	}
}
//---------------------------------------------------------------------------------------------
string CMemory::PrintCacheHitRates()
{
	ostringstream oss;
	

	for (int i = 0; i < mCache_L1.size(); i++)
	{

		int UsedLines = 0;
		for (int j = 0; j < mCache_L1[i].mLines.size(); j++)
			if (mCache_L1[i].mLines[j].Valid == true)
				UsedLines++;
	

		oss << "Cache L" << i << " line count : " 
		<< mCache_L1[i].mLines.size() 
		<< ", used lines : ("  <<UsedLines  << ") " << (100*( (float)UsedLines/(float)mCache_L1[i].mLines.size() ) ) << "%"
		<< ", replaced lines : (" << mCache_L1[i].mReplaceLineCount << " ) " << 100.0f*((float)mCache_L1[i].mReplaceLineCount/(float)mCache_L1[i].mLines.size())<< "% "
		<< " : hit rate : (" << mCache_L1[i].mHitCount <<  "/" 	<< mCache_L1[i].mReadCount 	<< ") "
		<< (100*((float)mCache_L1[i].mHitCount / (float)mCache_L1[i].mReadCount)) << "% \n";
		
	}	
	return oss.str();
}
//---------------------------------------------------------------------------------------------
void CMemory::Write( TMortonCode aAddress, unsigned long aData )
{
	mMainBuffer[ aAddress ] = aData;
}
//---------------------------------------------------------------------------------------------
void CMemory::ClearCaches(void)
{
	for (int i = 0; i < mCache_L1.size(); i++)
		mCache_L1[i].Clear();		
	
}
//---------------------------------------------------------------------------------------------
unsigned long int CMemory::GetCacheSizeBytes()
{
	unsigned long int SizeBytes = 0;

	for (int i = 0; i < mCache_L1.size(); i++)
		SizeBytes += mCache_L1[i].GetSizeBytes();


	return SizeBytes;

}
//---------------------------------------------------------------------------------------------
double CMemory::Read( TAddress aAddress )
{
	unsigned int Data;
	Statistics->Stat["mem.total_reads"] += 1;

	if (mParameter["cache-enabled"] == 0)
	{
		Statistics->Stat["mem.external.read_access_count"] += 1;
		return mMainBuffer[aAddress.LogicAddr];
	}



	bool Hit = false;
	//See which of the caches has the data
	//Start by extracting the depth
	/*
		Level			Hibitpos	Morton
		(0+1)/3 = 0		0			1
		(2+1)/3 = 1		2			100
		(6+1)/3	= 2		6			1000 000
		(9+1)/3 = 3		9			1000 000 000
	*/
	int TreeLevel = (GetHighestBitIndex( aAddress.LogicAddr ) + 1)/3;
		
	
	if ( (Hit = mCache_L1[TreeLevel].Read( aAddress, Data ) == true))
	{

		//Optional test to make sure that whatever it is that we read is in fact valid
		if (mParameter["validate-cache-data"])
		{
			unsigned int MainBufferData = mMainBuffer[aAddress.LogicAddr];
			if (Data != MainBufferData)
				cout << "Invalid data read from cache\n";
		
		}
		Statistics->Stat["mem.cache.l1.hit_count"] += 1;
		
		return Data;
	} else {
		//We got a cache miss, let's account for that
		Statistics->Stat["mem.cache.l1.miss_count"] += 1;
		mCache_L1[TreeLevel].Write(aAddress, mMainBuffer[aAddress.LogicAddr], Statistics);
		
	}
	
	//Not in cache, ok go read from second cache
/*	if ( (Hit = Cache_L2.Read( aAddress, Data ) == true))
	{
		Cache_L2.Write(aAddress,Data);
		return Data;
	}*/

	Statistics->Stat["mem.external.read_access_count"] += 1;
	return mMainBuffer[aAddress.LogicAddr];

};
//---------------------------------------------------------------------------------------------
void CMemory::CCache::Clear() 
{ 
	mWriteCount = 0; 
	mReadCount  = 0; 
	mHitCount   = 0; 
	mReplaceLineCount = 0; 
	for (int i = 0; i < mLines.size(); i++)
	{
		mLines[i].Valid = false;
		mLines[i].Tag = 0;
		mLines[i].Block = 0xffff;
	}
}
//---------------------------------------------------------------------------------------------
unsigned long int CMemory::CCache::GetSizeBytes()
{
	double SizeBytes = 0;
	for (int i = 0; i < mLines.size(); i++)
		SizeBytes +=  sizeof( mLines[i].Block ) + sizeof (mLines[i].Tag);
	
	return SizeBytes;
}
//---------------------------------------------------------------------------------------------
void CMemory::CCache::Initialize(int aDepth, int aBlocksPerLine )
{
	for (int i = 0; i < aDepth; i++)
	{

		CCache::CacheLine Line = {false,0,0xffff };
		mLines.push_back(Line);
	}
}
//---------------------------------------------------------------------------------------------
unsigned long int CMemory::CCache::GetLineIndex( TAddress aAddress )	
{
	TCacheAddress Address =  aAddress.CacheAddr;
	Address.Tag = aAddress.LogicAddr;
	
	return (Address.Index % mLines.size());

}
//---------------------------------------------------------------------------------------------
bool CMemory::CCache::Read(  CMemory::TAddress aAddress, unsigned int & aData )
{

	mReadCount++;

	TCacheAddress Address =  aAddress.CacheAddr;
	Address.Tag = aAddress.LogicAddr;
	
	int LineIndex = GetLineIndex( aAddress );
	//CacheLine Line = mLines[ Address.Index % mLines.size() ];
	CacheLine Line = mLines[ LineIndex ];
	if (Line.Valid == false)
	{
		return false;
	}

	if (Line.Tag == Address.Tag)	
	{
		mHitCount++;
#ifdef DUAL_BLOCK_ENABLE					
		aData =  Line.Block[ Address.BlockOffset ] ;
#else
		aData =  Line.Block ;	
#endif		
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------------------------

void CMemory::CCache::Write( CMemory::TAddress aAddress, unsigned int & aData, CStatistics * aStatistics)
{
	mWriteCount += 1;
	CMemory::TCacheAddress Address =  aAddress.CacheAddr;

	int Index = GetLineIndex( aAddress );


	if (mLines[ Index ].Valid == true)
	{
		bitset<32> BitsetOld(mLines[ Index ].Tag);
		bitset<32> BitsetNew(Index);

		mReplaceLineCount++;
		aStatistics->Stat["replace_cache_entry"] += 1;
	}

	mLines[ Index ].Tag = aAddress.LogicAddr;//Address.Tag;
	mLines[ Index ].Valid = true;
#ifdef DUAL_BLOCK_ENABLE						
	mLines[ Index ].Block[ Address.BlockOffset ] = aData;
#else
	mLines[ Index ].Block = aData;
#endif	
			
}
//---------------------------------------------------------------------------------------------
