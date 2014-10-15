#include "../include/CMemory.h"
#include "../include/Common.h"


//---------------------------------------------------------------------------------------------
CMemory::CMemory()
{
	//mValidateCacheData = true;
	

	mParameter["cache-enabled"] = 1;
	mParameter["validate-cache-data"] = 1;
	mParameter["cache-lines-per-way"] = 64;
	mParameter["cache-blocks-per-line"] = 64;
}
//---------------------------------------------------------------------------------------------
void CMemory::Initialize( int aTreeDepth, int aLinesPerCache )
{
	mCache_L1.clear();
	for (int i = 0; i < aTreeDepth; i++)
	{
		CCache Cache;
		int LineCount = (i == 0) ? 8 : aLinesPerCache;
		Cache.Initialize(aLinesPerCache, mParameter["cache-blocks-per-line"] );
		mCache_L1.push_back(Cache);
	}
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
		mCache_L1[TreeLevel].Write(WriteAddr, WriteData);

		WriteAddr.CacheAddr.BlockOffset = 1;
		WriteData = ((mMainBuffer.find(WriteAddr.LogicAddr) == mMainBuffer.end() )? 0xffff : mMainBuffer[WriteAddr.LogicAddr]);
		mCache_L1[TreeLevel].Write(WriteAddr,WriteData);
		
		
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
bool CMemory::CCache::Read(  CMemory::TAddress aAddress, unsigned int & aData )
{
	TCacheAddress Address =  aAddress.CacheAddr;
	Address.Tag = aAddress.LogicAddr;
	
	CacheLine Line = mLines[ Address.Index % mLines.size() ];
	if (Line.Valid == false)
	{
		return false;
	}

	if (Line.Tag == Address.Tag)	//This is dangerous. If just a single block is valid, then the entire line is marked as valid, since most stuff to the left (Tag) is zeroes, you may get false positives
	{
		aData =  Line.Block[ Address.BlockOffset ] ;
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------------------------
void CMemory::CCache::Write( CMemory::TAddress aAddress, unsigned int & aData)
{
	CMemory::TCacheAddress Address =  aAddress.CacheAddr;
	int Index = Address.Index % mLines.size();
	mLines[ Index ].Tag = aAddress.LogicAddr;//Address.Tag;
	mLines[ Index ].Valid = true;
	mLines[ Index ].Block[ Address.BlockOffset ] = aData;
			
}
//---------------------------------------------------------------------------------------------
