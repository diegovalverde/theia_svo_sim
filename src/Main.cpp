#include "../include/CSimulator.h"
#include <sstream>
#include <iterator>

CSimulator Simulator;

int main(void)
{
	try
	{
	Simulator.Initialize("teapod_svo__depth_4.config");
	Simulator.LoadConfigurationFile("teapod_svo__depth_4.config");
	
	//Simulator.Initialize("suzane_avo__depth_2.config");

	//Simulator.Scene.Camera.WriteProjectionPlaneObj("ProjectionPlane.obj");
	

	//Populate the OCtree
	Simulator.Scene.OCtree.Populate( Simulator.Scene.Geometry );

	//Simulator.Scene.OCtree.DumpObjMainBoundingCube( "BoundingCube.obj" );

	//Simulator.Scene.OCtree.DumpWireFrame( "OCtree.obj", COctree::DUMP_LEAFS );

	while (1)
	{
		cout << "::";
		string Line;
		std::getline (std::cin,Line);
		stringstream ss(Line);
		std::istream_iterator<string> begin(ss);
		std::istream_iterator<string> end;
		vector<string> Tokens(begin, end);

		cout << Simulator.ExecuteCommand(Tokens) << "\n\n";
	}

	
	//Simulator.RenderFrame();

	return 0;
	}
	catch (string aMessage)
	{
		cout << "-E- " << aMessage << "\n";
	}
}