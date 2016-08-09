#ifndef LOAN_MANAGER_H
#define LOAN_MANAGER_H 

#include <sqlite3.h>

#include <string>
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
class CLoanManager
{
public:
	std::string senddata(std::string data);
};

#endif
