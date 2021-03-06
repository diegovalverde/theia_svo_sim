#ifndef COCTREE_H_INCLUDED
#define COCTREE_H_INCLUDED
#include "Geometry.h"
#include "CStatistics.h"
#include "CGenericObject.h"
#include <map>

enum E_OCTANT
{
	TOP_FRONT_LEFT,       // 0
	TOP_FRONT_RIGHT,      // 1
	TOP_BACK_LEFT,        // 2
	TOP_BACK_RIGHT,       // 3
	BOTTOM_FRONT_LEFT,    // 4
	BOTTOM_FRONT_RIGHT,   // 5
	BOTTOM_BACK_LEFT,     // 6
	BOTTOM_BACK_RIGHT     // 7
};

//---------------------------------------------------------------------------------------------------------------------
class COctant : public CGenericObject
{
public:
	COctant() {  isEmpty= true; isLeaf = false; }
	string DumpVertex();
	string DumpFaces( int aFaceOffset );
	bool   ContainsVertex( CVector aVertex );
	bool   RayIntersection( CRay aRay );
	bool   TriangleIntersection( CTriangle aTriangle );

	CVector	MinVertex,MaxVertex;
	double VoxelEdgeLen;
	double Lenght;
	bool isLeaf;
    bool isEmpty;
	vector<int> VertexIndexList;

	vector< CFace<3> > FaceIndexList;
	CVector Center;
	CVoxel Voxel;
	CVector IntersectionPoint;	
};
//---------------------------------------------------------------------------------------------------------------------
class COctree : public CGenericObject
{
public:
	COctree() {mParameter["depth"] = 2; }
	~COctree() {}

	enum TDumpType
	{
		DUMP_LEAFS,
		DUMP_NON_EMPTY,
		DUMP_ALL,
	};
	

public:
	void	Populate( CGeometry & aGeometry );
	void	Save(string aPath );								///Save the OCtree as a OBJ file
	COctant GetOCtant( TMortonCode aMortonCode );
	void	DumpObjMainBoundingCube( string aFilePath );
	void	DumpWireFrame( string aFilePath, TDumpType aDumpType );
	

	TMortonCode  GetIntersectedVoxel(TMortonCode aMortonCode,  CRay  aRay );
	void   InitializeRootOctant();

public:
	CGeometry					 *  Geometry;
	CStatistics                  *  Statistics;
	
public:
	CVector GetCenterFromMortonKey( TMortonCode aMortonKey, CVector aParentCenter, float aParentDiameter );
	void CreateMainBoundigCube();
	void CreateOctant( CVector aCenter,double aParentEdgeLen,  TMortonCode aMortonCode );
	double VoxelEdgeLen;
	CVector	MinVertex,MaxVertex;
	map<TMortonCode,COctant> Octant;
};
//---------------------------------------------------------------------------------------------------------------------
#endif
