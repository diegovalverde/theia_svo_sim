#ifndef CSIMULATOR_H_INCLUDED
#define CSIMULATOR_H_INCLUDED



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
#include "../include/CGpu.h"
#include "CGenericObject.h"

#define NULL_MORTON_IDX 0

enum E_RENDER_TYPE
{
	RENDER_HW_NAIVE,
	RENDER_SW_PURE,
	RENDER_HW
};
//------------------------------------------------------------------
class CSimulator : public CGenericObject
{
public:
	CSimulator();
	~CSimulator();

public:
	typedef std::function<string (vector<string>, CSimulator * aSimulator ) > TCommand;

public:	
	
	CStatistics		Statistics;
	void			Initialize( string aFileName );
	void			Initialize( void );
	string          LoadConfigurationFile( string aFileName );
	string          ExecuteCommand(vector<string> aCommands);
public:
	void RenderFrame( string aFileName, E_RENDER_TYPE aRenderType );
	void RenderSwFast( ofstream & ofs );
	void RenderHw( ofstream & ofs );
	void RenderNaive( ofstream & ofs );

	CScene Scene;
	CGpu   Gpu;
public:
	string mModelName;
	struct
	{
		string                       LogFileName;
		vector< std::pair<int,int> > Resolutions;
		int                          Depth;
	} ExperimentParameters;


private:
	void OpenRenderFile( ofstream & ifs );
	void CloseRenderFile( ofstream & ifs );

	
	map<string, TCommand> mCommands;

	
	

};
//------------------------------------------------------------------

#endif
