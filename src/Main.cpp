#include "../include/CSimulator.h"
#include <sstream>
#include <iterator>
#ifndef _WIN32
 #include <readline/readline.h>
 #include <readline/history.h>
#endif

CSimulator Simulator;

int main(void)
{
	try
	{
		

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