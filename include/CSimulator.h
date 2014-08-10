#ifndef CSIMULATOR_H_INCLUDED
#define CSIMULATOR_H_INCLUDED



#include <string>
#include <vector>
using namespace std;

#include "../../include/Geometry.h"
#include "../include/CStatistics.h"
#include "../include/CRayGenertionUnit.h"
#include "../include/CGeometryTraversalUnit.h"

//------------------------------------------------------------------
class CGpu
{
public:
	CGpu(){};
	~CGpu(){Statistics = NULL; };

	void Execute( void );
	void  Initialialize( void );

public:
	CRayGenerationUnit				 Rgu;
	CGeometryTraversalUnit           Gt;
	CStatistics				*        Statistics;
	
};
//------------------------------------------------------------------
class CSimulator
{
public:
	CSimulator();
	~CSimulator();

public:
	
	
	CStatistics		Statistics;
	void			Initialize( string aFileName );
public:
	void RenderFrame( void );
	CScene Scene;
	CGpu   Gpu;

private:
	bool mRunning;

};
//------------------------------------------------------------------

#endif