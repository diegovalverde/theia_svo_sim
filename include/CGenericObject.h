#ifndef CGENERIC_OBJECT_H_DEFINED
#define CGENERIC_OBJECT_H_DEFINED

#include <string>
#include <sstream>
#include <map>
using namespace std;

class CGenericObject
{

public:
	void SetParameter(string aName, int aValue)
	{
		int pos = 0;
		if ((pos = aName.find(".")) != string::npos)
		{
			string ChildName = aName;
			ChildName.erase(pos,string::npos);
			aName.erase(0,pos+1);

			if (mObjects.find(ChildName) == mObjects.end())
				throw string("Invalid child object '"+ aName + "'");
			
			mObjects[ChildName]->SetParameter( aName, aValue );
			return;
			
		}

		if (mParameter.find(aName) == mParameter.end())
			throw string("Invalid paramter '"+ aName + "'" );

		mParameter[ aName ] = aValue;
			
	}


	string PrintObjects()
	{
		ostringstream oss;
		for (auto I = mParameter.begin(); I != mParameter.end(); I++)
		{
			char Buffer[256];
			sprintf_s(Buffer,"%-30s : %d\n",  I->first.c_str(), I->second);
 			oss  << Buffer;
		}

		for (auto I = mObjects.begin(); I != mObjects.end(); I++)
		{
			oss << "=== " << I->first << "\n";
			oss << I->second->PrintObjects();
			
		}

		return oss.str();	

	}

	

	map<string,int> mParameter;

	map<string,CGenericObject * > mObjects;
};
#endif
