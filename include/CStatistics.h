#ifndef STATISTICS_INCLUDED_H
#define STATISTICS_INCLUDED_H
#include <string>
#include <sstream>

using namespace std;

//------------------------------------------------------------------
class CStatistics
{
public:
	CStatistics()
	{
		Stat["TotalTriangleListCount"] = 0;		
		Stat["AverageTriangleListSize"] = 0;	
		Stat["AverageOCtantsVisited"] = 0;
		Stat["TotalRayCount"] = 0;
		Stat["TotalRayNotIntersectingVoxel"] = 0;
	    Stat["MaxOCtantVisited"] = 0;
	    Stat["MinOCtantVisited"] = 0;
	    Stat["VisitedOctansLessThan40"] = 0;
	    Stat["VisitedOctansBetween40And80"] = 0;
	    Stat["VisitedOctansBetween80And150"] = 0;
	    Stat["VisitedOctansBetween150And200"] = 0;
	    Stat["VisitedOctansMoreThan200"] = 0;
		Stat["BiggestTriangleListSize"] = 0;
		Stat["SmallestTriangleListSize"] = 0;
		Stat["GeometryAccessCount"] = 0;
	}

	~CStatistics() {};

public:
	string Print( void )
	{
		ostringstream oss;
		for (auto I = Stat.begin(); I != Stat.end(); I++)
		{
			char Buffer[1024];
			sprintf(Buffer,"%-50s : %d\n",I->first.c_str(), I->second);
			oss << Buffer;
		}

		return oss.str();
	}
	void   Clear( void );

public:
	map<string, unsigned long long> Stat;
	
} ;
//------------------------------------------------------------------
#endif


//---------------------------------------------------------------------------------------------------------------------
