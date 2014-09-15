#ifndef GEOMETRY_TRAVERSAL_UNIT_INCLUDED
#define GEOMETRY_TRAVERSAL_UNIT_INCLUDED

#include "../include/Geometry.h"
#include "../include/CStatistics.h"
#include "../include/CScene.h"

//------------------------------------------------------------------
class CGeometryTraversalUnit
{
public:
	CGeometryTraversalUnit();
	~CGeometryTraversalUnit();

	typedef struct 
	{
		TMortonCode VoxelMortonIdx;
		CVector		VoxelIntersectionPoint;
	} Result;

public:
	TMortonCode Execute( CRay & aRay );		//Calculates intersection and all that stuff
	string		  DumpState( void );
	CScene      * Scene;
	CStatistics * Statistics;

private:
	bool RayIntersectsVoxel( void );
	
	

private:
	float mTmin;
	float mTmax;

	CVector mVmin;
	CVector mVmax;

	CVoxel	mVoxel;
	CRay	mRay;
	CVector mIntersectionPoint;
};
//------------------------------------------------------------------

#endif

