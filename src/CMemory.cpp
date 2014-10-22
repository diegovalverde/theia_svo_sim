#include "../include/CMemory.h"
#include "../include/Common.h"
#include <bitset>

//---------------------------------------------------------------------------------------------
CMemory::CMemory()
{
	//mValidateCacheData = true;
	

	mParameter["cache-enabled"] = 1;
	mParameter["validate-cache-data"] = 1;
	mParameter["cache-lines-per-way"] = (8*8*8);
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
	



		oss << "Cache Level " << i << " line count : " 
		<< mCache_L1[i].mLines.size() 
		<< " used lines : ("  <<UsedLines  << ") " << (100*( (float)UsedLines/(float)mCache_L1[i].mLines.size() ) ) << "%\n"
		<< " : hit rate : " << (100*((float)mCache_L1[i].mHitCount / (float)mCache_L1[i].mReadCount)) << "% ";
		
	}	
	return oss.str();
}
//---------------------------------------------------------------------------------------------
void CMemory::Write( TMortonCode aAddress, unsigned long aData )
{
	mMainBuffer[ aAddress ] = aData;
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

		//The cache line size is 2 16bit blocks. You can only write the *entire* line,
		//you can not write a single block of the line but *all* of the blocks.
		//This is because you have a single valid bit per line, this is, either all blocks are
		//valid or they are not
		TAddress WriteAddr = aAddress;
		WriteAddr.CacheAddr.BlockOffset = 0;
		unsigned int WriteData = ((mMainBuffer.find(WriteAddr.LogicAddr) == mMainBuffer.end() )? 0xffff : mMainBuffer[WriteAddr.LogicAddr]);
		mCache_L1[TreeLevel].Write(WriteAddr, WriteData, Statistics);

		WriteAddr.CacheAddr.BlockOffset = 1;
		WriteData = ((mMainBuffer.find(WriteAddr.LogicAddr) == mMainBuffer.end() )? 0xffff : mMainBuffer[WriteAddr.LogicAddr]);
		mCache_L1[TreeLevel].Write(WriteAddr,WriteData, Statistics);
		
		
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
void CMemory::CCache::Initialize(int aDepth, int aBlocksPerLine )
{
	for (int i = 0; i < aDepth; i++)
	{
		CCache::CacheLine Line = {false,0,vector<unsigned int>(aBlocksPerLine,0xffffffff) };
		mLines.push_back(Line);
	}
}
//---------------------------------------------------------------------------------------------
//#define OPT_INDEX 1
unsigned long int CMemory::CCache::GetLineIndex( TAddress aAddress )	
{
	TCacheAddress Address =  aAddress.CacheAddr;
	Address.Tag = aAddress.LogicAddr;
	
#ifdef OPT_INDEX
	bitset<256> Bitset(Address.Index);

	int HighestBit = GetHighestBitIndex( Address.Index );
	//cout << "Highest bit " << std::dec << HighestBit << "\n";
	if (HighestBit > 0 && HighestBit < 256)
		Bitset[ HighestBit ] = false;
	return ( (int)Bitset.to_ulong() % mLines.size() );


#else
	return (Address.Index % mLines.size());
#endif		
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

	if (Line.Tag == Address.Tag)	//This is dangerous. If just a single block is valid, then the entire line is marked as valid, since most stuff to the left (Tag) is zeroes, you may get false positives
	{
		mHitCount++;
		aData =  Line.Block[ Address.BlockOffset ] ;
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------------------------

void CMemory::CCache::Write( CMemory::TAddress aAddress, unsigned int & aData, CStatistics * aStatistics)
{
	mWriteCount += 1;
	CMemory::TCacheAddress Address =  aAddress.CacheAddr;

#ifdef OPT_INDEX

	/*
	//This makes it muuuuuuuuuch worse!!!!

	//cout << "The Address is " << std::hex << aAddress.LogicAddr << "\n";
	bitset<128> Bitset(aAddress.LogicAddr);

	int HighestBit = GetHighestBitIndex( aAddress.LogicAddr );
	//cout << "Highest bit " << std::dec << HighestBit << "\n";
	if (HighestBit > 0 && HighestBit < 128)
		Bitset[ HighestBit ] = false;
	int Index = (int)Bitset.to_ulong() % mLines.size();  //TODO: Maybe MSB is not needed
*/

//This is worse but not as much as the previous one
bitset<256> Bitset(Address.Index);

	int HighestBit = GetHighestBitIndex( Address.Index );
	//cout << "Highest bit " << std::dec << HighestBit << "\n";
	if (HighestBit > 0 && HighestBit < 256)
		Bitset[ HighestBit ] = false;
	int Index = (int)Bitset.to_ulong() % mLines.size();  //TODO: Maybe MSB is not needed

	
#else	
	//int Index = Address.Index % mLines.size();  //TODO: Maybe MSB is not needed
	int Index = GetLineIndex( aAddress );
#endif	


	if (mLines[ Index ].Valid == true)
	{
		bitset<64> Bitset(Index);
	//	cout << std::hex << "0x" << aAddress.LogicAddr << " " << Bitset.to_string() << "\n";
		aStatistics->Stat["mem.replace_cache_entry"] += 1;
	}

	mLines[ Index ].Tag = aAddress.LogicAddr;//Address.Tag;
	mLines[ Index ].Valid = true;
	mLines[ Index ].Block[ Address.BlockOffset ] = aData;
			
}
//---------------------------------------------------------------------------------------------
