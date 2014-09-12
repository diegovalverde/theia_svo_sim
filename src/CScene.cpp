#include "../include/CScene.h"
#include "../include/Common.h"
#include <fstream>
#include <stdlib.h>


#define OBJ_TYPE_RAW_VERTEX "v"
#define OBJ_TYPE_FACE       "f"

//----------------------------------------------------------------------------------
CScene::CScene()
{
	mInitialized		= false;
	mIsVoxelFileLoaded	= false;
	ResolutionWidth     = 0;
	ResolutionHeight    = 0;

	OCtree.Geometry = &Geometry;
}
//----------------------------------------------------------------------------------
void CScene::PopulateOctree()
{

}
//----------------------------------------------------------------------------------
void CScene::Initailize( int aResolutionWidth, int aResolutionHeight )
{
	ResolutionHeight = aResolutionHeight;
	ResolutionWidth  = aResolutionWidth;

	mInitialized = true;
}
//----------------------------------------------------------------------------------
string GetNextObjValidLine( ifstream & ifs )
{
	string CurrentLine;
	int pos;
	do
	{
		std::getline(ifs,CurrentLine);

		//Skip Comments
		if ((pos = CurrentLine.find("#")) != string::npos )
			CurrentLine.erase(pos, string::npos);

		
	} while (CurrentLine.empty() && ifs.good() );


	return CurrentLine;
}
//----------------------------------------------------------------------------------
void CScene::LoadObject( string aFileName )
{
	CVector MaxVec(0,0,0),MinVec(0,0,0);
	LOG("Reading Obj File " + aFileName);

	ifstream ifs( aFileName.c_str() );
	if (!ifs.good() )
	{
		cout << "File not found " << aFileName << "\n";
		exit(1);
	}
	
	string CurrentLine;
	int pos = 0;
	

	while (!ifs.eof())
	{
		CurrentLine = GetNextObjValidLine(ifs);

		//Tokenize current line
		vector<string> Tokens;
		Tokenize(CurrentLine,Tokens," ");
		if (Tokens.empty())
			continue;

				
		
		int AddedVertices = 0;
		while (Tokens.size() && Tokens[0] == OBJ_TYPE_RAW_VERTEX )
		{
			Geometry.VertexBuffer.push_back( CVector( StringVecToIntVec( Tokens ) ) );
	
			CurrentLine = GetNextObjValidLine(ifs);
			Tokens.clear();
			Tokenize(CurrentLine,Tokens," ");
			AddedVertices++;
		
		}
		if (AddedVertices)
			cout << "Loaded " << AddedVertices << " vertices\n";
	

		int AddedFaces = 0;
		while (Tokens.size() && Tokens[0] == OBJ_TYPE_FACE )
		{
			if (Tokens.size() > 4)
				throw string("Only triangular primitives are supported");

			for (int i = 1; i <= 3; i++)
				if (Tokens[i].find("/") != string::npos)
					throw string("Textured Obj input models not supported\n");

			CFace<3> Face;
			Face.VertexIndex[0] = StringToInt( Tokens[1] ) - 1;
			Face.VertexIndex[1] = StringToInt( Tokens[2] ) - 1;
			Face.VertexIndex[2] = StringToInt( Tokens[3] ) - 1;
			Geometry.FaceBuffer.push_back( Face );



			CurrentLine = GetNextObjValidLine(ifs);
			Tokens.clear();
			Tokenize(CurrentLine,Tokens," ");
			AddedFaces++;
		
		}
		if (AddedFaces)
			cout << "Loaded " << AddedFaces << " faces\n";
		
		
	}

	LOG("Done");
	ifs.close();

	mIsVoxelFileLoaded = true;
}
//----------------------------------------------------------------------------------
#if 0
void CScene::LoadVoxels( string aFileName )
{

	CVector MaxVec(0,0,0),MinVec(0,0,0);
	LOG("Reading Obj File " + aFileName);

	ifstream ifs( aFileName.c_str() );
	if (!ifs.good() )
	{
		cout << "File not found " << aFileName << "\n";
		exit(1);
	}
	
	string CurrentLine;
	int pos = 0;
	

	while (!ifs.eof())
	{
		CurrentLine = GetNextObjValidLine(ifs);

		//Tokenize current line
		vector<string> Tokens;
		Tokenize(CurrentLine,Tokens," ");
		if (Tokens.empty())
			continue;

				
		
		int AddedVertices = 0;
		while (Tokens[0] == OBJ_TYPE_RAW_VERTEX )
		{
			Geometry.VertexBuffer.push_back( CVector( StringVecToIntVec( Tokens ) ) );
	
			CurrentLine = GetNextObjValidLine(ifs);
			Tokens.clear();
			Tokenize(CurrentLine,Tokens," ");
			AddedVertices++;
		
		}
		if (AddedVertices)
			cout << "Loaded " << AddedVertices << " vertices\n";
	
		
		int AddedFaces = 0;
		while (ifs.good() && Tokens[0] == OBJ_TYPE_FACE )
		{
			CFace<4> Face(StringVecToIntVec( Tokens ));
			Geometry.FaceBuffer.push_back( Face );
			
			CurrentLine = GetNextObjValidLine(ifs);
			Tokens.clear();
			Tokenize(CurrentLine,Tokens," ");
			
			AddedFaces++;
			
		}
		if (AddedFaces)
			cout << "Loaded " << AddedFaces << " faces\n";
		
		
	}

	LOG("Done");
	ifs.close();

	mIsVoxelFileLoaded = true;
}
#endif

//----------------------------------------------------------------------------------
