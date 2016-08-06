#ifndef TRUST_ENGINE_H
#define TRUST_ENGINE_H 

#include <stdio.h>
#include <algorithm>
#include <exception>
#include <map>
#include <set>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread.hpp>


extern int callback(void *NotUsed, int argc, char **argv, char **azColName);
extern std::map<std::string,int64_t> getbalances();

class TrustEngine
{
public:
    
	void createdb();
	
};

#endif
