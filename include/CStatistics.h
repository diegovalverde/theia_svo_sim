#ifndef STATISTICS_INCLUDED_H
#define STATISTICS_INCLUDED_H
#include <string>

using namespace std;

//------------------------------------------------------------------
class CStatistics
{
public:
	CStatistics()
	{
		TotalTriangleListCount = 0;		
		AverageTriangleListSize = 0;	
		AverageOCtantsVisited = 0;
		TotalRayCount = 0;
		TotalRayNotIntersectingVoxel = 0;
	    MaxOCtantVisited = 0;
	    MinOCtantVisited = 0;
	    VisitedOctansLessThan40 = 0;
	    VisitedOctansBetween40And80 = 0;
	    VisitedOctansBetween80And150 = 0;
	    VisitedOctansBetween150And200 = 0;
	    VisitedOctansMoreThan200 = 0;
		BiggestTriangleListSize = 0;
		SmallestTriangleListSize = 0;
		GeometryAccessCount = 0;
	}

	~CStatistics() {};

public:
	string Print( void );
	void   Clear( void );

public:
	unsigned long	TotalTriangleListCount;		//How many triangle lists
	unsigned long	AverageTriangleListSize;	//How big (average triangle lists sizes are)
	unsigned long	AverageOCtantsVisited;
	unsigned long   TotalRayCount;
	unsigned long   TotalRayNotIntersectingVoxel;
	unsigned int    MaxOCtantVisited;
	unsigned int    MinOCtantVisited;
	unsigned int    VisitedOctansLessThan40;
	unsigned int    VisitedOctansBetween40And80;
	unsigned int    VisitedOctansBetween80And150;
	unsigned int    VisitedOctansBetween150And200;
	unsigned int    VisitedOctansMoreThan200;
	unsigned int	BiggestTriangleListSize;
	unsigned int	SmallestTriangleListSize;
	unsigned long   GeometryAccessCount;
} ;
//------------------------------------------------------------------
#endif


//---------------------------------------------------------------------------------------------------------------------
