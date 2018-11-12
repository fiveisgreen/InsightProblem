#ifndef MAP_UTILITIES_CPP
#define MAP_UTILITIES_CPP

#include <map>
#include <string>

using namespace std;

bool is_in_map(const string& key_string, map<string,int>& Map) 
{  	//This returns true iff key_string is found as a key of Map. 
	for (map<string, int>::iterator x = Map.begin(); x != Map.end(); ++x)  //g++ errors on use of auto :-/
		if (key_string.compare(x->first) == 0) 
			return true;
        return false;
}

bool is_in_map(const int& key_int, map<int,string>& Map) 
{  	//This returns true iff the key_int is found as a key of Map.
	for (map<int, string>::iterator x = Map.begin(); x != Map.end(); ++x)
		if(key_int == x->first) 
			return true;
        return false;
}

int count_entries(map<string,int>& Map) 
{  	//This sums all the ints in the second field of Map. This is used in States.cpp.
        int total = 0;
	for (map<string, int>::iterator x = Map.begin(); x != Map.end(); ++x)  
		total += x->second;
        return total;
}

#endif //MAP_UTILITIES_CPP

