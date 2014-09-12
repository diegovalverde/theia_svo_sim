#include "../include/COctree.h"
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>

#define VOXELIZE 1
//-------------------------------------------------------------------------------------------------------------
int hibit(unsigned int n) 
{
   
	
	return (int)(log((double)n)/log((double)2));
}

//------------------------------------------------------------------------------------------------
string COctant::DumpVertex()
{
	string strOut;
	char Buffer[2048];
		
		double L = Lenght;

		sprintf(Buffer,"v %.5f %.5f %.5f\n", MinVertex.x       ,MinVertex.y         ,MinVertex.z       );
		strOut += Buffer;
		sprintf(Buffer,"v %.5f %.5f %.5f\n", MinVertex.x + L   ,MinVertex.y         ,MinVertex.z       );
		strOut += Buffer;
		sprintf(Buffer,"v %.5f %.5f %.5f\n", MinVertex.x + L   ,MinVertex.y + L     ,MinVertex.z       );
		strOut += Buffer;
		sprintf(Buffer,"v %.5f %.5f %.5f\n", MinVertex.x       ,MinVertex.y + L     ,MinVertex.z       );
		strOut += Buffer;
		sprintf(Buffer,"v %.5f %.5f %.5f\n", MinVertex.x       ,MinVertex.y         ,MinVertex.z  + L  );
		strOut += Buffer;
		sprintf(Buffer,"v %.5f %.5f %.5f\n", MinVertex.x + L   ,MinVertex.y         ,MinVertex.z  + L  );
		strOut += Buffer;
		sprintf(Buffer,"v %.5f %.5f %.5f\n", MinVertex.x + L   ,MinVertex.y + L     ,MinVertex.z  + L  );
		strOut += Buffer;
		sprintf(Buffer,"v %.5f %.5f %.5f\n", MinVertex.x       ,MinVertex.y + L     ,MinVertex.z  + L  );
		strOut += Buffer;

	

		return strOut;
}
//------------------------------------------------------------------------------------------------
string COctant::DumpFaces(int aFaceOffset )
{
	stringstream ss;
	ss << "\n";
	
	ss << "f " << (5+aFaceOffset)  << " " << (6+aFaceOffset)  << " " << (2+aFaceOffset)  << " " << (1+aFaceOffset) << "\n";
	ss << "f " << (6+aFaceOffset)  << " " << (7+aFaceOffset)  << " " << (3+aFaceOffset)  << " " << (2+aFaceOffset) << "\n";
	ss << "f " << (7+aFaceOffset)  << " " << (8+aFaceOffset)  << " " << (4+aFaceOffset)  << " " << (3+aFaceOffset) << "\n";
	ss << "f " << (8+aFaceOffset)  << " " << (5+aFaceOffset)  << " " << (1+aFaceOffset)  << " " << (4+aFaceOffset) << "\n";
	ss << "f " << (1+aFaceOffset)  << " " << (2+aFaceOffset)  << " " << (3+aFaceOffset)  << " " << (4+aFaceOffset) << "\n";
	ss << "f " << (8+aFaceOffset)  << " " << (7+aFaceOffset)  << " " << (6+aFaceOffset)  << " " << (5+aFaceOffset) << "\n";
	return ss.str();
		
}
//------------------------------------------------------------------------------------------------
void COctree::InitializeRootOctant()
{

	COctant Root;
	Root.MaxVertex = MaxVertex;
	Root.MinVertex = MinVertex;
	Root.isEmpty   = false;
	Root.isLeaf    = (mMaxDepth == 1);

	Root.Lenght =  CVector(MaxVertex - MinVertex).Lenght();

	
	for (int i = 0; i < Geometry->FaceBuffer.size(); i++)
	{
		CTriangle Triangle;
		Triangle.Vertex[0] = Geometry->VertexBuffer[  Geometry->FaceBuffer[i].VertexIndex[0] ] ;
		Triangle.Vertex[1] = Geometry->VertexBuffer[  Geometry->FaceBuffer[i].VertexIndex[1] ] ;
		Triangle.Vertex[2] = Geometry->VertexBuffer[  Geometry->FaceBuffer[i].VertexIndex[2] ] ;

		if (Root.TriangleIntersection( Triangle ) )
			Root.FaceIndexList.push_back( Geometry->FaceBuffer[i] );
	}

	if (Root.FaceIndexList.empty())
		Root.isEmpty = true;

	double L = Root.Lenght;
	Root.Center = CVector(MinVertex.x + L/2, MinVertex.y + L/2,MinVertex.z + L/2);
	Octant[ ROOT_MORTON_CODE ] = Root;

}
//------------------------------------------------------------------------------------------------
void COctree::Populate(  CGeometry & aGeometry )
{

	cout << "Start Populating Otree\n";
	CreateMainBoundigCube();

	InitializeRootOctant();

	for (int i = 0; i < 8; i++)
	{
		CreateOctant(Octant[ROOT_MORTON_CODE].Center,Octant[ROOT_MORTON_CODE].Lenght,0x8 + i);
		cout << "Done " << i << "/8\n";
	}
	
	cout << "Done Populating Otree\n";
	cout << "Created " << Octant.size() << " octants\n";

}
//------------------------------------------------------------------------------------------------
CVector COctree::GetCenterFromMortonKey( TMortonCode aMortonKey, CVector aParentCenter, float aParentDiameter )
{
	int Level = hibit(aMortonKey)/3;

	
	float Size = aParentDiameter/4;
	int aOctant = aMortonKey & 0x7;
	switch (aOctant)
		{
		case TOP_FRONT_LEFT:
			return CVector( aParentCenter.x - Size, aParentCenter.y + Size, aParentCenter.z + Size); 

		case TOP_FRONT_RIGHT:
			return CVector( aParentCenter.x + Size, aParentCenter.y + Size, aParentCenter.z + Size); 
			
		case TOP_BACK_LEFT:
			return CVector( aParentCenter.x - Size, aParentCenter.y + Size, aParentCenter.z - Size); 
			

		case TOP_BACK_RIGHT:
			return CVector( aParentCenter.x + Size, aParentCenter.y + Size, aParentCenter.z - Size); 
			

		case BOTTOM_FRONT_LEFT:
			return CVector( aParentCenter.x - Size, aParentCenter.y - Size, aParentCenter.z + Size);  

		case BOTTOM_FRONT_RIGHT:
			return CVector( aParentCenter.x + Size, aParentCenter.y - Size, aParentCenter.z + Size); 

		case BOTTOM_BACK_LEFT:
			return CVector( aParentCenter.x - Size, aParentCenter.y - Size, aParentCenter.z - Size); 

		case BOTTOM_BACK_RIGHT:
			return CVector( aParentCenter.x + Size, aParentCenter.y - Size, aParentCenter.z - Size); 
		}

};
//------------------------------------------------------------------------------------------------
bool  COctant::ContainsVertex( CVector aVertex )
{
	if (aVertex.x >= MaxVertex.x)
		return false;

	if (aVertex.y >= MaxVertex.y)
		return false;

	if (aVertex.z >= MaxVertex.z)
		return false;
	
	if (aVertex.x <= MinVertex.x)
		return false;

	if (aVertex.y <= MinVertex.y)
		return false;

	if (aVertex.z <= MinVertex.z)
		return false;

	return true;
}
//------------------------------------------------------------------------------------------------
void COctree::CreateOctant( CVector aParentCenter, double aParentDiameter, TMortonCode aMortonCode )
{

	
	COctant NewOctant;
	CVector Center = GetCenterFromMortonKey(aMortonCode,aParentCenter,aParentDiameter);
	float L =aParentDiameter/2;
	NewOctant.MaxVertex    = CVector(   Center.x + L/2, Center.y + L/2, Center.z + L/2);
	NewOctant.MinVertex    = CVector( Center.x - L/2, Center.y - L/2, Center.z - L/2);
	NewOctant.VoxelEdgeLen = VoxelEdgeLen;
	NewOctant.Lenght       = L;
	NewOctant.isEmpty      = false;
	NewOctant.Center       = Center;

	TMortonCode ParentMortonCode = (aMortonCode >> 3);

	for (int i = 0; i < Octant[ParentMortonCode].FaceIndexList.size(); i++)
	{
		
		CTriangle Triangle;
		Triangle.Vertex[0] = Geometry->VertexBuffer[ Octant[ParentMortonCode].FaceIndexList[i].VertexIndex[0] ] ;
		Triangle.Vertex[1] = Geometry->VertexBuffer[ Octant[ParentMortonCode].FaceIndexList[i].VertexIndex[1] ] ;
		Triangle.Vertex[2] = Geometry->VertexBuffer[ Octant[ParentMortonCode].FaceIndexList[i].VertexIndex[2] ] ;
		
		if (NewOctant.TriangleIntersection( Triangle ) )
			NewOctant.FaceIndexList.push_back( Octant[ParentMortonCode].FaceIndexList[i] );
	}

	if (NewOctant.FaceIndexList.size() == 0)
	{
		NewOctant.isEmpty   = true;
		return;
	}

	int Level = hibit(aMortonCode)/3;
	if (Level == mMaxDepth)
		NewOctant.isLeaf = true;

	Octant[aMortonCode] = NewOctant;

	int NextLevel = hibit(aMortonCode << 3)/3;
	if (NextLevel > mMaxDepth)
		return;


	for (int i = 0; i < 8; i++)
		CreateOctant(Center,L, ((aMortonCode << 3) + i));

	
}
//------------------------------------------------------------------------------------------------

void COctree::CreateMainBoundigCube()
{

	//Find the Max and Min Vertex
		for (size_t i = 0; i < Geometry->VertexBuffer.size(); i++)
		{

			if (Geometry->VertexBuffer[i].x > MaxVertex.x) 
				 MaxVertex.x = Geometry->VertexBuffer[i].x;

			if (Geometry->VertexBuffer[i].y > MaxVertex.y) 
				 MaxVertex.y = Geometry->VertexBuffer[i].y;

			if (Geometry->VertexBuffer[i].z > MaxVertex.z) 
				 MaxVertex.z = Geometry->VertexBuffer[i].z;

			if (Geometry->VertexBuffer[i].x < MinVertex.x) 
				 MinVertex.x = Geometry->VertexBuffer[i].x;

			if (Geometry->VertexBuffer[i].y < MinVertex.y) 
				 MinVertex.y = Geometry->VertexBuffer[i].y;

			if (Geometry->VertexBuffer[i].z < MinVertex.z) 
				 MinVertex.z = Geometry->VertexBuffer[i].z;
		}


		
		
}
//------------------------------------------------------------------------------------------------
float getmin(const vector<CVector> &points, CVector axis)
{
  float min = std::numeric_limits<float>::max(); 

  for (int ctr = 0; ctr < points.size(); ctr++)
    {
      float dotprod = Dot(points[ctr], axis);
      if (dotprod < min) min = dotprod;
    }
  return min;
}
//------------------------------------------------------------------------------------------------
float getmax(const vector<CVector> &points, CVector axis)
{
  float max = -std::numeric_limits<float>::max(); 

  for (int ctr = 0; ctr < points.size(); ctr++)
    {
      float dotprod = Dot(points[ctr], axis);
      if (dotprod > max) max = dotprod;
    }
  return max;
}
//------------------------------------------------------------------------------------------------
bool isect(const vector<CVector> &points1, const vector<CVector> &points2, CVector axis)
{
  if (getmin(points1, axis) > getmax(points2, axis)) return false;
  if (getmax(points1, axis) < getmin(points2, axis)) return false;
  return true;     
}
//------------------------------------------------------------------------------------------------
bool COctant::TriangleIntersection( CTriangle aTriangle )
{ 

  //If any vertex is inside the cube, return true! 	
  
  for (int i = 0; i <  aTriangle.Vertex.size(); i ++)
  if (  ContainsVertex( aTriangle.Vertex[i] ) )
	  return true;

   
  vector<CVector> boxpoints;
  boxpoints.push_back(CVector(Center.x+Lenght, Center.y+Lenght, Center.z+Lenght));
  boxpoints.push_back(CVector(Center.x+Lenght, Center.y+Lenght, Center.z-Lenght));
  boxpoints.push_back(CVector(Center.x+Lenght, Center.y-Lenght, Center.z+Lenght));
  boxpoints.push_back(CVector(Center.x+Lenght, Center.y-Lenght, Center.z-Lenght));
  boxpoints.push_back(CVector(Center.x-Lenght, Center.y+Lenght, Center.z+Lenght));
  boxpoints.push_back(CVector(Center.x-Lenght, Center.y+Lenght, Center.z-Lenght));
  boxpoints.push_back(CVector(Center.x-Lenght, Center.y-Lenght, Center.z+Lenght));
  boxpoints.push_back(CVector(Center.x-Lenght, Center.y-Lenght, Center.z-Lenght));


  if (!isect(boxpoints, aTriangle.Vertex, CVector(1, 0, 0))) return false;
  if (!isect(boxpoints, aTriangle.Vertex, CVector(0, 1, 0))) return false;
  if (!isect(boxpoints, aTriangle.Vertex, CVector(0, 0, 1))) return false;
  

  // test the triangle normal
  CVector triedge1 = aTriangle.Vertex[1] - aTriangle.Vertex[0];
  CVector triedge2 = aTriangle.Vertex[2] - aTriangle.Vertex[1];
  CVector trinormal = Cross(triedge1, triedge2);
  if (!isect(boxpoints, aTriangle.Vertex, trinormal)) return false;

  // test the 9 edge cross products
  CVector triedge3 = aTriangle.Vertex[0] - aTriangle.Vertex[2];

  CVector boxedge1 = CVector(1, 0, 0);
  CVector boxedge2 = CVector(0, 1, 0);
  CVector boxedge3 = CVector(0, 0, 1);

  if (!isect(boxpoints, aTriangle.Vertex, Cross(boxedge1, triedge1))) return false;
  if (!isect(boxpoints, aTriangle.Vertex, Cross(boxedge1, triedge2))) return false;
  if (!isect(boxpoints, aTriangle.Vertex, Cross(boxedge1, triedge3))) return false;

  if (!isect(boxpoints, aTriangle.Vertex, Cross(boxedge2, triedge1))) return false;
  if (!isect(boxpoints, aTriangle.Vertex, Cross(boxedge2, triedge2))) return false;
  if (!isect(boxpoints, aTriangle.Vertex, Cross(boxedge2, triedge3))) return false;

  if (!isect(boxpoints, aTriangle.Vertex, Cross(boxedge3, triedge1))) return false;
  if (!isect(boxpoints, aTriangle.Vertex, Cross(boxedge3, triedge2))) return false;
  if (!isect(boxpoints, aTriangle.Vertex, Cross(boxedge3, triedge3))) return false;

  return true;

}

//------------------------------------------------------------------------------------------------
bool COctant::RayIntersection( CRay aRay )
{
	float tmin = (MinVertex.x - aRay.Origen.x) * aRay.InvDirection.x;
    float tmax = (MaxVertex.x - aRay.Origen.x) * aRay.InvDirection.x;
    if (tmin > tmax) swap(tmin, tmax);
    float tymin = (MinVertex.y - aRay.Origen.y) * aRay.InvDirection.y;
    float tymax = (MaxVertex.y - aRay.Origen.y) * aRay.InvDirection.y;
    if (tymin > tymax) swap(tymin, tymax);
    if ((tmin > tymax) || (tymin > tmax))
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    float tzmin = (MinVertex.z - aRay.Origen.z) * aRay.InvDirection.z;
    float tzmax = (MaxVertex.z - aRay.Origen.z) * aRay.InvDirection.z;
    if (tzmin > tzmax) 
		swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    return true;
}
//------------------------------------------------------------------------------------------------
TMortonCode COctree::GetIntersectedVoxel( TMortonCode aMortonCode,  CRay  aRay )
{
  
	TMortonCode IntersectedMortonCode = NULL_MORTON_CODE;

	if (Octant.find(aMortonCode) == Octant.end())
		return NULL_MORTON_CODE;

	if (Octant[ aMortonCode ].isEmpty)
		return NULL_MORTON_CODE;

	if ( Octant[ aMortonCode ].RayIntersection( aRay ) == false )
		return NULL_MORTON_CODE;

	if (Octant[ aMortonCode ].isLeaf)
		return aMortonCode;
	else
	{
		int NextLevel = hibit(aMortonCode << 3)/3;
		if (NextLevel > mMaxDepth)
			return IntersectedMortonCode;

		for (int i = 0; i < 8; i++)
		{
			if (( IntersectedMortonCode = GetIntersectedVoxel(((aMortonCode << 3) + i), aRay)) != NULL_MORTON_CODE)
				return IntersectedMortonCode;
			
		}
	}


   return NULL_MORTON_CODE;
}
//------------------------------------------------------------------------------------------------
void COctree::DumpWireFrame( string aFilePath, COctree::TDumpType aDumpType )
{
	ofstream ofs( aFilePath.c_str() );
	if (!ofs.good())
		throw string( "Could not open " + aFilePath + " for writting\n" );

	ofs << "#Dumping Octree vertex\n\n";
	ofs << "#Total octants: " << Octant.size() << "\n";
	map<TMortonCode,COctant>::iterator I = Octant.begin();
	while (I != Octant.end())
	{
		switch (aDumpType)
		{
		case DUMP_ALL:
			ofs << I->second.DumpVertex();
			break;
		case DUMP_LEAFS:
			if ( I->second.isLeaf)
				ofs << I->second.DumpVertex();
			break;
		case DUMP_NON_EMPTY:
			if ( !I->second.isEmpty)
				ofs << I->second.DumpVertex();
			break;
		}
		
		I++;
	}

	I = Octant.begin();
	int i = 0;
	ofs << "#Dumping Octree faces\n\n";
	while (I != Octant.end())
	{

		switch (aDumpType)
		{
		case DUMP_ALL:
			ofs << I->second.DumpFaces(8*i++);
			break;
		case DUMP_LEAFS:
			if ( I->second.isLeaf)
				ofs << I->second.DumpFaces(8*i++);
			break;
		case DUMP_NON_EMPTY:
			if ( !I->second.isEmpty)
				ofs << I->second.DumpFaces(8*i++);
			break;
		}

		
			
		I++; ;
	}

	ofs.close();
}
//------------------------------------------------------------------------------------------------
void COctree::DumpObjMainBoundingCube( string aFilePath)
{
		ofstream ofs( aFilePath.c_str() );
		if (!ofs.good())
		{
			cout << "Could not open " << aFilePath << " for writting\n";
		}
		
		double L = VoxelEdgeLen;


		ofs << "v " <<  MinVertex.x        << " " <<  MinVertex.y      << " " << MinVertex.z      << "\n";
		ofs << "v " << (MinVertex.x  + L ) << " " <<  MinVertex.y      << " " << MinVertex.z      << "\n";
		ofs << "v " << (MinVertex.x  + L ) << " " << (MinVertex.y + L) << " " << MinVertex.z      << "\n";
		ofs << "v " <<  MinVertex.x        << " " << (MinVertex.y + L) << " " << MinVertex.z      << "\n";


		ofs << "v " <<  MinVertex.x        << " " <<  MinVertex.y      << " " << (MinVertex.z +L)      << "\n";
		ofs << "v " << (MinVertex.x  + L ) << " " <<  MinVertex.y      << " " << (MinVertex.z +L)      << "\n";
		ofs << "v " << (MinVertex.x  + L ) << " " << (MinVertex.y + L) << " " << (MinVertex.z +L)      << "\n";
		ofs << "v " <<  MinVertex.x        << " " << (MinVertex.y + L) << " " << (MinVertex.z +L)      << "\n";


		ofs << "\n";
		ofs << "f 1 2 3 4\n";
		ofs << "f 5 6 7 8\n";

		ofs << "f 1 2 6 5\n";

		ofs.close();
}
//------------------------------------------------------------------------------------------------
