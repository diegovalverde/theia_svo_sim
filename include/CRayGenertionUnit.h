#include "Geometry.h"
#include "../include/CCamera.h"
#include "../include/CStatistics.h"
#include "../include/CScene.h"

//------------------------------------------------------------------
class CRayGenerationUnit
{
public:
	CRayGenerationUnit();
	~CRayGenerationUnit();

	

public:
	CRay Execute(int aRow, int aColumn);
	CStatistics * Statistics;
	CScene      * Scene;

private:
	CVector GetPlanePoint( int aRow , int aColumn );
	
};
//------------------------------------------------------------------
