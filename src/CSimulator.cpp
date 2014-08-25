#include "../include/CSimulator.h"
#include "../../include/Common.h"

#include <iostream>
#include <fstream>


#define NULL_MORTON_IDX 0


//--------------------------------------------------------------------------------------------
string CallBack_Exit(vector<string> aArg, CSimulator * aSim )
{
	exit(0);
}
//--------------------------------------------------------------------------------------------
string CallBack_Load(vector<string> aArg, CSimulator * aSim )
{
	if (aArg.size() != 2)
		return "invalid number of arguments";
	string OperationType = aArg[0];
	string FilePath      = aArg[1];

	if (OperationType == "config")
		return aSim->LoadConfigurationFile(FilePath);
	else
		return OperationType + " : invalid load type";
}
//--------------------------------------------------------------------------------------------
string CallBack_Save(vector<string> aArg, CSimulator * aSim )
{
	if (aArg.size() != 2)
		return "invalid number of arguments";

	string OperationType = aArg[0];
	string FilePath      = aArg[1];

	if (OperationType == "octree")
	{
		aSim->Scene.OCtree.DumpWireFrame( FilePath, COctree::DUMP_LEAFS );
		return "Done saving Octree";
	} else
	if (OperationType == "projection_plane")
	{
		aSim->Scene.Camera.WriteProjectionPlaneObj(FilePath);
		return "Done saving projection plane";
	}
	else
		return OperationType + " : invalid save type";
}
//--------------------------------------------------------------------------------------------
string CallBack_Render(vector<string> aArg, CSimulator * aSim )
{
	if (aArg.size() != 1)
		return "invalid number of arguments";


	aSim->RenderFrame( aArg[0] );
	return "Render done";

}
//--------------------------------------------------------------------------------------------
CSimulator::CSimulator()
{
	
	mCommands["load"]		=  CallBack_Load;
	mCommands["save"]		=  CallBack_Save;
	mCommands["quit"]		=  CallBack_Exit;
	mCommands["render"]		=  CallBack_Render;

	
}
//--------------------------------------------------------------------------------------------
CSimulator::~CSimulator()
{
}
//--------------------------------------------------------------------------------------------
string CSimulator::ExecuteCommand(vector<string> aArguments)
{
	if (aArguments.size() < 1)
		return  "Invalid command line size";

	string Command = aArguments[0];
	aArguments.erase( aArguments.begin() );

	if (mCommands.find( Command  ) == mCommands.end())
		return Command + " : Command not found";

	return mCommands[Command]( aArguments, this );
}
//--------------------------------------------------------------------------------------------
string CSimulator::LoadConfigurationFile( string aFileName  )
{
		ifstream ifs( aFileName.c_str() );
	if (!ifs.good() )
	  return  aFileName + " : File not found ";
		
	vector<string> Tokens;
	while (ifs.good())
	{
		Tokens.clear();
		string CurrentLine;
		std::getline(ifs,CurrentLine);
		string::size_type pos = 0;
		
		//Get rid of comments
		if ((pos = CurrentLine.find("//")) != string::npos )
			CurrentLine.erase(pos,string::npos);

		//replace tabs by spaces
		while ((pos = CurrentLine.find(" ")) != string::npos)
			CurrentLine.erase(pos,1);
		
		if (CurrentLine.empty())
			continue;

		Tokenize(CurrentLine,Tokens,"=");
		string TokenType = Tokens[0];

		if (TokenType == "VoxelObjectFilePath")
		{
			//Scene.LoadVoxels( Tokens[1] );
			Scene.LoadObject( Tokens[1]  );
			continue;
		}

		if (TokenType == "Scene.OctreeDepth")
		{
			Scene.OCtree.SetDepth( StringToInt( Tokens[1] ));
			continue;
		}

		
		if (TokenType == "Scene.Width" )
		{
			
			Scene.ResolutionWidth = StringToInt( Tokens[1] );
			continue;
		}

		if (TokenType == "Scene.Height" )
		{
			Scene.ResolutionHeight = StringToInt( Tokens[1] );
			continue;
		}

		if (TokenType == "Camera.Position.x" )
		{
			Scene.Camera.Position.x = StringToInt( Tokens[1] );
			continue;
		}
		
		if (TokenType == "Camera.Position.y" )
		{
			Scene.Camera.Position.y = StringToInt( Tokens[1] );
			continue;
		}
		
		if (TokenType == "Camera.Position.z" )
		{
			Scene.Camera.Position.z = StringToInt( Tokens[1] );
			continue;
		}

		if (TokenType == "Camera.Rotation.x" )
		{
			
			Scene.Camera.EulerRotation.x = DegToRadians(  StringToInt( Tokens[1] ) );
			continue;
		}

		if (TokenType == "Camera.Rotation.y" )
		{
			Scene.Camera.EulerRotation.y = DegToRadians(  StringToInt( Tokens[1] ) );
			continue;
		}

		if (TokenType == "Camera.Rotation.z")
		{
			Scene.Camera.EulerRotation.z = DegToRadians(  StringToInt( Tokens[1] ) );
			continue;
		}

		if (CurrentLine.find("Camera.FocalDistance") != string::npos )
		{
			Scene.Camera.FocalDistance =   StringToInt( Tokens[1]  );
			continue;
		}


	}//while

	return "File Loaded";
}
//--------------------------------------------------------------------------------------------
void CSimulator::Initialize( string aFileName )
{
	Gpu.Rgu.Scene = &Scene;
	Gpu.Rgu.Statistics = &Statistics;
	Gpu.Gt.Scene = &Scene;



	Scene.Camera.Initialize();
}
//--------------------------------------------------------------------------------------------
void CSimulator::RenderFrame( string aFileName )
{
	ofstream ofs(aFileName.c_str());
	ofs << "P3\n";
	ofs << Scene.ResolutionWidth << " " << Scene.ResolutionHeight << "\n";
	ofs << "255\n";
	if (!ofs.good())
		throw string("Could not open file for writting\n");
	
	for (int j = 0; j < Scene.ResolutionHeight; j++)
	{
		
		for (int i = 0; i < Scene.ResolutionWidth; i++)
		{
			CRay Ray =  Gpu.Rgu.Execute( i,j );
			TMortonCode  GtResut = Gpu.Gt.Execute( Ray );

			if (GtResut == NULL_MORTON_IDX)
			{
				Statistics.TotalRayNotIntersectingVoxel++;
				ofs << " 0 0 0\n";	
			} else {
				cout << ".";
				ofs << "#" << GtResut << "\n";
				ofs << " 255 255 255\n";	
			}
		}
	}

	ofs.close();
}
//--------------------------------------------------------------------------------------------