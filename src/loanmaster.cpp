// Copyright (c) 2014-2016 Minato Mathers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "loanmaster.h"
#include "util.h"
#include "addrman.h"
#include "base58.h"
#include "trust.h"
#include "main.h"
#include "utilmoneystr.h"
#include "primitives/transaction.h"
#include "primitives/block.h"
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 2015
#define BACKLOG 20

#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread.hpp>

#include <sqlite3.h>

bool FXReceiveData(CNode* pfrom, std::string strCommand, CDataStream& vRecv){


	return true;
}

void CLoanServer::buildtrusttable()
{
        sqlite3 *rawdb;
        sqlite3_stmt *stmt, *statement;
        char *zErrMsg = 0;
        int rc;
        const char *sql ="select * from RAWDATA";

    rc = sqlite3_open((GetDataDir() / "ratings/rawdata.db").string().c_str(), &rawdb);
    if( rc ){
                if (fDebug) LogPrintf("CLoanServer :: Can't open database: %s\n", sqlite3_errmsg(rawdb));
                }else{
                if (fDebug) LogPrintf("CLoanServer:: Opened database successfully\n");
    }

    if ( sqlite3_prepare(rawdb, sql, -1, &stmt, 0 ) == SQLITE_OK )
    {
        int ctotal = sqlite3_column_count(stmt);
        int res = 0;

        while ( 1 ){
            res = sqlite3_step(stmt);

            if ( res == SQLITE_ROW ){
                for ( int i = 0; i < ctotal; i++ ){
					string address = (const char*)(sqlite3_column_text(stmt, 0));
					int64_t balance = (sqlite3_column_int(stmt, 1))/COIN;
					int64_t firstuse = sqlite3_column_int(stmt, 2);
					int64_t incomingtx = sqlite3_column_int(stmt, 3);
					int64_t outgoingtx = sqlite3_column_int(stmt, 4);
					int64_t totalinputs = sqlite3_column_int(stmt, 5);
					int64_t totaloutputs = sqlite3_column_int(stmt, 6);

					int64_t totaltx=  incomingtx+outgoingtx;
					double globallife = (GetTime()- 1418504572)/24*3600;
					double lifetime = (GetTime() - firstuse)/24*3600;
					int64_t totalnettx = chainActive.Tip()->nChainTx;
					double txfreq = totaltx/lifetime;
					double nettxfreq = totalnettx / globallife;
					double spendfactor = balance/totaloutputs;
					double savefactor;

					if (totalinputs !=0)
							savefactor =balance/totalinputs;
					else
							savefactor = 0;

					double nettxpart = totaltx/ totalnettx;
					double aveinput = totalinputs/ incomingtx;
					double aveoutput = totaloutputs / outgoingtx;
					double avedailyincome = totalinputs / lifetime;
					double avedailyexpenditure = totaloutputs / lifetime;
					double trust=0;
					double creditrating =0;
					int isbanned=0;
					int isregistered=0;
					int64_t limit=0;
					{
							{//lifetime
									if (lifetime > 360 )
											trust+= 20;
									else if (lifetime > 30 && lifetime < 360 )
											trust+= lifetime*0.055;
									else
											trust+= 0;
							}

							{//total transactions
									if (totaltx > 10000){
											trust+= 10;
							}
									else if (totaltx>0 && totaltx< 10000){
											trust+= totaltx*0.001;
							}
									else
											trust+= 0;
							}

							{//balance
									if(balance > 1000000){
											trust+= 25;
									}
									else if(balance > 0 && balance <= 1000000){
											trust+= balance/50000;
									}
									else
											trust+= 0;
							}

							{// transaction frequency
									if (txfreq > 5)
											trust+=15;
									else if (txfreq> 0.01 && txfreq< 5)
											trust+= txfreq *3;
									else
											trust+= 0;
							}

							{//save factor
									if (savefactor > 0.1)
											trust+=20;
									else if (savefactor> 0.001 && savefactor< 0.1)
											trust+= savefactor *200;
									else
											trust+= 0;
							}

							{// average daily income
									if (avedailyincome > 100)
											trust+=20;
									else if (avedailyincome > 1 && avedailyincome< 100)
											trust+= avedailyincome/5;
									else
											trust+= 0;
							}

							{// miner participation
									int count = 0;
									char* minerquery = sqlite3_mprintf("select count(*) from BLOCKS where MINER = '%q'",address.c_str());
									rc = sqlite3_exec(rawdb, minerquery, callback, &count, &zErrMsg);
									double points = count/chainActive.Tip()->nHeight;

									if (points > 0.01 )
											trust+= 20;
									else if (points > 0.0001 && points < 0.01)
											trust+= points*190;
									else
											trust+= 0;
							}

							/*{// ban status
									std::map<std::string, int>::iterator banit;
									std::map<std::string, int> banned = banlist();

									banit = banned.find(address);
									if (banit != banned.end()){
											if ((banit->second == 1))
													isbanned=1;
									}
							}

							{ // credit info
									std::map<std::string, double>::iterator creditit;
									std::map<std::string, double> credits = creditratings();

									creditit = credits.find(address);
									if (creditit != credits.end()){
											if ((creditit->second > 0))
													creditrating=creditit->second;
									}
							}

							{ // registered info
									std::map<std::string, int>::iterator verifiedit;
									std::map<std::string, int> credits = verifieddata();

									verifiedit = credits.find(address);
									if (verifiedit != credits.end()){
											if ((verifiedit->second == 1))
													isregistered=1;
									}
							}*/

							char *sql = "select * from RATINGS where ADDRESS = ?";

							rc = sqlite3_prepare(rawdb, sql, strlen(sql), &statement, 0);
							sqlite3_bind_text(statement, 1, address.data(), address.size(), 0);
							if (sqlite3_step(statement) == SQLITE_ROW){

									double oldtrust = (sqlite3_column_int(stmt, 1));
									double oldcreditrating = sqlite3_column_int(stmt, 2);
									int registered = sqlite3_column_int(stmt, 3);
									int banned = sqlite3_column_int(stmt, 4);
									int64_t oldlimit = sqlite3_column_int(stmt, 5);

									sqlite3_finalize(statement);

									if (fDebug)LogPrintf("SQlite ratings record retrieved %s, %lld, %lld, %d, %d, %d\n", address, oldtrust, oldcreditrating, registered, banned, oldlimit);

									char* updatequery = sqlite3_mprintf("update RATINGS set TRUST = %lld, TXOUTCOUNT =%lld, TOTALOUT= %lld where ADDRESS = '%q'", trust, creditrating,  isregistered, isbanned, limit,address.c_str());
									rc = sqlite3_exec(rawdb, updatequery, callback, 0, &zErrMsg);

									if (rc != SQLITE_OK){
									if (fDebug)LogPrintf("SQL update output error: %s\n", zErrMsg);
									sqlite3_free(zErrMsg);
									}
									else{
											if (fDebug)LogPrintf("update created successfully\n");
									}
							}
							else{
									char * insertquery = sqlite3_mprintf("insert into RATINGS (ADDRESS, TRUST, CREDITRATING, REGISTERED, BAN, LIMIT) values ('%q',%lld,%lld,%lld,%lld)", address.c_str(), trust, creditrating, isregistered, isbanned, limit);
									rc = sqlite3_exec(rawdb, insertquery, callback, 0, &zErrMsg);

									if (rc != SQLITE_OK){
											if (fDebug)LogPrintf("SQL insert error: %s\n", zErrMsg);
											sqlite3_free(zErrMsg);
									}
									else{
											if (fDebug)LogPrintf("insert created successfully\n");
									}
									sqlite3_finalize(statement);
							}
					}
                }
            }

            if ( res == SQLITE_DONE || res==SQLITE_ERROR){
                                if (fDebug) LogPrintf("CLoanServer::buildtrustDB %s\n", sqlite3_errmsg(rawdb));

                break;
            }
        }
        sqlite3_finalize(stmt);
    }
}

unsigned short GetP2PServicesPort()
{
    return (unsigned short)(GetArg("-serviceport",PORT));
}

void registernewaddress(string address, string btctx, string tx){

        sqlite3 *rawdb;
        sqlite3_stmt *stmt;
        char *zErrMsg = 0;
        int rc;

        sqlite3_open((GetDataDir() / "ratings/rawdata.db").string().c_str(), &rawdb);
        sqlite3_exec(rawdb, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);
        sqlite3_exec(rawdb, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);

        char *sql = "select * from CHAINIDS where CHAINID = ?";
        rc = sqlite3_prepare(rawdb, sql, strlen(sql), &stmt, 0);
        sqlite3_bind_text(stmt, 1, address.data(), address.size(), 0);
        if (sqlite3_step(stmt) == SQLITE_ROW){

                if (fDebug)LogPrintf("SQlite registry record retrieved %s\n", address);
        }
        else{
            char * insertquery = sqlite3_mprintf("insert into CHAINIDS (CHAINID, BTCHASH, TX) values ('%q','%q','%q')", address.c_str(), btctx.c_str(), tx.c_str());
            rc = sqlite3_exec(rawdb, insertquery, callback, 0, &zErrMsg);

                if (rc != SQLITE_OK){
                        if (fDebug)LogPrintf("SQL insert error: %s\n", zErrMsg);
                        sqlite3_free(zErrMsg);
                }
                else{
                        if (fDebug)LogPrintf("insert created successfully\n");
                }
        }

        sqlite3_exec(rawdb, "END TRANSACTION", NULL, NULL, &zErrMsg);
        if (sqlite3_close(rawdb) != SQLITE_OK){
                if (fDebug)LogPrintf("SQL unable to close database %s\n", sqlite3_errmsg(rawdb));
                sqlite3_free(zErrMsg);
        }
        else{
                if (fDebug)LogPrintf("database closed successfully\n");
        }
}

void addnewloanrequest(string address, CAmount amount, double premium, int expiry, int period, string message, string tx){


        sqlite3 *rawdb;
        sqlite3_stmt *stmt;
        char *zErrMsg = 0;
        int rc;

        sqlite3_open((GetDataDir() / "ratings/rawdata.db").string().c_str(), &rawdb);
        sqlite3_exec(rawdb, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);
        sqlite3_exec(rawdb, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);

        char *sql = "select * from LOANREQUESTS where CHAINID = ?";
        rc = sqlite3_prepare(rawdb, sql, strlen(sql), &stmt, 0);
        sqlite3_bind_text(stmt, 1, address.data(), address.size(), 0);
        if (sqlite3_step(stmt) == SQLITE_ROW){

                if (fDebug)LogPrintf("SQlite loan record already exists, please use a different ChainID \n");
        }
        else{
        char * insertquery = sqlite3_mprintf("insert into LOANREQUESTS (CHAINID, AMOUNT, PREMIUM, EXPIRY, PERIOD, TX) values ('%q', %lld, %f, %d, %d,'%q','%q')", address.c_str(), amount, premium, expiry, period, message.c_str(), tx.c_str());
        rc = sqlite3_exec(rawdb, insertquery, callback, 0, &zErrMsg);

                if (rc != SQLITE_OK){
                        if (fDebug)LogPrintf("SQL insert error: %s\n", zErrMsg);
                        sqlite3_free(zErrMsg);
                }
                else{
                        if (fDebug)LogPrintf("insert created successfully\n");
                }
        }

        sqlite3_exec(rawdb, "END TRANSACTION", NULL, NULL, &zErrMsg);
        if (sqlite3_close(rawdb) != SQLITE_OK){
                if (fDebug)LogPrintf("SQL unable to close database %s\n", sqlite3_errmsg(rawdb));
                sqlite3_free(zErrMsg);
        }
        else{
                if (fDebug)LogPrintf("database closed successfully\n");
        }

}

void addnewloan(string address, string reqaddress, string reqtx, CAmount amount, string requestid, string message){

        sqlite3 *rawdb;
        sqlite3_stmt *stmt;
        char *zErrMsg = 0;
        int rc;

        sqlite3_open((GetDataDir() / "ratings/rawdata.db").string().c_str(), &rawdb);
        sqlite3_exec(rawdb, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);

        sqlite3_exec(rawdb, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);

        char * insertquery = sqlite3_mprintf("insert into LOANS (CHAINID, REQCHAINID , REQTX, AMOUNT, REQID, MSG) values ('%q', '%q', '%q', %d, '%q','%q')", address.c_str(), reqaddress.c_str(), reqtx.c_str() , amount, requestid.c_str(), message.c_str());
        rc = sqlite3_exec(rawdb, insertquery, callback, 0, &zErrMsg);

        if (rc != SQLITE_OK){
                if (fDebug)LogPrintf("SQL insert error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
        }
        else{
                if (fDebug)LogPrintf("insert created successfully\n");
        }

        sqlite3_exec(rawdb, "END TRANSACTION", NULL, NULL, &zErrMsg);
        if (sqlite3_close(rawdb) != SQLITE_OK){
                if (fDebug)LogPrintf("SQL unable to close database %s\n", sqlite3_errmsg(rawdb));
                sqlite3_free(zErrMsg);
        }
        else{
                if (fDebug)LogPrintf("database closed successfully\n");
        }

}

void defaultreport(string address, string reqaddress, string reqtx, string loantx, CAmount amount, string requestid, string tx){

        sqlite3 *rawdb;
        sqlite3_stmt *stmt;
        char *zErrMsg = 0;
        int rc;

        sqlite3_open((GetDataDir() / "ratings/rawdata.db").string().c_str(), &rawdb);
        sqlite3_exec(rawdb, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);
        sqlite3_exec(rawdb, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);


}

void submitnewvote(string address, string topicstarter, string topic, string option1, string option2, string description, string tx){

        sqlite3 *rawdb;
        sqlite3_stmt *stmt;
        char *zErrMsg = 0;
        int rc;

        sqlite3_open((GetDataDir() / "ratings/rawdata.db").string().c_str(), &rawdb);
        sqlite3_exec(rawdb, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);
        sqlite3_exec(rawdb, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);



}

void vote(string address, string topic, int option, string tx){

        sqlite3 *rawdb;
        sqlite3_stmt *stmt;
        char *zErrMsg = 0;
        int rc;

        sqlite3_open((GetDataDir() / "ratings/rawdata.db").string().c_str(), &rawdb);
        sqlite3_exec(rawdb, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);
        sqlite3_exec(rawdb, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);


}

