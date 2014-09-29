#include "../include/CSimulator.h"
#include "../include/Common.h"

#include <iostream>
#include <fstream>
#include <bitset>




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
	if (aArg.size() != 2)
		return "invalid number of arguments";

	if (aSim->mVoxelizationDone == false)
		return "Vozelization not yet performed. Please run the 'voxelize'command first";

	E_RENDER_TYPE RenderType;

	map<string,E_RENDER_TYPE> strRenderType;
	strRenderType["hw"] = RENDER_HW;
	strRenderType["sw"] = RENDER_SW_PURE;

	if (strRenderType.find( aArg[1] ) == strRenderType.end())
		return "Invalid render type";

	aSim->RenderFrame( aArg[0], strRenderType[aArg[1] ] );
	#ifndef _WIN32
		system(string("display " + aArg[0] + " &").c_str());
	#endif
	return "\nRender output written into " +  aArg[0] ;

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
	} 
	else if (Object == "cache")
	{
		if (Attribute == "enable")
		{
			if (aArg[2] == "true")
				aSim->Gpu.Memory.mBypassCache = false;
			else if (aArg[2] == "false")
				aSim->Gpu.Memory.mBypassCache = true;
			else
				return "This attribute only accepts true or false";
		}
	}
	else
		return "Invalid type '" + Object + "'\n";

	return "Value was set";
}
//--------------------------------------------------------------------------------------------
string CallBack_Voxelize(vector<string> aArg, CSimulator * aSim )
{
	if (aArg.size() != 0)
		return "invalid number of arguments";

	aSim->Scene.OCtree.Populate( aSim->Scene.Geometry );
	aSim->mVoxelizationDone = true;

	aSim->Gpu.Memory.Initialize( aSim->Scene.OCtree.mMaxDepth, 64 );

	//Ok, now initialize the GPU memory
	cout << "Writting back GPU memory hierarchy\n";

	for (auto O = aSim->Scene.OCtree.Octant.begin(); O != aSim->Scene.OCtree.Octant.end(); O++)
	{
		TMortonCode ParentMortonCode = O->first;
		int WriteAddress = ParentMortonCode;	//Write address is Morton code belonging to parent
		bitset<16> WriteData;

		int BitPosition = 0;
		for (int i = 0; i < 8; i++)
		{
			TMortonCode ChildMortonCode = ((ParentMortonCode << 3) + i);
			if (aSim->Scene.OCtree.Octant.find( ChildMortonCode ) == aSim->Scene.OCtree.Octant.end())
			{
				BitPosition += 2;
				continue;
			}

			COctant OCtant = aSim->Scene.OCtree.GetOCtant( ChildMortonCode );
			/*
			EMPTY,		
			LEAF,
			NON_EMPTY,
			RESERVED
			*/

			if (OCtant.isEmpty)
			{
			
				WriteData[BitPosition]   = 0;
				WriteData[BitPosition+1] = 0;
				
			} 
			else if (OCtant.isLeaf)
			{
				WriteData[BitPosition]   = 1;
				WriteData[BitPosition+1] = 0;

			} else {

				WriteData[BitPosition]   = 0;
				WriteData[BitPosition+1] = 1;
			}
			BitPosition += 2;
		}
		aSim->Gpu.Memory.Write( WriteAddress, WriteData.to_ulong() ); 
		
	}
	

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
		"set <octree|cache> <attribute> <value>           : sets values\n"
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

	mVoxelizationDone = false;
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
	mVoxelizationDone = false;

	return "File Loaded";
}
//--------------------------------------------------------------------------------------------
void CSimulator::Initialize( string aFileName )
{
	Gpu.Initialialize( &Statistics, &Scene );


	cout << LoadConfigurationFile( aFileName ) << "\n";
	Scene.Camera.Initialize();
}
//--------------------------------------------------------------------------------------------
void CSimulator::OpenRenderFile( ofstream & ofs )
{

	ofs << "P3\n";
	ofs << Scene.ResolutionWidth << " " << Scene.ResolutionHeight << "\n";
	ofs << "255\n";
	
}
//--------------------------------------------------------------------------------------------
void CSimulator::CloseRenderFile( ofstream & ofs )
{
	ofs.close();
}
//--------------------------------------------------------------------------------------------
void CSimulator::RenderSwFast( ofstream & ofs )
{
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
}
//--------------------------------------------------------------------------------------------
void CSimulator::RenderHw( ofstream & ofs )
{
	Gpu.Execute(  Scene.ResolutionWidth, Scene.ResolutionHeight, ofs );
}
//--------------------------------------------------------------------------------------------
void CSimulator::RenderFrame( string aFileName, E_RENDER_TYPE aRenderType )
{

	ofstream ofs(aFileName.c_str());
	if (!ofs.good())
		throw string("Could not open file '" + aFileName + "' for writting\n");

	OpenRenderFile(ofs );

	switch (aRenderType)
	{
	case RENDER_SW_PURE:
		RenderSwFast( ofs );
		break;
	case RENDER_HW:
		RenderHw( ofs );
		break;
	}
	

	CloseRenderFile( ofs );

}
//--------------------------------------------------------------------------------------------
