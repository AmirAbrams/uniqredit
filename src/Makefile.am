DIST_SUBDIRS = secp256k1 univalue

AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS) $(HARDENED_LDFLAGS)
AM_CXXFLAGS = $(HARDENED_CXXFLAGS)
AM_CPPFLAGS = $(HARDENED_CPPFLAGS)
EXTRA_LIBRARIES =
LIBS += -lsqlite3
if EMBEDDED_UNIVALUE
LIBUNIVALUE = univalue/libunivalue.la

$(LIBUNIVALUE): $(wildcard univalue/lib/*) $(wildcard univalue/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)
else
LIBUNIVALUE = $(UNIVALUE_LIBS)
endif

UNIQREDIT_CONFIG_INCLUDES=-I$(builddir)/config
UNIQREDIT_INCLUDES=-I$(builddir) -I$(builddir)/obj $(BDB_CPPFLAGS) $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) $(CRYPTO_CFLAGS) $(SSL_CFLAGS)

UNIQREDIT_INCLUDES += -I$(srcdir)/secp256k1/include
UNIQREDIT_INCLUDES += $(UNIVALUE_CFLAGS)

LIBUNIQREDIT_SERVER=libuniqredit_server.a
LIBUNIQREDIT_COMMON=libuniqredit_common.a
LIBUNIQREDIT_CONSENSUS=libuniqredit_consensus.a
LIBUNIQREDIT_CLI=libuniqredit_cli.a
LIBUNIQREDIT_UTIL=libuniqredit_util.a
LIBUNIQREDIT_CRYPTO=crypto/libuniqredit_crypto.a
LIBUNIQREDITQT=qt/libuniqreditqt.a
LIBSECP256K1=secp256k1/libsecp256k1.la

if ENABLE_ZMQ
LIBUNIQREDIT_ZMQ=libuniqredit_zmq.a
endif
if BUILD_UNIQREDIT_LIBS
LIBUNIQREDITCONSENSUS=libuniqreditconsensus.la
endif
if ENABLE_WALLET
LIBUNIQREDIT_WALLET=libuniqredit_wallet.a
endif

$(LIBSECP256K1): $(wildcard secp256k1/src/*) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

# Make is not made aware of per-object dependencies to avoid limiting building parallelization
# But to build the less dependent modules first, we manually select their order here:
EXTRA_LIBRARIES += \
  $(LIBUNIQREDIT_CRYPTO) \
  $(LIBUNIQREDIT_UTIL) \
  $(LIBUNIQREDIT_COMMON) \
  $(LIBUNIQREDIT_CONSENSUS) \
  $(LIBUNIQREDIT_SERVER) \
  $(LIBUNIQREDIT_CLI) \
  $(LIBUNIQREDIT_WALLET) \
  $(LIBUNIQREDIT_ZMQ)

lib_LTLIBRARIES = $(LIBUNIQREDITCONSENSUS)

bin_PROGRAMS =
TESTS =
BENCHMARKS =

if BUILD_UNIQREDITD
  bin_PROGRAMS += uniqreditd
endif

if BUILD_UNIQREDIT_UTILS
  bin_PROGRAMS += uniqredit-cli uniqredit-tx
endif

.PHONY: FORCE check-symbols check-security
# uniqredit core #
UNIQREDIT_CORE_H = \
  addrman.h \
  base58.h \
  bloom.h \
  blockencodings.h \
  chain.h \
  chainparams.h \
  chainparamsbase.h \
  chainparamsseeds.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coincontrol.h \
  coins.h \
  compat.h \
  compat/byteswap.h \
  compat/endian.h \
  compat/sanity.h \
  compressor.h \
  consensus/consensus.h \
  core_io.h \
  core_memusage.h \
  httprpc.h \
  httpserver.h \
  indirectmap.h \
  init.h \
  key.h \
  keystore.h \
  dbwrapper.h \
  limitedmap.h \
  loanmanager.h \
  main.h \
  memusage.h \
  merkleblock.h \
  miner.h \
  net.h \
  netbase.h \
  noui.h \
  policy/fees.h \
  policy/policy.h \
  policy/rbf.h \
  pow.h \
  protocol.h \
  random.h \
  reverselock.h \
  rpc/client.h \
  rpc/protocol.h \
  rpc/server.h \
  rpc/register.h \
  scheduler.h \
  script/sigcache.h \
  script/sign.h \
  script/standard.h \
  script/ismine.h \
  streams.h \
  support/allocators/secure.h \
  support/allocators/zeroafterfree.h \
  support/cleanse.h \
  support/pagelocker.h \
  sync.h \
  threadsafety.h \
  timedata.h \
  torcontrol.h \
  trust.h \
  txdb.h \
  txmempool.h \
  ui_interface.h \
  undo.h \
  util.h \
  utilmoneystr.h \
  utiltime.h \
  validationinterface.h \
  versionbits.h \
  wallet/crypter.h \
  wallet/db.h \
  wallet/rpcwallet.h \
  wallet/wallet.h \
  wallet/walletdb.h \
  zmq/zmqabstractnotifier.h \
  zmq/zmqconfig.h\
  zmq/zmqnotificationinterface.h \
  zmq/zmqpublishnotifier.h


obj/build.h: FORCE
	@$(MKDIR_P) $(builddir)/obj
	@$(top_srcdir)/share/genbuild.sh $(abs_top_builddir)/src/obj/build.h \
	  $(abs_top_srcdir)
libuniqredit_util_a-clientversion.$(OBJEXT): obj/build.h

# server: shared between uniqreditd and uniqredit-qt
libuniqredit_server_a_CPPFLAGS = $(AM_CPPFLAGS) $(UNIQREDIT_INCLUDES) $(MINIUPNPC_CPPFLAGS) $(EVENT_CFLAGS) $(EVENT_PTHREADS_CFLAGS)
libuniqredit_server_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libuniqredit_server_a_SOURCES = \
  addrman.cpp \
  bloom.cpp \
  blockencodings.cpp \
  chain.cpp \
  checkpoints.cpp \
  httprpc.cpp \
  httpserver.cpp \
  init.cpp \
  dbwrapper.cpp \
  loanmanager.cpp \
  main.cpp \
  merkleblock.cpp \
  miner.cpp \
  net.cpp \
  noui.cpp \
  policy/fees.cpp \
  policy/policy.cpp \
  pow.cpp \
  rest.cpp \
  rpc/blockchain.cpp \
  rpc/mining.cpp \
  rpc/misc.cpp \
  rpc/loans.cpp \
  rpc/net.cpp \
  rpc/rawtransaction.cpp \
  rpc/server.cpp \
  script/sigcache.cpp \
  script/ismine.cpp \
  timedata.cpp \
  torcontrol.cpp \
  trust.cpp \
  txdb.cpp \
  txmempool.cpp \
  ui_interface.cpp \
  validationinterface.cpp \
  versionbits.cpp \
  $(UNIQREDIT_CORE_H)

if ENABLE_ZMQ
libuniqredit_zmq_a_CPPFLAGS = $(UNIQREDIT_INCLUDES) $(ZMQ_CFLAGS)
libuniqredit_zmq_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libuniqredit_zmq_a_SOURCES = \
  zmq/zmqabstractnotifier.cpp \
  zmq/zmqnotificationinterface.cpp \
  zmq/zmqpublishnotifier.cpp
endif


# wallet: shared between uniqreditd and uniqredit-qt, but only linked
# when wallet enabled
libuniqredit_wallet_a_CPPFLAGS = $(AM_CPPFLAGS) $(UNIQREDIT_INCLUDES)
libuniqredit_wallet_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libuniqredit_wallet_a_SOURCES = \
  wallet/crypter.cpp \
  wallet/db.cpp \
  wallet/rpcdump.cpp \
  wallet/rpcwallet.cpp \
  wallet/wallet.cpp \
  wallet/walletdb.cpp \
  policy/rbf.cpp \
  $(UNIQREDIT_CORE_H)

# crypto primitives library
crypto_libuniqredit_crypto_a_CPPFLAGS = $(AM_CPPFLAGS) $(UNIQREDIT_CONFIG_INCLUDES)
crypto_libuniqredit_crypto_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
crypto_libuniqredit_crypto_a_SOURCES = \
  crypto/aes.cpp \
  crypto/aes.h \
  crypto/common.h \
  crypto/hmac_sha256.cpp \
  crypto/hmac_sha256.h \
  crypto/hmac_sha512.cpp \
  crypto/hmac_sha512.h \
  crypto/ripemd160.cpp \
  crypto/ripemd160.h \
  crypto/sha1.cpp \
  crypto/sha1.h \
  crypto/sha256.cpp \
  crypto/sha256.h \
  crypto/sha512.cpp \
  crypto/sha512.h

# consensus: shared between all executables that validate any consensus rules.
libuniqredit_consensus_a_CPPFLAGS = $(AM_CPPFLAGS) $(UNIQREDIT_INCLUDES)
libuniqredit_consensus_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libuniqredit_consensus_a_SOURCES = \
  amount.h \
  arith_uint256.cpp \
  arith_uint256.h \
  consensus/merkle.cpp \
  consensus/merkle.h \
  consensus/params.h \
  consensus/validation.h \
  hash.cpp \
  hash.h \
  prevector.h \
  primitives/block.cpp \
  primitives/block.h \
  primitives/transaction.cpp \
  primitives/transaction.h \
  pubkey.cpp \
  pubkey.h \
  script/uniqreditconsensus.cpp \
  script/interpreter.cpp \
  script/interpreter.h \
  script/script.cpp \
  script/script.h \
  script/script_error.cpp \
  script/script_error.h \
  serialize.h \
  tinyformat.h \
  uint256.cpp \
  uint256.h \
  utilstrencodings.cpp \
  utilstrencodings.h \
  version.h

# common: shared between uniqreditd, and uniqredit-qt and non-server tools
libuniqredit_common_a_CPPFLAGS = $(AM_CPPFLAGS) $(UNIQREDIT_INCLUDES)
libuniqredit_common_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libuniqredit_common_a_SOURCES = \
  amount.cpp \
  base58.cpp \
  chainparams.cpp \
  coins.cpp \
  compressor.cpp \
  core_read.cpp \
  core_write.cpp \
  key.cpp \
  keystore.cpp \
  netbase.cpp \
  protocol.cpp \
  scheduler.cpp \
  script/sign.cpp \
  script/standard.cpp \
  $(UNIQREDIT_CORE_H)

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libuniqredit_util_a_CPPFLAGS = $(AM_CPPFLAGS) $(UNIQREDIT_INCLUDES)
libuniqredit_util_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libuniqredit_util_a_SOURCES = \
  support/pagelocker.cpp \
  chainparamsbase.cpp \
  clientversion.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  compat/strnlen.cpp \
  random.cpp \
  rpc/protocol.cpp \
  support/cleanse.cpp \
  sync.cpp \
  util.cpp \
  utilmoneystr.cpp \
  utilstrencodings.cpp \
  utiltime.cpp \
  $(UNIQREDIT_CORE_H)

if GLIBC_BACK_COMPAT
libuniqredit_util_a_SOURCES += compat/glibc_compat.cpp
endif

# cli: shared between uniqredit-cli and uniqredit-qt
libuniqredit_cli_a_CPPFLAGS = $(AM_CPPFLAGS) $(UNIQREDIT_INCLUDES)
libuniqredit_cli_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libuniqredit_cli_a_SOURCES = \
  rpc/client.cpp \
  $(UNIQREDIT_CORE_H)

nodist_libuniqredit_util_a_SOURCES = $(srcdir)/obj/build.h
#

# uniqreditd binary #
uniqreditd_SOURCES = uniqreditd.cpp
uniqreditd_CPPFLAGS = $(AM_CPPFLAGS) $(UNIQREDIT_INCLUDES)
uniqreditd_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
uniqreditd_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
uniqreditd_SOURCES += uniqreditd-res.rc
endif

uniqreditd_LDADD = \
  $(LIBUNIQREDIT_SERVER) \
  $(LIBUNIQREDIT_COMMON) \
  $(LIBUNIVALUE) \
  $(LIBUNIQREDIT_UTIL) \
  $(LIBUNIQREDIT_WALLET) \
  $(LIBUNIQREDIT_ZMQ) \
  $(LIBUNIQREDIT_CONSENSUS) \
  $(LIBUNIQREDIT_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBMEMENV) \
  $(LIBSECP256K1)

uniqreditd_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(MINIUPNPC_LIBS) $(EVENT_PTHREADS_LIBS) $(EVENT_LIBS) $(ZMQ_LIBS)

# uniqredit-cli binary #
uniqredit_cli_SOURCES = uniqredit-cli.cpp
uniqredit_cli_CPPFLAGS = $(AM_CPPFLAGS) $(UNIQREDIT_INCLUDES) $(EVENT_CFLAGS)
uniqredit_cli_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
uniqredit_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
uniqredit_cli_SOURCES += uniqredit-cli-res.rc
endif

uniqredit_cli_LDADD = \
  $(LIBUNIQREDIT_CLI) \
  $(LIBUNIVALUE) \
  $(LIBUNIQREDIT_UTIL) \
  $(LIBUNIQREDIT_CRYPTO)

uniqredit_cli_LDADD += $(BOOST_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(EVENT_LIBS)
#

# uniqredit-tx binary #
uniqredit_tx_SOURCES = uniqredit-tx.cpp
uniqredit_tx_CPPFLAGS = $(AM_CPPFLAGS) $(UNIQREDIT_INCLUDES)
uniqredit_tx_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
uniqredit_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
uniqredit_tx_SOURCES += uniqredit-tx-res.rc
endif

uniqredit_tx_LDADD = \
  $(LIBUNIVALUE) \
  $(LIBUNIQREDIT_COMMON) \
  $(LIBUNIQREDIT_UTIL) \
  $(LIBUNIQREDIT_CONSENSUS) \
  $(LIBUNIQREDIT_CRYPTO) \
  $(LIBSECP256K1)

uniqredit_tx_LDADD += $(BOOST_LIBS) $(CRYPTO_LIBS)
#

# uniqreditconsensus library #
if BUILD_UNIQREDIT_LIBS
include_HEADERS = script/uniqreditconsensus.h
libuniqreditconsensus_la_SOURCES = $(crypto_libuniqredit_crypto_a_SOURCES) $(libuniqredit_consensus_a_SOURCES)

if GLIBC_BACK_COMPAT
  libuniqreditconsensus_la_SOURCES += compat/glibc_compat.cpp
endif

libuniqreditconsensus_la_LDFLAGS = $(AM_LDFLAGS) -no-undefined $(RELDFLAGS)
libuniqreditconsensus_la_LIBADD = $(LIBSECP256K1)
libuniqreditconsensus_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(builddir)/obj -I$(srcdir)/secp256k1/include -DBUILD_UNIQREDIT_INTERNAL
libuniqreditconsensus_la_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)

endif
#

CTAES_DIST =  crypto/ctaes/bench.c
CTAES_DIST += crypto/ctaes/ctaes.c
CTAES_DIST += crypto/ctaes/ctaes.h
CTAES_DIST += crypto/ctaes/README.md
CTAES_DIST += crypto/ctaes/test.c

CLEANFILES = $(EXTRA_LIBRARIES)

CLEANFILES += *.gcda *.gcno
CLEANFILES += compat/*.gcda compat/*.gcno
CLEANFILES += consensus/*.gcda consensus/*.gcno
CLEANFILES += crypto/*.gcda crypto/*.gcno
CLEANFILES += policy/*.gcda policy/*.gcno
CLEANFILES += primitives/*.gcda primitives/*.gcno
CLEANFILES += script/*.gcda script/*.gcno
CLEANFILES += support/*.gcda support/*.gcno
CLEANFILES += univalue/*.gcda univalue/*.gcno
CLEANFILES += wallet/*.gcda wallet/*.gcno
CLEANFILES += wallet/test/*.gcda wallet/test/*.gcno
CLEANFILES += zmq/*.gcda zmq/*.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = $(CTAES_DIST)

clean-local:
	-$(MAKE) -C secp256k1 clean
	-$(MAKE) -C univalue clean
	-rm -f leveldb/*/*.gcda leveldb/*/*.gcno leveldb/helpers/memenv/*.gcda leveldb/helpers/memenv/*.gcno
	-rm -f config.h
	-rm -rf test/__pycache__

.rc.o:
	@test -f $(WINDRES)
	## FIXME: How to get the appropriate modulename_CPPFLAGS in here?
	$(AM_V_GEN) $(WINDRES) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) -DWINDRES_PREPROC -i $< -o $@

.mm.o:
	$(AM_V_CXX) $(OBJCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	  $(CPPFLAGS) $(AM_CXXFLAGS) $(QT_INCLUDES) $(AM_CXXFLAGS) $(PIE_FLAGS) $(CXXFLAGS) -c -o $@ $<

check-symbols: $(bin_PROGRAMS)
if GLIBC_BACK_COMPAT
	@echo "Checking glibc back compat..."
	$(AM_V_at) READELF=$(READELF) CPPFILT=$(CPPFILT) $(top_srcdir)/contrib/devtools/symbol-check.py < $(bin_PROGRAMS)
endif

check-security: $(bin_PROGRAMS)
if HARDEN
	@echo "Checking binary security..."
	$(AM_V_at) READELF=$(READELF) OBJDUMP=$(OBJDUMP) $(top_srcdir)/contrib/devtools/security-check.py < $(bin_PROGRAMS)
endif

%.pb.cc %.pb.h: %.proto
	@test -f $(PROTOC)
	$(AM_V_GEN) $(PROTOC) --cpp_out=$(@D) --proto_path=$(<D) $<

if EMBEDDED_LEVELDB
include Makefile.leveldb.include
endif

if ENABLE_TESTS
include Makefile.test.include
endif

if ENABLE_BENCH
include Makefile.bench.include
endif

if ENABLE_QT
include Makefile.qt.include
endif

if ENABLE_QT_TESTS
include Makefile.qttest.include
endif
