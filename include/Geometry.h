#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

typedef unsigned int TMortonCode;
#define NULL_MORTON_CODE ((TMortonCode)0)
#define ROOT_MORTON_CODE ((TMortonCode)1)

using namespace std;
#include <map>
#include <vector>
#include <string>
#include <math.h>
#include <fstream>
#include <iostream>

//------------------------------------------------------------------------------
class CVector
{
public:
	CVector(){ x=y=z=0.0f; }
	CVector( double aX, double aY, double aZ ) : x(aX),y(aY),z(aZ) {  }
	CVector( vector<double> Data )
	{
		x = Data[0];
		y = Data[1];
		z = Data[2];
	}
	~CVector(){};

	void Normalize( void )
	{
		double Length = 
		sqrt( x*x + y*y + z*z );

		x /= Length;
		y /= Length;
		z /= Length;
	};

	double Lenght( void )
	{
		return sqrt( x*x + y*y + z*z );
	}

	void Scale( double S )
	{
			x *= S;
			y *= S;
			z *= S;
	}

	void XTraslation( double aX )
	{
		x += aX;
	}

	void YTraslation( double aY )
	{
		 y += aY;
	}

	void ZTraslation( double aZ )
	{
		z += aZ;
	}

	void RotationYAxis(double Theta ) 
	{
		double _Z,_X;
		
		 _Z = z * cos( Theta ) - x * sin( Theta );
		 _X = z * sin( Theta ) + x * cos( Theta );

		z = _Z;
		x = _X;
		
	}
	void RotationXAxis(double Theta ) 
	{
		double _Y,_Z;
		
		 _Y = y * cos( Theta ) - z * sin( Theta );
		 _Z = y * sin( Theta ) + z * cos( Theta );

		z = _Z;
		y = _Y;
		
	}
	void RotationZAxis(double Theta )  
	{
		double _X,_Y;
		
		 _X = x * cos( Theta ) - y * sin( Theta );
		 _Y = x * sin( Theta ) + y * cos( Theta );

		x = _X;
		y = _Y;
		
	}

	const CVector operator + (const CVector & V)
	{
		return CVector(this->x + V.x, this->y + V.y, this->z + V.z);
	}

	const CVector operator - (const CVector & V)
	{
		return CVector(this->x - V.x, this->y - V.y, this->z - V.z);
	}

	const CVector operator * (const double Val )
	{
		return CVector(this->x * Val, this->y * Val , this->z * Val);
	}

	const CVector operator / (const double Val )
	{
		return CVector(this->x / Val, this->y / Val , this->z / Val);
	}

public:
	double x,y,z;

};

//-----------------------------------------------------------------------
class CTriangle
{
public:
	CTriangle()
	{
		Vertex.push_back( CVector() );
		Vertex.push_back( CVector() );
		Vertex.push_back( CVector() );
	}

	
	vector<CVector>		Vertex;
	
	void Scale( float S)
	{
		for (int i = 0; i < 3; i++)
			Vertex[i].Scale( S );
	}

	void XTraslation( float X )
	{
		for (int i = 0; i < 3; i++)
			Vertex[i].XTraslation( X );
	}

	void YTraslation( float Y )
	{
		for (int i = 0; i < 3; i++)
			Vertex[i].YTraslation( Y );
	}

	void ZTraslation( float Z )
	{
		for (int i = 0; i < 3; i++)
			Vertex[i].ZTraslation( Z );
	}

	//http://en.wikipedia.org/wiki/Euler_angles
	void RotationYAxis(double Theta ) //WORKS FINE :)
	{
		for (int i = 0; i < 3; i++)
			Vertex[i].RotationYAxis( Theta );
	}
	void RotationXAxis(double Theta ) 
	{
		for (int i = 0; i < 3; i++)
			Vertex[i].RotationXAxis( Theta );
	}
	void RotationZAxis(double Theta )  
	{
		for (int i = 0; i < 3; i++)
			Vertex[i].RotationZAxis( Theta );
	}
};
//-----------------------------------------------------------------------
inline CVector Cross(  CVector V1, CVector V2 )
{
	CVector  D;
	 D.x	= V1.y * V2.z - V1.z * V2.y; 
     D.y	= V1.z * V2.x - V1.x * V2.z; 
     D.z	= V1.x * V2.y - V1.y * V2.x;

	 return D;
}
//-----------------------------------------------------------------------
inline double Dot( CVector V1, CVector V2 )
{
	return (V1.x * V2.x + V1.y * V2.y + V1.z * V2.z );
}
//------------------------------------------------------------------------------
class CRay
{
public:
	CRay(){ mMaxLenght = 100.0f;};
	CRay(CVector aOrigen,CVector aDirection) : Origen(aOrigen),Direction(aDirection){};
	~CRay(){}
public:
	void WriteObj( string aPath )
	{
		ofstream ofs(aPath.c_str());
		if (!ofs.good())
			throw string("Could not open file + '" + aPath + "'\n");
		ofs << "#Vertices\n";
		char Buffer[2048];
		string strOut;

		sprintf_s(Buffer,"v %.5f %.5f %.5f\n", Origen.x,Origen.y,Origen.z);
		strOut += Buffer;
		strOut += Buffer;
		sprintf_s(Buffer,"v %.5f %.5f %.5f\n", 
			(Origen.x + mMaxLenght*Direction.x) ,
			(Origen.x + mMaxLenght*Direction.y) ,
			(Origen.x + mMaxLenght*Direction.z) 
			);
		strOut += Buffer;
		strOut += Buffer;
		ofs << strOut;
	
		ofs << "#Faces\n";
		ofs << "f 2 1 3 4\n";
		ofs.close();
		
	}
	CVector Origen;
	CVector Direction;
	CVector InvDirection;
	double  mMaxLenght;
};
//-----------------------------------------------------------------------
class CQuad
{
public:
	void Scale( double S)
	{
		for (int i = 0; i < 4; i++)
			Vertex[i].Scale( S );
	}

	void XTraslation( double X )
	{
		for (int i = 0; i < 4; i++)
			Vertex[i].x += X;


	}

	void YTraslation( double Y )
	{
		for (int i = 0; i < 4; i++)
			Vertex[i].y += Y;

	
	}

	void ZTraslation( double Z )
	{
		for (int i = 0; i < 4; i++)
			Vertex[i].z += Z;

	
	}

	void RotationYAxis(double Theta ) //WORKS FINE :)
	{
		for (int i = 0; i < 4; i++)
			Vertex[i].RotationYAxis( Theta );

		
		
	}
	void RotationXAxis(double Theta ) 
	{
		for (int i = 0; i < 4; i++)
			Vertex[i].RotationXAxis( Theta );

		
	}
	void RotationZAxis(double Theta )  
	{
		for (int i = 0; i < 4; i++)
			Vertex[i].RotationZAxis( Theta );

	
	}

public:
	CVector		Vertex[4];
	CVector		Up,Right;
};
//------------------------------------------------------------------------------
template <int N> class CFace
{
public:
	CFace(){}
	CFace( vector<double> aData )
	{
		if (aData.size() > N)
			throw "Error: Invalid number of elements\n";

		for (int i = 0; i < N; i++)
			VertexIndex[i] = (unsigned int) aData[i];
	}

public:
	unsigned int VertexIndex[ N ];
};


//---------------------------------------------------------------------------------------------------------------------
class CGeometry
{


public:
	vector< CVector  > 				VertexBuffer;			///The list of vertex
	vector< CFace<3> > 				FaceBuffer;				///The list of faces. Each face has 4 indexes to mVertex
	
};
//---------------------------------------------------------------------------------------------------------------------
class CVoxel
{
public:
	CVoxel() {  }

public:
	vector<int>		mVertex;		//Indexes into Scene.mVertexBuffer
};


#endif
