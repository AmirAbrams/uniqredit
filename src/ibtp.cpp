// Copyright (c) 2014 The Sapience AIFX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "ibtp.h"
#include "main.h"
#include "util.h"
#include "chainparams.h"
#include <map>

using namespace std;

void CIbtp::LoadMsgStart()
{
    vChains.push_back(SChain("UniQredit P2P Platform", "UNIQ", 0xb9, 0xad, 0xbc, 0x17));
    vChains.push_back(SChain("Random Coin Testnet", "TEST", 0x07, 0xa1, 0x05, 0x0b));
}

bool CIbtp::IsIbtpChain(const unsigned char msgStart[], std::string& chainName)
{
    bool bFound = false;
	const CChainParams& chainparams = Params();
    BOOST_FOREACH(SChain p, vChains)
    {
        unsigned char pchMsg[4] = { p.pchMessageOne, p.pchMessageTwo, p.pchMessageThree, p.pchMessageFour };
        if(memcmp(msgStart, chainparams.MessageStart(), MESSAGE_START_SIZE) != 0)
        {
            if(memcmp(msgStart, pchMsg, sizeof(pchMsg)) == 0)
            {
                bFound = true;
                chainName = p.sChainName;
                printf("Found IBTP chain: %s\n", p.sChainName.c_str());
            }
        }
    }
    return bFound;
}
