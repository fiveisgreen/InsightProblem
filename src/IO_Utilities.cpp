#ifndef IO_UTILITIES_CPP
#define IO_UTILITIES_CPP

#include <sys/stat.h>  
#include <string>

bool file_Exists(std::string filename)
{   //returns true if a file exists
    //if file doesn't exist, runs in 0.72 microseconds
    //if file exists and is 500MB, runs in 0.93 microseconds
    struct stat buf;
    return stat(filename.c_str(), &buf) != -1;
}

#endif //IO_UTILITIES_CPP
