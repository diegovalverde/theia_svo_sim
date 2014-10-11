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
	

	typedef struct T_CacheAddress
	{
			
			
			WORD32 BlockOffset : 1  ;		//16 block per line
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

	class CCache
	{
	public:
		struct CacheLine
		{
			bool Valid;
			WORD32         Tag;
			vector<unsigned int> Block;
		};
	public:
		CCache(){}
		void Initialize(int aDepth, int aBlocksPerLine );
		bool Read( TAddress aAddress, unsigned int & aData );
	

		void Write( TAddress aAddress, unsigned int & aData);
		
	public:
		vector<CacheLine> mLines;
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