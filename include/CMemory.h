#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED
#include "Geometry.h"
#include "CStatistics.h"
#include "CGenericObject.h"

typedef unsigned int WORD32;
//----------------------------------------------------------------------------------------------
class CMemory : public CGenericObject
{
public:
	CMemory();
	

	void Initialize( int aTreeDepth, int aLinesPerCache );
	void Clear();
	string PrintCacheHitRates();
#ifdef DUAL_BLOCK_ENABLE
	typedef struct T_CacheAddress
	{
			
			
			WORD32 BlockOffset : 1  ;		//16 block per line
			WORD32 Index       : 7  ;       //16 lines in the cache
			WORD32 Tag         : 24 ;		//The rest of stuff is just the TAG} TCacheAddress; 
	 } TCacheAddress;
#else
	 typedef struct T_CacheAddress
	{
			
			WORD32 Index       : 7  ;       //16 lines in the cache
			WORD32 Tag         : 24 ;		//The rest of stuff is just the TAG} TCacheAddress; 
	 } TCacheAddress;
#endif	 

		typedef union T_Address
		{
			TCacheAddress CacheAddr;
			WORD32        LogicAddr;
		} TAddress;

    double Read( TAddress aAddress );
	void Write( TMortonCode  aAddress, unsigned long aData );

	class CCache
	{
	public:
		struct CacheLine
		{
			bool Valid;
			WORD32         Tag;
#ifdef DUAL_BLOCK_ENABLE			
			vector<unsigned int> Block;
#else
			unsigned int Block;
#endif			
		};
	public:
		CCache(){ mWriteCount = 0; mReadCount = 0; mHitCount = 0; mReplaceLineCount = 0;}
		void Initialize(int aDepth, int aBlocksPerLine );
		bool Read( TAddress aAddress, unsigned int & aData );
	

		void Write( TAddress aAddress, unsigned int & aData, CStatistics * aStatistics);
		
	public:
		vector<CacheLine> mLines;
		long unsigned int mWriteCount;
		long unsigned int mReadCount;
		long unsigned int mHitCount;
		long unsigned int mReplaceLineCount;
	private:
		unsigned long int GetLineIndex( TAddress aAddress )	;
	};
	map<TMortonCode, unsigned int > mMainBuffer;
	vector<CCache> mCache_L1;
	CCache mCache_L2;
	CStatistics                  *  Statistics;

	bool mValidateCacheData;
//	bool mBypassCache;
};
//----------------------------------------------------------------------------------------------
#endif