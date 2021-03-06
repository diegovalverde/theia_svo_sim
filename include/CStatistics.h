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
		

		//Stat["gpu.average_octants_visited_per_ray"]       = 0;
		Stat["gpu.total_ray_not_intersecting_any_voxel"] = 0;
		Stat["gpu.rpu.ray_count"]                        = 0;
		Stat["mem.external.read_access_count"]           = 0;
		Stat["mem.cache.l1.hit_count"]                   = 0;
		Stat["mem.cache.l1.miss_count"]                  = 0;
		Stat["mem.total_reads"]                          = 0;
		Stat["mem.replace_cache_entry"]                  = 0;
		Stat["mem.external.used_rows"]					 = 0;
		
	}

	~CStatistics() {};

public:
	string Print( void )
	{
		ostringstream oss;
		for (auto I = Stat.begin(); I != Stat.end(); I++)
		{
			char Buffer[1024];
			sprintf_s(Buffer,"%-50s : %llu\n",I->first.c_str(), I->second);
			oss << Buffer;
		}

		return oss.str();
	}
	void   Clear( void )
	{
	//	cout << "-I- Clearing all statistics\n";
		for (auto I = Stat.begin(); I != Stat.end(); I++)
		{
			I->second = 0;
		//	cout << I->first << " = " << I->second << "\n";
			
		}
		
	}

public:
	map<string, unsigned long long> Stat;
	
} ;
//------------------------------------------------------------------

	class CStatisticFactor
	{
	public:
		CStatisticFactor( )  {}
		CStatisticFactor(string aName, string Levels )  
		{
			mName = aName;
			stringstream oss;
			int Value;
			oss << Levels;
			while (oss >> Value)
				mLevel.push_back( Value );
			
		}


		string      mName;
		vector<int> mLevel;
	};
//------------------------------------------------------------------
#endif


//---------------------------------------------------------------------------------------------------------------------
