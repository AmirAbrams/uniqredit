// Copyright (c) 2014-2016 Minato Mathers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "loanmanager.h"
#include "util.h"
#include "trust.h"
#include "main.h"
#include "net.h"
#include "addrman.h"
#include "chainparams.h"
#include "clientversion.h"
#include "consensus/consensus.h"
#include "crypto/common.h"
#include "crypto/sha256.h"
#include "hash.h"
#include "primitives/transaction.h"
#include "scheduler.h"
#include "ui_interface.h"
#include "utilstrencodings.h"
#include "arith_uint256.h"
#include "uint256.h"
#include "policy/policy.h"
#include "script/script.h"
#include "script/script_error.h"
#include "script/sign.h"
#include "script/standard.h"
#include <math.h>
#ifdef ENABLE_WALLET
#include "wallet/db.h"
#include "wallet/wallet.h"
#include "wallet/walletdb.h"
#endif

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

#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread.hpp>

#define PORT 2020
#define DEST_IP "192.52.166.220"

#ifdef WIN32
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#endif

static int const escrow_expiry = 5;

/** Loan manager */

string CLoanManager::senddata(string data)
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

    string result(buffer);

	return result;

}

bool GetBindHash(uint160& hash, CTxOut const& txout, bool senderbind) {
    CScript const payload = txout.scriptPubKey;
    opcodetype opcode;
    std::vector<unsigned char> data;
    CScript::const_iterator position = payload.begin();
    if (position >= payload.end()) {
        return false;
    }
    if (!payload.GetOp(position, opcode, data)) {
        return false;
    }
    if (0 <= opcode && opcode <= OP_PUSHDATA4) {
        return false;
    } else {
        if (OP_IF != opcode) {
            return false;
        }
    }
    if (position >= payload.end()) {
        return false;
    }
    if (!payload.GetOp(position, opcode, data)) {
        return false;
    }

    //
    if (senderbind) {
        if (0 <= opcode && opcode <= OP_PUSHDATA4) {
            return false;
        } else {
            if (OP_IF != opcode) {
                return false;
            }
        }
        if (position >= payload.end()) {
            return false;
        }
        if (!payload.GetOp(position, opcode, data)) {
            return false;
        }
    }

    if (0 <= opcode && opcode <= OP_PUSHDATA4) {
        if (data.size() < sizeof(hash)) {
            return false;
        }
        memcpy(&hash, data.data(), sizeof(hash));
        return true;
    } else {
        return false;
    }
}

bool GetBindHash(uint160& hash, CTransaction const& tx, bool senderbind) {
    for (std::vector<CTxOut>::const_iterator txout = tx.vout.begin();tx.vout.end() != txout;txout++) {
        if (GetBindHash(hash, *txout, senderbind)) {
            return true;
        }
    }
    return false;
}


std::vector<unsigned char> CreateAddressIdentification(CNetAddr const& tor_address_parsed,boost::uint64_t const& nonce) {
    std::vector<unsigned char> identification(24);

    for (int filling = 0; 16 > filling; filling++) {
        identification[filling] = tor_address_parsed.GetByte(15 - filling);
    }

    for ( int filling = 0;8 > filling; filling++ ) {
        identification[filling + 16] = 0xff & (nonce >> (filling * 8));
    }

    return identification;
}

void PushOffChain( CNetAddr const& parsed,std::string const& name, CTransaction const& tx) {
    
    CAddress destination(CService(parsed, GetListenPort()), NODE_NONE);

    CNode* connected = ConnectNode(destination, NULL, true);

    if (NULL == connected) {
        throw std::runtime_error("not connected to destination");
    }

    connected->PushMessage("pushoffchain", name, tx);
}

void  InitializeDelegateBind(std::vector<unsigned char> const& delegate_key, uint64_t const& delegate_address_bind_nonce,
    CNetAddr const& local,CNetAddr const& sender_address,uint64_t const& nAmount) {
    CPubKey recovery_key;

    do {
        CReserveKey reserve_key(pwalletMain);
        if (!reserve_key.GetReservedKey(recovery_key)) {
            throw std::runtime_error("could not find recovery address");
        }
    } while (false);

    CUniqreditAddress recovery_address;

    recovery_address.Set(recovery_key.GetID());

    CTransaction const rawTx = CreateDelegateBind(
        local,
        delegate_address_bind_nonce,
        nAmount,
        chainActive.Height() + escrow_expiry,
        recovery_address
    );
    uint160 delegate_id_hash;

    if (!GetBindHash(delegate_id_hash, rawTx)) {
        throw std::runtime_error("failure creating transaction");
    }

    pwalletMain->store_hash_delegate(
        delegate_id_hash,
        delegate_key
    );

    //store recovery address for retrieval
    uint64_t sender_address_bind_nonce;

    if(!pwalletMain->GetBoundNonce(sender_address, sender_address_bind_nonce)) {
       printf("InitializeDelegateBind() : could not find nonce for address %s \n",
              sender_address.ToStringIP().c_str());
    } else {
        pwalletMain->add_to_retrieval_string_in_nonce_map(sender_address_bind_nonce, recovery_address.ToString(), true);
        printf("InitializeDelegateBind() : wrote recovery address to retrieve string %s \n", recovery_address.ToString().c_str());
    }

    PushOffChain(sender_address, "to-sender", rawTx);
}


void InitializeSenderBind(std::vector<unsigned char> const& my_key, uint64_t& sender_address_bind_nonce, CNetAddr const& local,
    CNetAddr const& sufficient, uint64_t const& nAmount) {
    CPubKey recovery_key;

    do {
        CReserveKey reserve_key(pwalletMain);
        if (!reserve_key.GetReservedKey(recovery_key)) {
            throw std::runtime_error("could not find recovery address");
        }
    } while (false);

    CUniqreditAddress recovery_address;

    recovery_address.Set(recovery_key.GetID());

    int64_t delegateFee = pwalletMain->DelegateFee(nAmount);

    CTransaction const rawTx = CreateSenderBind(local, sender_address_bind_nonce,nAmount, delegateFee, chainActive.Height() + escrow_expiry, recovery_address);

    uint160 id_hash;

    if (!GetBindHash(id_hash, rawTx, true)) {
        throw std::runtime_error("failure creating transaction");
    }

    pwalletMain->store_hash_delegate(
        id_hash,
        my_key
    );
    pwalletMain->add_to_retrieval_string_in_nonce_map(sender_address_bind_nonce, recovery_address.ToString(), false);
    if (fDebug) printf("InitializeSenderBind() : wrote recovery address to retrieve string %s \n", recovery_address.ToString().c_str());

    try {
        PushOffChain(sufficient, "to-delegate", rawTx);
    } catch (std::exception& e) {
        PrintExceptionContinue(&e, " PushOffChain(to-delegate)");
    }

}

std::string CreateTransferEscrow (std::string const destination_address, uint256 const sender_confirmtx_hash,std::string const sender_tor_address,
    boost::uint64_t const sender_address_bind_nonce,boost::uint64_t const transfer_nonce,vector<unsigned char> const transfer_tx_hash,int depth){
    
    std::string err;
    CUniqreditAddress destination_address_parsed(destination_address);
    if (!destination_address_parsed.IsValid()) {
       err = "Invalid MIL address";
       return err;
    }
    CNetAddr tor_address_parsed;
    tor_address_parsed.SetSpecial(sender_tor_address);

    std::vector<unsigned char> identification = CreateAddressIdentification(tor_address_parsed, sender_address_bind_nonce);

    CTransaction prevTx;
    uint256 hashBlock = uint256S(itostr(0));
    if (!GetTransaction(sender_confirmtx_hash, prevTx, Params().GetConsensus(),hashBlock, true)) {
       err = "transaction unknown";
       return err;
    }
    int output_index = 0;
    CTxOut const* found = NULL;
    for (vector<CTxOut>::const_iterator checking = prevTx.vout.begin(); prevTx.vout.end() != checking; checking++, output_index++) {
        txnouttype transaction_type;
        vector<vector<unsigned char> > values;
        if (!Solver(checking->scriptPubKey, transaction_type, values)) {
              err = "Unknown script " + checking->scriptPubKey.ToString();
              return err;
        }

        if (TX_ESCROW_SENDER == transaction_type) {
            found = &(*checking);
            break;
        }
    }
    if (NULL == found) {
        err = "invalid bind transaction";
        return err;
    }

    CMutableTransaction rawTx;

    CTxOut transfer;

    transfer.scriptPubKey= GetScriptForDestination(destination_address_parsed.Get());
    transfer.nValue = found->nValue;

    rawTx.vout.push_back(transfer);

    rawTx.vin.push_back(CTxIn());

    CTxIn& input = rawTx.vin[0];

    input.prevout = COutPoint(sender_confirmtx_hash, output_index);
    input.scriptSig << transfer_tx_hash;
    input.scriptSig << transfer_nonce;
    input.scriptSig << identification;
    input.scriptSig << OP_TRUE;
    input.scriptSig << OP_TRUE;

    ScriptError serror = SCRIPT_ERR_OK;
     if (!VerifyScript(input.scriptSig, found->scriptPubKey, STANDARD_SCRIPT_VERIFY_FLAGS, MutableTransactionSignatureChecker(&rawTx, output_index), &serror)) {
        err = "verification failed";
        return err;
    }
/*
    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
    ss << rawTx;
    return HexStr(ss.begin(), ss.end());
 */
  return SendRetrieveTx(rawTx, depth);
}

std::string CreateTransferExpiry(std::string const destination_address, uint256 const bind_tx, int depth){
	
    std::string err;
    CUniqreditAddress destination_address_parsed(destination_address);
    if (!destination_address_parsed.IsValid()) {
        err = "Invalid MIL address";
        return err;
    }

    CTransaction prevTx;
    uint256 hashBlock = uint256S(itostr(0));
    if (!GetTransaction(bind_tx, prevTx, Params().GetConsensus(), hashBlock, true)) {
        err = "transaction unknown";
        return err;
    }

    CMutableTransaction rawTx;

    uint64_t value = 0;
    int output_index = 0;

    for (std::vector<CTxOut>::const_iterator checking = prevTx.vout.begin();prevTx.vout.end() != checking; checking++, output_index++) {
        txnouttype transaction_type;
        std::vector<vector<unsigned char> > values;
        if (!Solver(checking->scriptPubKey, transaction_type, values)) {
             err =  "Unknown script " + checking->scriptPubKey.ToString();
        }
        if ((TX_ESCROW == transaction_type ) || ( TX_ESCROW_FEE == transaction_type) || (TX_ESCROW_SENDER == transaction_type)) {
            value += checking->nValue;
            CTxIn claiming;
            claiming.prevout = COutPoint(bind_tx, output_index);
            int const expected = ScriptSigArgsExpected(transaction_type, values);
            for (int filling = 1; expected > filling; filling++ ) {
                claiming.scriptSig << OP_TRUE;
            }
            claiming.scriptSig << OP_FALSE;

            rawTx.vin.push_back(claiming);
        }
    }

    CTxOut transfer;

    transfer.scriptPubKey= GetScriptForDestination(destination_address_parsed.Get());
    transfer.nValue = value;

    rawTx.vout.push_back(transfer);

    return SendRetrieveTx(rawTx, depth);
}

std::string SendRetrieveTx(CTransaction tx, int depth)
{
    std::string err;
    int countdown = escrow_expiry - depth;
    if (countdown > 0) {
        std::stringstream ss;
        ss << countdown;
        err = std::string("Retrievable after " + ss.str() + " blocks");
        return err;
    }

	uint256 hashTx = tx.GetHash();
    // See if the transaction is already in a block
    // or in the memory pool:
    CTransaction existingTx;
    uint256 hashBlock = uint256S(itostr(0));
    CWalletTx mtx = CWalletTx(pwalletMain, tx);
    if (GetTransaction(hashTx, existingTx,Params().GetConsensus(), hashBlock, true))
    {
        if (hashBlock != uint256S(itostr(0))) {
             err = "transaction already in block ";
             return err;
        }

        // Not in block, but already in the memory pool; will drop
        // through to re-relay it.
    }
    else
    {
        // push to local node
        if (!mtx.AcceptToMemoryPool(true,maxTxFee)) {
            err = "TX rejected";
            return err;
        }

        SyncWithWallets(tx, NULL);
    }
    RelayTransaction(tx);

    return std::string("OK! Sent retrieval transaction with txid \n") + hashTx.GetHex();
}

bool SendByDelegate(CWallet* wallet,CUniqreditAddress const& address,int64_t const& nAmount,CAddress& sufficient) {

    CScript address_script;

    address_script= GetScriptForDestination(address.Get());

    std::map<CAddress, uint64_t> advertised_balances = ListAdvertisedBalances();

    bool found = false;

    //find delegate candidate
    for (std::map< CAddress, uint64_t >::const_iterator address = advertised_balances.begin(); advertised_balances.end() != address; address++ ) {
        if (nAmount <= (int64_t)address->second) {
            found = true;
            sufficient = address->first;
            break;
        }
    }

    if (!found) {
        return false;
    }

    CNetAddr const local = sufficient;

    vector<unsigned char> identification(16);

    for (int filling = 0;16 > filling;filling++) {
        identification[filling] = local.GetByte(15 - filling);
    }

    uint64_t const join_nonce = GetRand(std::numeric_limits<uint64_t>::max());

    std::vector<unsigned char> const key = wallet->store_delegate_attempt(false,local,sufficient,address_script,nAmount);

    wallet->store_join_nonce_delegate(join_nonce, key);

    CMutableTransaction rawTx;

    CTxOut transfer;
    transfer.scriptPubKey = CScript() << join_nonce << identification << key;
    transfer.scriptPubKey += address_script;
    transfer.nValue = nAmount;

    rawTx.vout.push_back(transfer);
    try {
        PushOffChain(sufficient, "request-delegate", rawTx);
    }   catch (std::exception& e) {
            PrintExceptionContinue(&e, "SendByDelegate()");
            return false;
    }
    return true;
}

void SignDelegateBind(CWallet* wallet,CMutableTransaction& mergedTx, CUniqreditAddress const& address) {
    
	for ( vector<CTxOut>::iterator output = mergedTx.vout.begin();mergedTx.vout.end() != output;output++) {
        bool at_data = false;
        CScript with_signature;
        opcodetype opcode;
        std::vector<unsigned char> vch;
        CScript::const_iterator pc = output->scriptPubKey.begin();
        while (pc < output->scriptPubKey.end())
        {
            if (!output->scriptPubKey.GetOp(pc, opcode, vch))
            {
                throw runtime_error("error parsing script");
            }
            if (0 <= opcode && opcode <= OP_PUSHDATA4) {
                with_signature << vch;
                if (at_data) {
                    at_data = false;
                    with_signature << OP_DUP;
                    uint256 hash = Hash(vch.begin(), vch.end());

                    if (!Sign1(boost::get<CKeyID>(address.Get()),*wallet,hash,SIGHASH_ALL, with_signature)) {
                        throw runtime_error("data signing failed");
                    }

                    CPubKey public_key;
                    wallet->GetPubKey(
                        boost::get<CKeyID>(address.Get()),
                        public_key
                    );
                    with_signature << public_key;
                    with_signature << OP_CHECKDATASIG << OP_VERIFY;
                    with_signature << OP_SWAP << OP_HASH160 << OP_EQUAL;
                    with_signature << OP_VERIFY;
                }
            }
            else {
                with_signature << opcode;
                if (OP_IF == opcode) {
                    at_data = true;
                }
            }
        }
        output->scriptPubKey = with_signature;
    }
}

void SignSenderBind(CWallet* wallet, CMutableTransaction& mergedTx, CUniqreditAddress const& address) {
   
    for (vector<CTxOut>::iterator output = mergedTx.vout.begin(); mergedTx.vout.end() != output; output++) {
        int at_data = 0;
        CScript with_signature;
        opcodetype opcode;
        std::vector<unsigned char> vch;
        CScript::const_iterator pc = output->scriptPubKey.begin();
        while (pc < output->scriptPubKey.end())
        {
            if (!output->scriptPubKey.GetOp(pc, opcode, vch))
            {
                throw runtime_error("error parsing script");
            }
            if (0 <= opcode && opcode <= OP_PUSHDATA4) {
                with_signature << vch;
                if (2 == at_data) {
                    at_data = 0;
                    with_signature << OP_DUP;
                    uint256 hash = Hash(vch.begin(), vch.end());

                    if (!Sign1(boost::get<CKeyID>(address.Get()), *wallet, hash, SIGHASH_ALL, with_signature)) {
                        throw runtime_error("data signing failed");
                    }

                    CPubKey public_key;
                    wallet->GetPubKey(boost::get<CKeyID>(address.Get()), public_key);
                    with_signature << public_key;
                    with_signature << OP_CHECKDATASIG << OP_VERIFY;
                    with_signature << OP_SWAP << OP_HASH160 << OP_EQUAL;
                    with_signature << OP_VERIFY;
                }
            }
            else {
                with_signature << opcode;
                if (OP_IF == opcode) {
                    at_data++;
                } else {
                    at_data = 0;
                }
            }
        }
        output->scriptPubKey = with_signature;
    }
}

