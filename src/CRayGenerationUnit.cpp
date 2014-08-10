#include "../include/CRayGenertionUnit.h"

//-------------------------------------------------------------------------------------------
CRayGenerationUnit::CRayGenerationUnit( )
{
	
}
//-------------------------------------------------------------------------------------------
CRayGenerationUnit::~CRayGenerationUnit()
{

}
//-------------------------------------------------------------------------------------------
CVector CRayGenerationUnit::GetPlanePoint( int aRow , int aColumn )
{
	float NormalizedRow   = (float) (((float)aRow    / Scene->ResolutionWidth ) - 0.5f);
	float NormalizedColum = (float) (((float)aColumn / Scene->ResolutionHeight) - 0.5f);

	  CVector Temp_i((NormalizedRow   * Scene->Camera.Right.x), (NormalizedRow   * Scene->Camera.Right.y) ,(NormalizedRow   * Scene->Camera.Right.z ));
	  CVector Temp_j((NormalizedColum * Scene->Camera.Up.x   ), (NormalizedColum * Scene->Camera.Up.y   ) ,(NormalizedColum * Scene->Camera.Up.z    ));

	  CVector ImagePoint = (Temp_i + Temp_j);
	 
	  ImagePoint.XTraslation( Scene->Camera.Position.x );
	  ImagePoint.YTraslation( Scene->Camera.Position.y );
	  ImagePoint.ZTraslation( Scene->Camera.Position.z );

	  ImagePoint = ImagePoint + (Scene->Camera.Direction * Scene->Camera.FocalDistance );

	  return ImagePoint;
}
//-------------------------------------------------------------------------------------------
CRay CRayGenerationUnit::Execute(int aRow, int aColumn )
{
	CRay Ray;

	CVector Point     = GetPlanePoint(aRow,aColumn);
	CVector Direction = Point - Scene->Camera.Position;
	Direction.Normalize();

	 
	Ray.Direction    =    Direction;
	Ray.Origen       =    Scene->Camera.Position;
	Ray.InvDirection =    CVector(10.f/Ray.Direction.x,10.f/Ray.Direction.y,10.f/Ray.Direction.z);

	
	Statistics->TotalRayCount++;
	return Ray;
}
//-------------------------------------------------------------------------------------------