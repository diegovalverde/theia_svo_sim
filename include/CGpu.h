#ifndef CGPU_H_INCLUDED
#define CGPU_H_INCLUDED



#include <string>
#include <vector>
#include <functional>
#include <map>
using namespace std;

#include "../include/Geometry.h"
#include "../include/CStatistics.h"
#include "../include/CRayGenertionUnit.h"
#include "../include/CGeometryTraversalUnit.h"
#include "../include/CMemory.h"


//------------------------------------------------------------------
class CGpu
{
public:
	CGpu(){};
	~CGpu(){Statistics = NULL; };

	void Execute( int aRow, int aCol, ofstream & ofs );
	void  Initialialize( CStatistics * aStatistics, CScene * aScene );  

public:
	CRayGenerationUnit				 Rgu;
	CGeometryTraversalUnit           Gt;
	CStatistics				*        Statistics;
	CMemory                          Memory;
	
};
//------------------------------------------------------------------

#endif
