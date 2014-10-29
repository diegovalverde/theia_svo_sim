#include "../include/CGpu.h"
#define NULL_MORTON_IDX 0

//----------------------------------------------------------------------
CGpu::CGpu()
{
	mParameter["core-count"] = 1;
	mParameter["grid-partition-size"] = 10;
	mParameter["x0"]                  = 0;
	mParameter["y0"]                  = 0;
	mParameter["print-status"]       = 1;

	mObjects["memory"] = (CGenericObject*)&Memory;
	Gt.push_back( CGeometryTraversalUnit() );
}
//----------------------------------------------------------------------
void CGpu::Initialialize(CStatistics * aStatistics, CScene * aScene )
{ 
	Statistics = aStatistics;
	mScene = aScene;

	for (int i = 0; i < Gt.size(); i++)
	{
		Gt[i].Memory = & Memory;  
		Gt[i].Memory->Statistics = Statistics;
		Gt[i].Scene =aScene;
		Gt[i].Statistics = aStatistics;
		Gt[i].Scene->OCtree.Statistics = aStatistics;

	}
		
	
	Rgu.Scene = aScene;
	Memory.Statistics = aStatistics;
	Rgu.Statistics = aStatistics;
	
}
//----------------------------------------------------------------------
int  CGpu::GetAssignedGt(int aRow,int aCol)
{
	if (mParameter["core-count"] == 1)
		return 0;

	int Partitions = mParameter["core-count"] >> 1;	//Only works if "core-count" = 4,8,16,32,64,128,256,521, etc.. does not work for 2,1,0

	int ResolutionWidth  = mScene->mParameter["resolution-width"];
	int ResolutionHeight = mScene->mParameter["resolution-height"];
	return (aRow / (ResolutionWidth >> Partitions)) + (aCol  / (ResolutionHeight >> Partitions) );

	/*
	switch (CoreCount)
	{
	case 1: return 0;
	case 2: 
		{
			if (aRow < mScene->ResolutionHeight/2)
				return 0;
			else
				return 1;
		}
	};
	return 0;*/
}
//----------------------------------------------------------------------
/*
	This is the heart and soul of the simulation. a Gpu simulates one or more Gts (Geometry Traversal Units).
	At nay given point in time, each Gt is in charge of a single rectangular section of the proyection plane. This
	rectagunlar section is called a 'GridParitition'.
*/
void CGpu::Execute( ofstream & ofs)
{


	//Ok, so each of the GP's will only get a *portion* of the projection screen to work on.
	//That essetially means a square section of the projection plane.
	//So if we have just one single GT, then we 'slide'the window
	int DX = mParameter["grid-partition-size"];
	

	int ResolutionWidth  = mScene->mParameter["resolution-width"];
	int ResolutionHeight = mScene->mParameter["resolution-height"];

	
	typedef struct T_GridElement { 	int X0,X1,Y0,Y1; } TGridElement;
    vector< TGridElement > GridParitition;

	if (mParameter["print-status"] == 1)
		cout << "-I- Initializing Proyection Plane grid partitions\n";

	for ( int Col = 0; (Col+DX) <= ResolutionHeight ; Col+=DX)
	{
		for ( int Row = 0; (Row+DX) <= ResolutionWidth ; Row+=DX)
		{

			TGridElement E = {Row,Row + DX - 1,Col,Col + DX - 1};
			GridParitition.push_back(E);
		}
	}

if (mParameter["print-status"] == 1)
		cout << "-I- Initializing color dump memory\n";

	vector< vector< bool> > ColorDump ;
	for (int Row = 0; Row < ResolutionWidth; Row++)
	{
		ColorDump.push_back(vector<bool>());
		for ( int Col = 0; Col < ResolutionHeight ; Col++)
			ColorDump[Row].push_back(false);
	}


	cout << "-I- Starting render\n";
	vector<TMortonCode> GtResut;
	for (int ParititioIndex = 0; ParititioIndex < (GridParitition.size()/mParameter["core-count"]); ParititioIndex++)
	{
		if (mParameter["print-status"] == 1)
			printf("]\n[ %-3d , %-3d, %-3d , %-3d ] [ ",
		GridParitition[ParititioIndex].X0 , GridParitition[ParititioIndex].Y0,
		GridParitition[ParititioIndex].X1, GridParitition[ParititioIndex].Y1 );

		//If multicore is enabled, this is place to reset the cache and averaging the hits/misses!!

		for (  int Col = GridParitition[ParititioIndex].Y0 ; Col < GridParitition[ParititioIndex].Y1 ; Col++)
		{
			for (int Row = GridParitition[ParititioIndex].X0; Row < GridParitition[ParititioIndex].X1; Row++)
			{

			
				CRay Ray =  Rgu.Execute( Row ,Col );

				//int GtIndex  = GetAssignedGt(Row,Col);

				//I need to consider the case where we we have more than 1 gt. A single Gt is too unfair for the cache...
				//In reality I do not need to model all of the gt, only one of them... however it would be best to model
				//this single guy going tru the grid and then 'reseting' the cache and averaging the hits/misses
				TMortonCode  GtResut = Gt[ 0 ].Execute( Ray, ROOT_MORTON_CODE, NON_EMPTY );
				
				ColorDump[Row][Col] = (GtResut == NULL_MORTON_IDX) ? false : true;
				
			}
			if (mParameter["print-status"] == 1)
				cout << ".";
		}
	}
	
	if (mParameter["print-status"] == 1)
		cout << "]\n Dumping colors to file\n";

	//Now dump the colors into the file
	for ( int Col = 0; Col < ResolutionHeight ; Col++)
	{
		for (int Row = 0; Row < ResolutionWidth; Row++)
		{
			if (ColorDump[Row][Col] == false)
			{
				Statistics->Stat["gpu.total_ray_not_intersecting_any_voxel"] += 1;
				ofs << " 0 0 0\n";	
			} else {
				
				ofs << "#" << Row << "," << Col  << "\n";
				ofs << " 255 255 255\n";	
			}
		}
	}


}
//----------------------------------------------------------------------