#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED
#include "Geometry.h"
#include "CStatistics.h"
#include "CGenericObject.h"
#include <cstdint>

typedef unsigned int WORD32;
//----------------------------------------------------------------------------------------------
class CMemory : public CGenericObject
{
public:
	CMemory();
	

	void Initialize( int aTreeDepth, int aLinesPerCache );
	void Clear();
	string PrintCacheHitRates();
	unsigned long int GetCacheSizeBytes();
	 typedef struct T_CacheAddress
	{
			
			WORD32 Index       : 7  ;       //16 lines in the cache
			WORD32 Tag         : 24 ;		//The rest of stuff is just the TAG} TCacheAddress; 
	 } TCacheAddress;

		typedef union T_Address
		{
			TCacheAddress CacheAddr;
			WORD32        LogicAddr;
		} TAddress;

    double Read( TAddress aAddress );
	void Write( TMortonCode  aAddress, unsigned long aData );
	void ClearCaches(void);

	class CCache
	{
	public:
		struct CacheLine
		{
			bool Valid;
			WORD32         Tag;
			std::uint16_t Block;

		};
	public:
		CCache(){ Clear(); }
	
		void Initialize(int aDepth, int aBlocksPerLine );
		void Clear();
		bool Read( TAddress aAddress, unsigned int & aData );
		unsigned long int  GetSizeBytes();

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

};
//----------------------------------------------------------------------------------------------
#endif