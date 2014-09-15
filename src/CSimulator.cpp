#include "../include/CSimulator.h"
#include "../include/Common.h"

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
	{
		string Out = aSim->LoadConfigurationFile(FilePath);
		aSim->Scene.Camera.Initialize();
		return Out;
	}
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
	#ifndef _WIN32
		system(string("display " + aArg[0] + " &").c_str());
	#endif
	return "Render output written into " +  aArg[0] ;

}
//--------------------------------------------------------------------------------------------
string CallBack_Show(vector<string> aArg, CSimulator * aSim )
{
	string ReturString;
	ostringstream oss;

	if (aArg.size() != 1)
		return "invalid number of arguments";
	string Type = aArg[0];
	if (Type == "config")
	{
		oss << "Camera.Position            = (" 
			<< aSim->Scene.Camera.Position.x << " , "  
			<< aSim->Scene.Camera.Position.y << " , "  
			<< aSim->Scene.Camera.Position.z << " , )\n"; 

		oss << "Camera.Direction           = (" 
			<< aSim->Scene.Camera.Direction.x << " , "  
			<< aSim->Scene.Camera.Direction.y << " , "  
			<< aSim->Scene.Camera.Direction.z << " , )\n"; 

		oss << "Camera.EulerRotation       = (" 
			<< aSim->Scene.Camera.EulerRotation.x << " , "  
			<< aSim->Scene.Camera.EulerRotation.y << " , "  
			<< aSim->Scene.Camera.EulerRotation.z << " , )\n"; 

		oss << "Camera.FocalDistance       = " << aSim->Scene.Camera.FocalDistance << "\n"; 

		oss << "Geometry.VertexBuffer.size = " << aSim->Scene.Geometry.VertexBuffer.size() << "\n";
		oss << "Geometry.FaceBuffer.size   = " << aSim->Scene.Geometry.FaceBuffer.size() << "\n";
		
	}
	else if (Type == "stat"	)
	{
		return aSim->Statistics.Print();
	} else {
		return "Invalid type '" + Type + "'\n";
	}

	return oss.str();
}
//--------------------------------------------------------------------------------------------
string CallBack_Set(vector<string> aArg, CSimulator * aSim )
{
	int Value;
	if (aArg.size() != 3)
		return "invalid number of arguments";

	string Object = aArg[0];
	string Attribute = aArg[1];
	stringstream ss(aArg[2]);
	 
	ss >> Value ;
	if (Object == "octree")
	{
		if (Attribute == "depth")
		{
			aSim->Scene.OCtree.SetDepth( Value );
		}
	} else
		return "Invalid type '" + Object + "'\n";

	return "Value was set";
}
//--------------------------------------------------------------------------------------------
string CallBack_Voxelize(vector<string> aArg, CSimulator * aSim )
{
	if (aArg.size() != 0)
		return "invalid number of arguments";

	aSim->Scene.OCtree.Populate( aSim->Scene.Geometry );
	return "Voxelization complete";
}
//--------------------------------------------------------------------------------------------
string CallBack_Help(vector<string> aArg, CSimulator * aSim )
{
	string Help =
		"save <octree|projection_plane> <filepath>        : saves specified object\n"
		"load <config>                                    : loads specified object\n"
		"show <config|stats>                              : shows config\n"
		"render <filepath>                                : renders PPM to specified file\n"
		"voxelize                                         : Voxelizes currently loaded model\n"
		"set <octree> <depth> <value>                     : sets values\n"
		"quit                                             : exits application\n";

	return Help;
		
}

//--------------------------------------------------------------------------------------------
CSimulator::CSimulator()
{
	
	mCommands["load"]		=  CallBack_Load;
	mCommands["save"]		=  CallBack_Save;
	mCommands["quit"]		=  mCommands["q"] = CallBack_Exit;
	mCommands["voxelize"]	=  CallBack_Voxelize;
	mCommands["render"]		=  CallBack_Render;
	mCommands["help"]		=  CallBack_Help;
	mCommands["show"]		=  CallBack_Show;
	mCommands["set"]        =  CallBack_Set;

	
}
//--------------------------------------------------------------------------------------------
CSimulator::~CSimulator()
{
}
//--------------------------------------------------------------------------------------------
string CSimulator::ExecuteCommand(vector<string> aArguments)
{
	if (aArguments.size() < 1)
		return  "\n";

	string Command = aArguments[0];
	aArguments.erase( aArguments.begin() );

	if (mCommands.find( Command  ) == mCommands.end())
		return Command + " : Command not found";

	return mCommands[Command]( aArguments, this );
}
//--------------------------------------------------------------------------------------------
string CSimulator::LoadConfigurationFile( string aFileName  )
{
	cout <<  "-I- Loading configuration from '"<<  aFileName << "'\n";

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

		if (TokenType == "Scene.Object")
		{
			
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
	Gpu.Gt.Statistics = &Statistics;
	Gpu.Gt.Scene->OCtree.Statistics = &Statistics;
	cout << LoadConfigurationFile( aFileName ) << "\n";
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
		throw string("Could not open file '" + aFileName + "' for writting\n");
	
	for (int j = 0; j < Scene.ResolutionHeight; j++)
	{
		
		for (int i = 0; i < Scene.ResolutionWidth; i++)
		{
			CRay Ray =  Gpu.Rgu.Execute( i,j );
			TMortonCode  GtResut = Gpu.Gt.Execute( Ray );

			if (GtResut == NULL_MORTON_IDX)
			{
				Statistics.Stat["TotalRayNotIntersectingVoxel"] += 1;
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
