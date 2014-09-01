#ifndef CSIMULATOR_H_INCLUDED
#define CSIMULATOR_H_INCLUDED



#include <string>
#include <vector>
#include <functional>
#include <map>
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
	typedef std::function<string (vector<string>, CSimulator * aSimulator ) > TCommand;

public:	
	
	CStatistics		Statistics;
	void			Initialize( string aFileName );
	string          LoadConfigurationFile( string aFileName );
	string          ExecuteCommand(vector<string> aCommands);
public:
	void RenderFrame( string aFileName );
	CScene Scene;
	CGpu   Gpu;

private:
	bool mRunning;
	map<string, TCommand> mCommands;

};
//------------------------------------------------------------------

#endif