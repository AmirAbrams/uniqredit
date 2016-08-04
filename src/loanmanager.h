#ifndef LOAN_MANAGER_H
#define LOAN_MANAGER_H 

#include "amount.h"
#include "util.h"
#include "utilmoneystr.h"
#include "bloom.h"
#include "compat.h"
#include "hash.h"
#include "limitedmap.h"
#include "netbase.h"
#include "protocol.h"
#include "random.h"
#include "streams.h"
#include "sync.h"
#include "uint256.h"
#include "utilstrencodings.h"
#include "primitives/transaction.h"
#include "pubkey.h"

#include <sqlite3.h>
#include <iostream>
#include <string>


#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
using namespace std;
class CAddrMan;

class CLoanManager
{
public:
	string senddata(string data);
};

bool GetBindHash(uint160& hash, CTransaction const& tx, bool senderbind = false);

std::map<CAddress, uint64_t> ListAdvertisedBalances();
std::vector<unsigned char> CreateAddressIdentification(
    CNetAddr const& tor_address_parsed,
    boost::uint64_t const& nonce
);

void PushOffChain(CNetAddr const& parsed,std::string const& name,CTransaction const& tx);

void InitializeSenderBind(std::vector<unsigned char> const& key, uint64_t &sender_address_bind_nonce,
    CNetAddr const& local, CNetAddr const& sufficient, uint64_t const& nAmount);

void InitializeDelegateBind(std::vector<unsigned char> const& key,uint64_t const& nonce,
    CNetAddr const& local,CNetAddr const& sufficient,uint64_t const& nAmount);

std::string CreateTransferEscrow (std::string const destination_address,uint256 const sender_confirmtx_hash,
	std::string const sender_tor_address, boost::uint64_t const sender_address_bind_nonce,
    const boost::uint64_t transfer_nonce, const std::vector<unsigned char> transfer_tx_hash,int depth);

std::string SendRetrieveTx(CTransaction tx, int depth);

std::string CreateTransferExpiry(std::string const destination_address,uint256 const bind_tx,int depth);

#endif
