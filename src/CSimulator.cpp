#include "../include/CSimulator.h"
#include "../include/Common.h"

#include <iostream>
#include <fstream>
#include <bitset>
#include <algorithm>




//--------------------------------------------------------------------------------------------
string CallBack_Exit(vector<string> aArg, CSimulator * aSim )
{
	exit(0);
}
//--------------------------------------------------------------------------------------------
string CallBack_Voxelize(vector<string> aArg, CSimulator * aSim )
{
	if (aArg.size() != 0)
		return "invalid number of arguments";

	aSim->Scene.OCtree.Populate( aSim->Scene.Geometry );
	aSim->mParameter["voxels-created"] = 1;

	aSim->Gpu.Memory.Initialize( 
		aSim->Scene.OCtree.mParameter["depth"], 
		aSim->Gpu.Memory.mParameter["cache-lines-per-way"] );

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

	if (aSim->mParameter["voxels-created"] == 0)
		return "Vozelization not yet performed. Please run the 'voxelize'command first";

	

	map<string,E_RENDER_TYPE> strRenderType;
	strRenderType["hw"] = RENDER_HW;
	strRenderType["sw"] = RENDER_SW_PURE;

	if (strRenderType.find( aArg[1] ) == strRenderType.end())
		return "Invalid render type";

	aSim->RenderFrame( aArg[0], strRenderType[aArg[1] ] );
	#ifndef _WIN32
		if (aSim->Gpu.mParameter["core-count"])
			system(string("display " + aArg[0] + " &").c_str());
	#endif
	return "\nRender output written into " +  aArg[0] ;

}
 
map<string,bool> ExperimentsRan;		//Time is precious, use this list to make sure no experiment ran twice


//--------------------------------------------------------------------------------------------
string FormatFactorString( string aFactor )
{
	int pos = 0;
	string ExpFormatedString = aFactor;
	while ( (pos = ExpFormatedString.find(".")) != string::npos )
		ExpFormatedString.replace(pos,1,"_");

	while ( (pos = ExpFormatedString.find("-")) != string::npos )
		ExpFormatedString.replace(pos,1,"_");

	return ExpFormatedString;				
}
//--------------------------------------------------------------------------------------------
void RunExperiment(vector<CStatisticFactor> & aFactors, bitset<32> aBitset, 
	int aMaxLevels, CSimulator * aSim, ofstream & anova, ofstream & plot)
{
	static int ExperimentNumber = 0;
	string strExperimentsRan;
	for (int l = 0; l < aMaxLevels; l++)
	{
		int Index = 0;
		for (int r = 0; r < aMaxLevels; r++)
		{
			Index = 0;
			strExperimentsRan = "";
			for (auto  F = aFactors.begin(); F != aFactors.end(); F++)
			{
					
					if (aBitset[Index] == false)	//This is one of the factor that is kept 'constant'for this run
					{
					//	cout  << F->mName << " = "  << F->mLevel[l] << " , ";
						aSim->SetParameter(F->mName,F->mLevel[l] );
						ostringstream oss;
						oss <<  F->mLevel[l] <<  " , ";
						strExperimentsRan += oss.str();

					} else {	//This is the factor that we 'evaluate'. This is, while keeping all other factors constant, run a sim using each level from this factor
				
					//	cout  << F->mName << "* = "  << F->mLevel[r] << ", ";
						aSim->SetParameter(F->mName,F->mLevel[r] );
						ostringstream oss;
						oss <<  F->mLevel[r] <<  " , ";
						strExperimentsRan += oss.str();
					}

					//Make sure to re-initilize the memory
					aSim->Gpu.Memory.Initialize( 
							aSim->Scene.OCtree.mParameter["depth"], 
							aSim->Gpu.Memory.mParameter["cache-lines-per-way"] );


					Index++;
			}
				
				if (ExperimentsRan.find( strExperimentsRan ) == ExperimentsRan.end())
				{
					cout << strExperimentsRan << "\n";
					string ImageFileName = std::to_string(ExperimentNumber++) + ".ppm";
					aSim->RenderFrame(  ImageFileName, RENDER_HW );

					#ifndef _WIN32
						if (aSim->mParameter["rfe-display-images"]  == 1 && aSim->Gpu.mParameter["core-count"] == 1)
							system(string("display " + ImageFileName + " &").c_str());
					#endif

					ExperimentsRan[ strExperimentsRan ] = true;
		
//TODO: What is happening with multiple cores???
					float CacheSize = ((float)aSim->Gpu.Memory.GetCacheSizeBytes()/1000000.0f);


					anova << aSim->Scene.mParameter["resolution-width"] << "x" << aSim->Scene.mParameter["resolution-height"] << " ,  ";
					anova << aSim->Scene.OCtree.mParameter["depth"] << " , " << CacheSize << " , " << strExperimentsRan ;
					anova << ((float)aSim->Statistics.Stat["mem.cache.l1.hit_count"]/(float)aSim->Statistics.Stat["mem.total_reads"]) << "  ,  ";
					anova << ((float)aSim->Statistics.Stat["mem.cache.l1.miss_count"]/aSim->Statistics.Stat["mem.total_reads"]) << "  ,  ";
					anova << aSim->Statistics.Stat["mem.replace_cache_entry"] << " , ";
					anova << aSim->Statistics.Stat["mem.external.read_access_count"] << "\n";

//This is for our nice to see plot

				
					

				}
				
		}
			
	}
}
//--------------------------------------------------------------------------------------------
string CallBack_RunFactorialExperiment(vector<string> aArg, CSimulator * aSim )
{
	
	vector<CStatisticFactor> Factors;
	
	
	//Factors.push_back( CStatisticFactor("gpu.core-count","1 8 16 32"));
	Factors.push_back( CStatisticFactor("gpu.grid-partition-size", "2 10 20 40 ")); //1% 5% 10% 100%
	//Factors.push_back( CStatisticFactor("gpu.memory.cache-enabled", "1 0 0 0 "));
	Factors.push_back( CStatisticFactor("gpu.memory.cache-lines-per-way", "64 512 4096 32768" ));

	//Run the factorial experiment
	ofstream anova("anova.csv");
	ofstream plot("plot.csv");


	if (!anova.good())
		throw string("Could not open file");
	//Do the voxelization first because it takes a lot of time
	std::pair<int,int> Resolutions[] = 
	{ 
		
		std::make_pair<int,int>(640,480),
		std::make_pair<int,int>(800,600),
	//	std::make_pair<int,int>(1024,768),
		std::make_pair<int,int>(1280,720),		//HD (720p)
		std::make_pair<int,int>(1920,1080),		//Full HD (1080p)
		std::make_pair<int,int>(3200,1800),		//UHD (4k)


	};
	int VoxelLevel[] = {5,6,7,8};

	anova << "# Loaded model: " << aSim->mModelName << "\n";
	anova << "resolution,  octree_depth , total_cache_size_mb , ";

	for (int h = 0; h < Factors.size(); h++)
		anova << FormatFactorString( Factors[h].mName ) << ",";

	anova << " cache_l1_hit_rate , cache_l1_miss_rate , cache_replace_rate, external_mem_read_count\n";
	

	for (int res = 0; res < sizeof(Resolutions)/sizeof(std::pair<int,int>); res++)
	{
		aSim->SetParameter("scene.resolution-width", Resolutions[res].first );
		aSim->SetParameter("scene.resolution-height", Resolutions[res].second );

		for (int v= 0; v < (sizeof(VoxelLevel)/sizeof(int)); v++ )
		{
			ExperimentsRan.clear();

			aSim->SetParameter("scene.octree.depth", VoxelLevel[v] );
			
			CallBack_Voxelize(vector<string>(),aSim);

			for (int e = 1; e < (2 << Factors.size())-1; e++)
			{
				bitset<32> Bitset(e);
				cout << Bitset.to_string() << "\n";
				RunExperiment( Factors,Bitset,4,aSim,anova,plot);
			}
		}
	}
	anova.close();
	plot.close();

	return "Factorial experiment complete\n";
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
		ostringstream oss;
		oss  
		<< aSim->Statistics.Print() << "\n"
		<< " **** \n"
		<< "Total cache hit rate " << (100*((float)aSim->Statistics.Stat["mem.cache.l1.hit_count"]/(float)aSim->Statistics.Stat["mem.total_reads"])) << "%\n"
		<< "Total cache size " << ((float)aSim->Gpu.Memory.GetCacheSizeBytes()/1000000.0f) << " MB\n"
		<< aSim->Gpu.Memory.PrintCacheHitRates() << "\n";

		return oss.str();
	} else {
		return "Invalid type '" + Type + "'\n";
	}

	return oss.str();
}
//--------------------------------------------------------------------------------------------
string CallBack_PrintObject(vector<string> aArg, CSimulator * aSim )
{
		if (aArg.size() != 1)
		return "invalid number of arguments";

		string Object = aArg[0];
		if (Object == "simulator")
			return aSim->PrintObjects( );
		else if (aSim->mObjects.find(Object) == aSim->mObjects.end())
			return "invalid object " +  Object;

		return aSim->mObjects[ Object ]->PrintObjects( );
}
//--------------------------------------------------------------------------------------------
string CallBack_Set(vector<string> aArg, CSimulator * aSim )
{
try
{
	if (aArg.size() != 2)
		return "invalid number of arguments";


	string Object = aArg[0];
	string Parameter = aArg[0];
	int Value = StringToInt( aArg[1] );

	int pos = 0;
	if ((pos = Object.find(".")) == string::npos)
		return "Incorrect syntax";

	Object.erase(pos,string::npos);
	Parameter.erase(0,pos+1);
	
	if (Object == "simulator")
	{
		aSim->SetParameter( Parameter, Value );
		return "Value was set";
	}
	else if (aSim->mObjects.find(Object) == aSim->mObjects.end())
		return "invalid object " +  Object;

	aSim->mObjects[ Object ]->SetParameter( Parameter, Value );
	return "Value was set";
}

catch (string Error)
{

	return Error + "\n";
}

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
		"run_factorial_exp                                : \n"
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
	mCommands["v"]	        =  CallBack_Voxelize;
	mCommands["render"]		=  CallBack_Render;
	mCommands["help"]		=  CallBack_Help;
	mCommands["show"]		=  CallBack_Show;
	mCommands["set"]        =  CallBack_Set;
	mCommands["print"]      =  CallBack_PrintObject;
	mCommands["run_factorial_exp"]      =  CallBack_RunFactorialExperiment;
	mCommands["rfe"]                    =  CallBack_RunFactorialExperiment;

	mObjects["gpu"]   = (CGenericObject*)&Gpu;
	mObjects["scene"] = (CGenericObject*)&Scene;
	

	
	mParameter["voxels-created"] = 0;
	mParameter["rfe-display-images"] = 0;
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
			mModelName = Tokens[1];
			Scene.LoadObject( Tokens[1]  );
			continue;
		}

		if (TokenType == "Scene.OctreeDepth")
		{
			Scene.OCtree.SetParameter( "depth",StringToInt( Tokens[1] ));
			continue;
		}

		
		if (TokenType == "Scene.Width" )
		{
			
			Scene.SetParameter("resolution-width",StringToInt( Tokens[1] ));
			continue;
		}

		if (TokenType == "Scene.Height" )
		{
			Scene.SetParameter("resolution-height",StringToInt( Tokens[1] ));
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
	mParameter["voxels-created"] = 0;

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
void CSimulator::Initialize(  )
{
	Gpu.Initialialize( &Statistics, &Scene );

;
}
//--------------------------------------------------------------------------------------------
void CSimulator::OpenRenderFile( ofstream & ofs )
{

	ofs << "P3\n";
	ofs << Scene.mParameter["resolution-width"] << " " << Scene.mParameter["resolution-height"] << "\n";
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
	int ResolutionWidth  = Scene.mParameter["resolution-width"];
	int ResolutionHeight = Scene.mParameter["resolution-height"];

	for (int j = 0; j < ResolutionHeight; j++)
	{
		
		for (int i = 0; i < ResolutionWidth; i++)
		{
			
			CRay Ray =  Gpu.Rgu.Execute( i,j );
			TMortonCode  GtResut = Gpu.Gt[0].Execute( Ray );

			if (GtResut == NULL_MORTON_IDX)
			{
				Statistics.Stat["gpu.total_ray_not_intersecting_any_voxel"] += 1;
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
			Gpu.Memory.ClearCaches();
			Gpu.Execute(  ofs );
		
	
}
//--------------------------------------------------------------------------------------------
void CSimulator::RenderFrame( string aFileName, E_RENDER_TYPE aRenderType )
{
	
	ofstream ofs(aFileName.c_str());
	if (!ofs.good())
		throw string("Could not open file '" + aFileName + "' for writting\n");

	OpenRenderFile(ofs );

	Statistics.Clear();

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
