#ifndef GEOMETRY_TRAVERSAL_UNIT_INCLUDED
#define GEOMETRY_TRAVERSAL_UNIT_INCLUDED

#include "../include/Geometry.h"
#include "../include/CStatistics.h"
#include "../include/CScene.h"
#include "../include/CMemory.h"


typedef struct T_NODETYPE16
{
	int Child0 : 2;
	int Child1 : 2;
	int Child2 : 2;
	int Child3 : 2;
	int Child4 : 2;
	int Child5 : 2;
	int Child6 : 2;
	int Child7 : 2;

} TNODETYPE16;


typedef union T_NODETYPE_16
{
	TNODETYPE16 Oct;
	WORD32      Value;
} TNODETYPE_16;



enum E_OCTREE_TYPE
{
	EMPTY,		//No leaf no children
	LEAF,
	NON_EMPTY,
	RESERVED

};


typedef union T_NODETYPEU
{
	int Value;
	E_OCTREE_TYPE Type;
} TNODETYPEU;
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
	TMortonCode Execute( CRay & aRay );										//Calculates intersection behavioural way
	TMortonCode Execute(CRay & aRay, TMortonCode aMyMortonCode, E_OCTREE_TYPE aMyType );		//Calculates intersection structural way
	string		  DumpState( void );
	CScene      * Scene;
	CStatistics * Statistics;
	CMemory     * Memory;

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

