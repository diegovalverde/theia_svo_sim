#include "../include/CGeometryTraversalUnit.h"
#include "../include/Common.h"
#include "../include/Geometry.h"


#define WORD32 unsigned int
//----------------------------------------------------------------------------------
CGeometryTraversalUnit::CGeometryTraversalUnit()
{
	mTmin =  1000.0f;
	mTmax = -1000.0f;
}
//----------------------------------------------------------------------------------
CGeometryTraversalUnit::~CGeometryTraversalUnit()
{

}
//----------------------------------------------------------------------------------
TMortonCode CGeometryTraversalUnit::Execute( CRay & aRay )
{
	
	
	return Scene->OCtree.GetIntersectedVoxel( ROOT_MORTON_CODE, aRay );

	
}
//----------------------------------------------------------------------------------
TMortonCode CGeometryTraversalUnit::Execute( CRay & aRay, TMortonCode aCurrentMortonCode, E_OCTREE_TYPE aCurrentType )
{
	TMortonCode IntersectedMortonCode = NULL_MORTON_CODE;
	
	if (aCurrentType == EMPTY)
		return NULL_MORTON_CODE;

	if (aCurrentType == LEAF)
		return aCurrentMortonCode;

	

	//Step 1:Do we intersect?
	//The intersection method needs the octant max/min BB vertex (comes from center, which comes from morton code)
	if (Scene->OCtree.Octant.find( aCurrentMortonCode ) == Scene->OCtree.Octant.end())
	{
		//TODO: FIX ME!
		//cout << "-W- Cache had non-exisiting OCtant\n";
		return NULL_MORTON_CODE;
	}

	COctant CurrentOctant = Scene->OCtree.GetOCtant( aCurrentMortonCode );
	if ( CurrentOctant.RayIntersection( aRay ) == false )
		return NULL_MORTON_CODE;


	//Step 1: Are we a Leaf? We have children?
	int NextLevel = (GetHighestBitIndex(aCurrentMortonCode << 3) + 1)/3;
	if (NextLevel > Scene->OCtree.mParameter["depth"])
		return IntersectedMortonCode;

	
	//Read from memory
	CMemory::TAddress ReadAddress;
	ReadAddress.LogicAddr = aCurrentMortonCode;

	
	WORD32 ReadData = Memory->Read(ReadAddress);

	


	for (int i = 0; i < 8; i++)
	{
		//Get the node type. This is 2 bits for each of the 8 children
		E_OCTREE_TYPE NodeType = (E_OCTREE_TYPE)((ReadData & (0x3 << 2*i)) >> 2*i);

		if ( (IntersectedMortonCode = Execute( aRay, ((aCurrentMortonCode << 3) + i), NodeType)) != NULL_MORTON_CODE)
			return IntersectedMortonCode;
	}
	


	return IntersectedMortonCode;
	
}
//----------------------------------------------------------------------------------