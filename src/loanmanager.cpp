// Copyright (c) 2014-2016 Minato Mathers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "loanmanager.h"
#include "util.h"
#include "trust.h"

#include <fstream>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <sstream>
#include <algorithm>
#include <exception>
#include <set>
#include <stdint.h>
#include <utility>
#include <vector>

#define PORT 2020
#define DEST_IP "192.52.166.220"

#ifdef WIN32
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#endif

/** Loan manager */

std::string CLoanManager::senddata(std::string data)
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        LogPrintf("ERROR opening socket");
    server = gethostbyname(DEST_IP);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        LogPrintf("ERROR connecting\n");

    bzero(buffer,256);

    std::memcpy(buffer, data.c_str(), data.size() + 1);

    n = write(sockfd,buffer,strlen(buffer));

    if (n < 0)
         LogPrintf("ERROR writing to socket\n");

    bzero(buffer,256);

    n = read(sockfd,buffer,255);

    if (n < 0)
         LogPrintf("ERROR reading from socket\n");

    close(sockfd);

    std::string result(buffer);

	return result;

}
