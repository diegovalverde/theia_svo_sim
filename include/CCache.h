#ifndef CCACHE_INCLUDED
#define CCACHE_INCLUDED
#include "../include/CMemory.h"
#include <vector>

using namespace std;

//typedef unsigned int WORD32;

class Foo
{
public:
	CQuad Q;
	Caca MXXX;
};


//----------------------------------------------------------------------------------------------
class CCache
{
public:
	#if 0

	typedef struct TLine_t
	{
		bool Valid;
		WORD32         Tag;
		vector<double> Block;
	} Line_t;

	typedef struct TAddress_t
	{
			
			
			WORD32 BlockOffset : 4  ;		//16 block per line
			WORD32 Index       : 4  ;       //16 lines in the cache
			WORD32 Tag         : 24 ;		//The rest of stuff is just the TAG} TCacheAddress; 
	 } Address_t;


public:
	CCache(){}
	//CMemory M;

	
	void Initialize(int aDepth, int aBlocksPerLine )
	{
		for (int i = 0; i < aDepth; i++)
		{
			Line_t Line = {false,0,vector<double>(aBlocksPerLine,0) };
			mLines.push_back(Line);
		}
	}

	bool Read( CMemory::Address_t aAddress, double & aData )
	{
		Address_t Address =  aAddress.CacheAddr;
		Line_t Line = mLines[ Address.Index ];
		if (Line.Valid == false)
		{
			return false;
		}

		if (Line.Tag == Address.Tag)
		{
			aData =  Line.Block[ Address.BlockOffset ] ;
			return true;
		}

		return false;
	}

	void Write( CMemory::Address_t  aAddress, double & aData)
	{
		Address_t Address =  aAddress.CacheAddr;
		mLines[ Address.Index ].Tag = Address.Tag;
		mLines[ Address.Index ].Valid = true;
		mLines[ Address.Index ].Block[ Address.BlockOffset ] = aData;
			
	}
	
public:
	vector<Line_t> mLines;
#endif

};
//----------------------------------------------------------------------------------------
#endif
