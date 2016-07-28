#ifndef LOAN_SERVER_H
#define LOAN_SERVER_H 

#include <string>
#include "net.h"
#include "util.h"
#include "utilmoneystr.h"

#ifndef WIN32
#include <arpa/inet.h>
#endif

#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
using namespace std;
bool FXReceiveData(CNode* pfrom, std::string strCommand, CDataStream& vRecv);
void registernewaddress(string address, string btctx, string tx);
void addnewloanrequest(string address, CAmount amount, double premium, int expiry, int period, string message, string tx);
void addnewloan(string address, string reqaddress, string reqtx, CAmount amount, string requestid, string message);
void defaultreport(string address, string reqaddress, string reqtx,string loantx, CAmount amount,string requestid, string tx);
void submitnewvote(string address, string topicstarter, string topic, string option1, string option2, string description, string tx);
void vote(string address, string topic, int option, string tx);

unsigned short GetP2PServicesPort();

class CLoanServer
{
public:

   void buildtrusttable(); 

};

#endif
