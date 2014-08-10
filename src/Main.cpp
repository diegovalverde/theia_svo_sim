#include "../include/CSimulator.h"

CSimulator Simulator;

int main(void)
{

	Simulator.Initialize("teapod_svo__depth_4.config");
	
	//Simulator.Initialize("suzane_avo__depth_2.config");

	Simulator.Scene.Camera.WriteProjectionPlaneObj("ProjectionPlane.obj");
	

	//Populate the OCtree
	Simulator.Scene.OCtree.Populate( Simulator.Scene.Geometry );

	//Simulator.Scene.OCtree.DumpObjMainBoundingCube( "BoundingCube.obj" );

	Simulator.Scene.OCtree.DumpWireFrame( "OCtree.obj", COctree::DUMP_LEAFS );

	

	
//	Simulator.RenderFrame();

	return 0;
}