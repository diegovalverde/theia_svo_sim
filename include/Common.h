#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
#include <string>
#include <sstream>
#include <iostream>
using namespace std;
#define PI 3.14159265359


#define LOG(aMsg) cout << __FUNCTION__ << " : " << aMsg << "\n";

//-----------------------------------------------------------------------
inline void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
{
    
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
      string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
    
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }
}
//-----------------------------------------------------------------------

inline int StringToInt(string aString)
{
	int Integer = 0;
	istringstream ss( aString );
	ss >> Integer;
	return Integer;
}
//-----------------------------------------------------------------------

inline double DegToRadians( double aDec )
{
	return (aDec*PI/180);
}
//-----------------------------------------------------------------------
inline vector<double> StringVecToIntVec( vector<string> S, int Begin = 1 )
{
	vector<double> R;
	
	for (size_t i = Begin; i < S.size(); i++)
	{
		istringstream iss( S[i] );
		double N;
		iss >> N;
		R.push_back( N );
	}
return R;		

}
//-----------------------------------------------------------------------
#endif