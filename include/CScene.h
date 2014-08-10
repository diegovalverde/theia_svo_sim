#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED
#include "../../include/Geometry.h"
#include "../include/CCamera.h"
#include "../include/COctree.h"
//---------------------------------------------------------------------------------------------------------------------
class CScene
{

public:
	CScene() ;
	~CScene() {};

public:	
	void Initailize( int ResolutionWidth, int ResolutionHeight);
	void LoadVoxels( string aFileName );			///Loads voxels from wavefront's OBJ file
	void LoadObject( string aFileName );			///Loads mesh from wavefront's OBJ file
	void PopulateOctree( void );					///Populates the mOctree from the voxels obtained using the LoadVoxels method
	

private:	
	void LoadMaterials( string aFileName );			///Loads material from wavefront's MAT file

public:
	CGeometry Geometry;
	COctree	  OCtree;								///The actual octree
	CCamera   Camera;
	int       ResolutionWidth, ResolutionHeight;	///Widht and Height of the screen

private:
	bool	mInitialized;
	bool	mIsVoxelFileLoaded;						///Did we already call LoadVoxels?
};
//---------------------------------------------------------------------------------------------------------------------


#endif
