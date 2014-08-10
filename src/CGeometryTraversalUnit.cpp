#include "../include/CGeometryTraversalUnit.h"

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
	TMortonCode Result;
	Result = Scene->OCtree.GetIntersectedVoxel( ROOT_MORTON_CODE, aRay );

	return Result;
}
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------