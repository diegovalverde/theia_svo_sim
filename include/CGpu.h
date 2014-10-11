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
#include "CGenericObject.h"

//------------------------------------------------------------------
class CGpu : public CGenericObject
{
public:
	CGpu();
	~CGpu(){Statistics = NULL; };

	void Execute( ofstream & ofs );
	void  Initialialize( CStatistics * aStatistics, CScene * aScene );  
	int   GetAssignedGt( int aRow, int aCol );
public:
	CRayGenerationUnit				 Rgu;
	vector<CGeometryTraversalUnit>   Gt;
	CStatistics				*        Statistics;
	CScene                  *        mScene;
	CMemory                          Memory;

	
};
//------------------------------------------------------------------

#endif
