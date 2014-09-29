#include "../include/CGpu.h"
#define NULL_MORTON_IDX 0

//----------------------------------------------------------------------
void CGpu::Initialialize(CStatistics * aStatistics, CScene * aScene )
{ 
	Statistics = aStatistics;

	Gt.Memory = & Memory;  
	Gt.Memory->Statistics = Statistics;

		
	
	Rgu.Scene = aScene;
	Memory.Statistics = aStatistics;
	Rgu.Statistics = aStatistics;
	Gt.Scene =aScene;
	Gt.Statistics = aStatistics;
	Gt.Scene->OCtree.Statistics = aStatistics;
}
//----------------------------------------------------------------------
void CGpu::Execute( int aWidth, int aHeight , ofstream & ofs)
{
	

	vector<TMortonCode> GtResut;
	for ( int Col = 0; Col < aHeight; Col++)
	{
		for (int Row = 0; Row < aWidth; Row++)
		{
			CRay Ray =  Rgu.Execute( Row ,Col );

			TMortonCode  GtResut = Gt.Execute( Ray, ROOT_MORTON_CODE, NON_EMPTY );

			if (GtResut == NULL_MORTON_IDX)
			{
				Statistics->Stat["TotalRayNotIntersectingVoxel"] += 1;
				ofs << " 0 0 0\n";	
			} else {
				cout << ".";
				ofs << "#" << GtResut << "\n";
				ofs << " 255 255 255\n";	
			}
				
		}
	}
	
	
	

}
//----------------------------------------------------------------------