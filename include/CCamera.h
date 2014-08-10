#ifndef CCAMERA_H_INCLUDED
#define CCAMERA_H_INCLUDED
#include "../../include/Geometry.h"
#include <fstream>

class CCamera
{
public:
	CCamera(){};
	~CCamera(){};

void Initialize( void )
{
	
	ProPlane.Vertex[0] = CVector(  1.0f,  1.0f, 0.0f );
	ProPlane.Vertex[1] = CVector( -1.0f,  1.0f, 0.0f );
	ProPlane.Vertex[2] = CVector( -1.0f, -1.0f, 0.0f );
	ProPlane.Vertex[3] = CVector(  1.0f, -1.0f, 0.0f );
	ProPlane.Right = ProPlane.Vertex[1] - ProPlane.Vertex[0];
	ProPlane.Up    = ProPlane.Vertex[3] - ProPlane.Vertex[0];

	ProPlane.RotationXAxis( EulerRotation.x );
	ProPlane.RotationYAxis( EulerRotation.y );
	ProPlane.RotationZAxis( EulerRotation.z );

	ProPlane.XTraslation( Position.x );
	ProPlane.YTraslation( Position.y );
	ProPlane.ZTraslation( Position.z );

	CVector ProPlaneNormal = Cross((ProPlane.Vertex[3] - ProPlane.Vertex[0]),(ProPlane.Vertex[1] - ProPlane.Vertex[0]));

	ProPlaneNormal.Normalize();
	float FFF = 1.0f;
	ProPlane.XTraslation( ProPlaneNormal.x*FFF );
	ProPlane.YTraslation( ProPlaneNormal.y*FFF );
	ProPlane.ZTraslation( ProPlaneNormal.z*FFF );

	Right = ProPlane.Vertex[1] - ProPlane.Vertex[0];
	Up    = ProPlane.Vertex[3] - ProPlane.Vertex[0];
	Direction = Cross(Up,Right);
	Direction.Normalize();
}

CVector Up;
CVector Right;
CVector Position;
CVector Direction;
CVector EulerRotation;
float   FocalDistance;
CQuad ProPlane;

void WriteProjectionPlaneObj( string aPath )
{
	ofstream ofs( aPath.c_str() );
	if ( !ofs.good())
		throw string("Cannot open '" + aPath + " for writting\n");

	ofs << "#Projection plane\n";
	string strOut;
	char Buffer[2048];
	for (int i = 0; i < 4; i++)
	{
		sprintf(Buffer,"v %.5f %.5f %.5f\n", ProPlane.Vertex[i].x,ProPlane.Vertex[i].y,ProPlane.Vertex[i].z);
		strOut += Buffer;
	}
	ofs << strOut;
	ofs << "#Faces\n";
	ofs << "f 1 2 3 4\n";


	ofs.close();
}
};


#endif