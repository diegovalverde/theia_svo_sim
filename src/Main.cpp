#include "../include/CSimulator.h"
#include "../include/Common.h"
#include <sstream>
#include <algorithm>
#include <iterator>
#ifndef _WIN32
 #include <readline/readline.h>
 #include <readline/history.h>
#endif

CSimulator Simulator;

int main(int argc, char * argv[])
{
	try
	{
		
		//See if the W or the H are specified in the command line
		int Width = 0,Heigth = 0;
		bool ResolutionSpecified = false;
		Simulator.ExperimentParameters.Depth = 5;

		for (int i = 1; i < argc; i++)
		{

			string Arg = argv[i];
			if (Arg == "-H")
				Heigth = StringToInt( argv[++i]);
			
			if (Arg == "-W")
				Width = StringToInt( argv[++i]);

			if (Arg == "-D")
				Simulator.ExperimentParameters.Depth = StringToInt( argv[++i]);

		}

		if (Width || Heigth)
		{
			std::pair<int,int> Res;
			Res.first = Width;
			Res.second = Heigth;
			Simulator.ExperimentParameters.Resolutions.push_back( Res);
		}
		else
		{
			Simulator.ExperimentParameters.Resolutions.push_back( std::make_pair<int,int>(640,480)  );
			Simulator.ExperimentParameters.Resolutions.push_back( std::make_pair<int,int>(800,600)  );
			Simulator.ExperimentParameters.Resolutions.push_back( std::make_pair<int,int>(1280,720) );		//HD (720p)
			Simulator.ExperimentParameters.Resolutions.push_back( std::make_pair<int,int>(1920,1080) );		//Full HD (1080p)
			Simulator.ExperimentParameters.Resolutions.push_back( std::make_pair<int,int>(3200,1800) );		//UHD (4k)
		}





#ifdef _WIN32
	//Shorcut for VS210 path
	Simulator.Initialize("teapod_svo__depth_4.config");
#else
	Simulator.Initialize();	
#endif	
	
	//Simulator.Initialize("suzane_avo__depth_2.config");
	cout << "Type 'help' to see the available commands\n";
	
	while (1)
	{
		string Prompt = "<simulator>::";
		string Line;

#ifndef _WIN32

      
        char * cLine = readline(Prompt.c_str());
        if (*cLine)
                add_history(cLine);
        Line = string(cLine);
#else
		cout << Prompt;
		std::getline (std::cin,Line);
#endif
		stringstream ss(Line);
		std::istream_iterator<string> begin(ss);
		std::istream_iterator<string> end;
		vector<string> Tokens(begin, end);

		cout << Simulator.ExecuteCommand(Tokens) << "\n\n";
	}

	
	return 0;
	}
	catch (string aMessage)
	{
		cout << "-E- " << aMessage << "\n";
	}
}