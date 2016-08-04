TEMPLATE = app
TARGET = uniqredit-qt
VERSION = 0.30.19.7
INCLUDEPATH += src src/bench src/consensus src/leveldb src/policy src/rpc src/support src/univalue src/wallet src/zmq src/qt src/qt/forms src/compat src/crypto src/primitives src/script src/secp256k1/include src/univalue src/xxhash
DEFINES += QT_GUI BOOST_THREAD_USE_LIB BOOST_SPIRIT_THREADSAFE CURL_STATICLIB ENABLE_WALLET HAVE_WORKING_BOOST_SLEEP SQLITE_ENABLE_FTS5 SQLITE_ENABLE_RTREE SQLITE_ENABLE_DBSTAT_VTAB SQLITE_ENABLE_JSON1 SQLITE_ENABLE_RBU PIC PIE WANT_DENSE
CONFIG += no_include_pwd thread static release
QT += core gui network printsupport widgets sql
#QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
}

# Use command line:
#   qmake xxx.pro RELEASE=1 USE_UPNP=1 -config release QMAKE_LFLAGS+="-static-libgcc -static-libstdc++" BOOST_INCLUDE_PATH=C:/MinGW/msys/1.0/local/include BOOST_LIB_PATH=C:/MinGW/msys/1.0/local/lib BOOST_LIB_SUFFIX=-mgw46-mt-s-1_54
#   make -f Makefile.Release
#

win32 {
BOOST_LIB_SUFFIX=-mgw49-mt-s-1_57
BOOST_INCLUDE_PATH=C:/deps/boost_1_57_0
BOOST_LIB_PATH=C:/deps/boost_1_57_0/stage/lib
BDB_INCLUDE_PATH=C:/deps/db-4.8.30.NC/build_unix
BDB_LIB_PATH=C:/deps/db-4.8.30.NC/build_unix
OPENSSL_INCLUDE_PATH=C:/deps/openssl-1.0.1l/include
OPENSSL_LIB_PATH=C:/deps/openssl-1.0.1l
MINIUPNPC_INCLUDE_PATH=C:/deps/
MINIUPNPC_LIB_PATH=C:/deps/miniupnpc
QRENCODE_INCLUDE_PATH=C:/deps/qrencode-3.4.4
QRENCODE_LIB_PATH=C:/deps/qrencode-3.4.4/.libs
SQLITE_INCLUDE_PATH=C:/deps/sqlite
SQLITE_LIB_PATH=C:/deps/sqlite/.libs
LIBPNG_INCLUDE_PATH=C:/deps/libpng-1.6.16
LIBPNG_LIB_PATH=C:/deps/libpng-1.6.16/.libs
CURL_INCLUDE_PATH=C:/deps/curl/include
CURL_LIB_PATH=C:/deps/curl/lib/.libs
PROTOBUF_INCLUDE_PATH=C:/deps/protobuf-2.6.1/src
PROTOBUF_LIB_PATH=C:/deps/protobuf-2.6.1/src/.libs
}

SECP256K1_INCLUDE_PATH = $$PWD/src/secp256k1/include
SECP256K1_LIB_PATH = $$PWD/src/secp256k1/.libs

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

USE_UPNP=1
USE_QRCODE=1

# use: qmake "RELEASE=1"
contains(RELEASE, 1) {
        # Mac: compile for maximum compatibility (10.5, 32-bit)
        macx:QMAKE_CXXFLAGS += -mmacosx-version-min=10.5 -arch x86_64 -isysroot /Developer/SDKs/MacOSX10.5.sdk

    !windows:!macx {
        # Linux: static link
        LIBS += -Wl,-Bstatic -Wl,-z,relro -Wl,-z,now
    }
}

!win32 {
# for extra security against potential buffer overflows: enable GCCs Stack Smashing Protection
QMAKE_CXXFLAGS *= -fstack-protector-all --param ssp-buffer-size=1
QMAKE_LFLAGS *= -fstack-protector-all --param ssp-buffer-size=1
# We need to exclude this for Windows cross compile with MinGW 4.2.x, as it will result in a non-working executable!
# This can be enabled for Windows, when we switch to MinGW >= 4.4.x.
}
# for extra security (see: https://wiki.debian.org/Hardening): this flag is GCC compiler-specific
QMAKE_CXXFLAGS *= -D_FORTIFY_SOURCE=2
# for extra security on Windows: enable ASLR and DEP via GCC linker flags
win32:QMAKE_LFLAGS *= -Wl,--dynamicbase -Wl,--nxcompat -static
win32:QMAKE_LFLAGS += -static-libgcc -static-libstdc++

# use: qmake "USE_QRCODE=1"
# libqrencode (http://fukuchi.org/works/qrencode/index.en.html) must be installed for support
contains(USE_QRCODE, 1) {
    message(Building with QRCode support)
    DEFINES += USE_QRCODE
    LIBS += -lqrencode
}

# use: qmake "USE_UPNP=1" ( enabled by default; default)
#  or: qmake "USE_UPNP=0" (disabled by default)
#  or: qmake "USE_UPNP=-" (not supported)
# miniupnpc (http://miniupnp.free.fr/files/) must be installed for support
contains(USE_UPNP, -) {
    message(Building without UPNP support)
} else {
    message(Building with UPNP support)
    count(USE_UPNP, 0) {
        USE_UPNP=1
    }
    DEFINES += USE_UPNP=$$USE_UPNP MINIUPNP_STATICLIB
    INCLUDEPATH += $$MINIUPNPC_INCLUDE_PATH
    LIBS += $$join(MINIUPNPC_LIB_PATH,,-L,) -lminiupnpc
    win32:LIBS += -liphlpapi
}

#use: qmake "USE_DBUS=1" or qmake "USE_DBUS=0"
linux:count(USE_DBUS, 0) {
    USE_DBUS=1
}
contains(USE_DBUS, 1) {
   message(Building with DBUS (Freedesktop notifications) support)
    DEFINES += USE_DBUS
    QT += dbus
}

# use: qmake "USE_IPV6=1" ( enabled by default; default)
#  or: qmake "USE_IPV6=0" (disabled by default)
#  or: qmake "USE_IPV6=-" (not supported)
contains(USE_IPV6, -) {
    message(Building without IPv6 support)
} else {
    message(Building with IPv6 support)
    count(USE_IPV6, 0) {
        USE_IPV6=1
    }
    DEFINES += USE_IPV6=$$USE_IPV6
}

include (protobuf.pri)
PROTOS += src/qt/paymentrequest.proto \

contains(BITCOIN_NEED_QT_PLUGINS, 1) {
    DEFINES += BITCOIN_NEED_QT_PLUGINS
    QTPLUGIN += qcncodecs qjpcodecs qtwcodecs qkrcodecs qtaccessiblewidgets
}

# LIBSEC256K1 SUPPORT
QMAKE_CXXFLAGS *= -DUSE_SECP256K1

INCLUDEPATH += src/leveldb/include src/leveldb/helpers/memenv
LIBS += $$PWD/src/leveldb/libleveldb.a $$PWD/src/leveldb/libmemenv.a
!win32 {
        # we use QMAKE_CXXFLAGS_RELEASE even without RELEASE=1 because we use RELEASE to indicate linking preferences not -O preferences
        genleveldb.commands = cd $$PWD/src/leveldb && CC=$$QMAKE_CC CXX=$$QMAKE_CXX $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\" libleveldb.a libmemenv.a
} else {
        # make an educated guess about what the ranlib command is called
        isEmpty(QMAKE_RANLIB) {
                QMAKE_RANLIB = $$replace(QMAKE_STRIP, strip, ranlib)
        }
        LIBS += -lshlwapi
       # genleveldb.commands = cd $$PWD/src/leveldb && CC=$$QMAKE_CC CXX=$$QMAKE_CXX TARGET_OS=OS_WINDOWS_CROSSCOMPILE $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\" libleveldb.a libmemenv.a && $$QMAKE_RANLIB $$PWD/src/leveldb/libleveldb.a && $$QMAKE_RANLIB $$PWD/src/leveldb/libmemenv.a
}
genleveldb.target = src/leveldb/libleveldb.a
genleveldb.depends = FORCE
PRE_TARGETDEPS += src/leveldb/libleveldb.a
QMAKE_EXTRA_TARGETS += genleveldb
# Gross ugly hack that depends on qmake internals, unfortunately there is no other way to do it.
QMAKE_CLEAN += src/leveldb/libleveldb.a; cd $$PWD/src/leveldb ; $(MAKE) clean

# regenerate src/build.h
!windows|contains(USE_BUILD_INFO, 1) {
    genbuild.depends = FORCE
    genbuild.commands = cd $$PWD; /bin/sh share/genbuild.sh $$OUT_PWD/build/build.h
    genbuild.target = $$OUT_PWD/build/build.h
    PRE_TARGETDEPS += $$OUT_PWD/build/build.h
    QMAKE_EXTRA_TARGETS += genbuild
    DEFINES += HAVE_BUILD_INFO
}

contains(USE_O3, 1) {
        message(Building O3 optimization flag)
        QMAKE_CXXFLAGS_RELEASE -= -O2
        QMAKE_CFLAGS_RELEASE -= -O2
        QMAKE_CXXFLAGS += -O3
        QMAKE_CFLAGS += -O3
}

*-g++-32 {
    message("32 platform, adding -msse2 flag")

    QMAKE_CXXFLAGS += -msse2
    QMAKE_CFLAGS += -msse2
}

QMAKE_CXXFLAGS_WARN_ON = -fdiagnostics-show-option -Wall -Wextra -Wno-ignored-qualifiers -Wformat -Wformat-security -Wno-unused-parameter -Wstack-protector

# Input
DEPENDPATH += json qt
HEADERS +=  src/addrman.h \
  src/amount.h \
  src/arith_uint256.h \
  src/consensus/merkle.h \
  src/consensus/params.h \
  src/consensus/validation.h \
  src/hash.h \
  src/prevector.h \
  src/primitives/block.h \
  src/primitives/transaction.h \
  src/pubkey.h \
  src/script/interpreter.h \
  src/script/script.h \
  src/script/script_error.h \
  src/serialize.h \
  src/tinyformat.h \
  src/uint256.cpp \
  src/uint256.h \
  src/utilstrencodings.h \
  src/version.h \
  src/base58.h \
  src/bloom.h \
  src/chain.h \
  src/chainparams.h \
  src/chainparamsbase.h \
  src/chainparamsseeds.h \
  src/checkpoints.h \
  src/checkqueue.h \
  src/clientversion.h \
  src/crypto/hmac_sha256.h \
  src/crypto/common.h \
  src/crypto/hmac_sha512.h \
  src/crypto/ripemd160.h \
  src/crypto/sha1.h \
  src/crypto/sha256.h \
  src/crypto/sha512.h \
  src/coincontrol.h \
  src/coins.h \
  src/compat.h \
  src/compat/byteswap.h \
  src/compat/endian.h \
  src/compat/sanity.h \
  src/compressor.h \
  src/consensus/consensus.h \
  src/core_io.h \
  src/core_memusage.h \
  src/httprpc.h \
  src/httpserver.h \
  src/init.h \
  src/key.h \
  src/keystore.h \
  src/dbwrapper.h \
  src/limitedmap.h \
  src/loanmanager.h \
  src/main.h \
  src/memusage.h \
  src/merkleblock.h \
  src/miner.h \
  src/net.h \
  src/netbase.h \
  src/noui.h \
  src/policy/fees.h \
  src/policy/policy.h \
  src/policy/rbf.h \
  src/pow.h \
  src/protocol.h \
  src/random.h \
  src/reverselock.h \
  src/rpc/client.h \
  src/rpc/protocol.h \
  src/rpc/server.h \
  src/rpc/register.h \
  src/scheduler.h \
  src/script/sigcache.h \
  src/script/sign.h \
  src/script/standard.h \
  src/script/ismine.h \
  src/smessage.h \
  src/streams.h \
  src/support/allocators/secure.h \
  src/support/allocators/zeroafterfree.h \
  src/support/cleanse.h \
  src/support/pagelocker.h \
  src/sync.h \
  src/threadsafety.h \
  src/timedata.h \
  src/torcontrol.h \
  src/trust.h \
  src/txdb.h \
  src/txmempool.h \
  src/ui_interface.h \
  src/undo.h \
  src/util.h \
  src/utilmoneystr.h \
  src/utiltime.h \
  src/validationinterface.h \
  src/versionbits.h \
  src/wallet/crypter.h \
  src/wallet/db.h \
  src/wallet/rpcwallet.h \
  src/wallet/wallet.h \
  src/wallet/walletdb.h \
  src/zmq/zmqabstractnotifier.h \
  src/zmq/zmqconfig.h \
  src/zmq/zmqnotificationinterface.h \
  src/zmq/zmqpublishnotifier.h \
  src/qt/addressbookpage.h \
  src/qt/addresstablemodel.h \
  src/qt/askpassphrasedialog.h \
  src/qt/bantablemodel.h \
  src/qt/uniqreditaddressvalidator.h \
  src/qt/uniqreditamountfield.h \
  src/qt/uniqreditgui.h \
  src/qt/uniqreditunits.h \
  src/qt/clientmodel.h \
  src/qt/coincontroldialog.h \
  src/qt/coincontroltreewidget.h \
  src/qt/csvmodelwriter.h \
  src/qt/editaddressdialog.h \
  src/qt/guiconstants.h \
  src/qt/guiutil.h \
  src/qt/intro.h \
  src/qt/macdockiconhandler.h \
  src/qt/macnotificationhandler.h \
  src/qt/networkstyle.h \
  src/qt/notificator.h \
  src/qt/openuridialog.h \
  src/qt/optionsdialog.h \
  src/qt/optionsmodel.h \
  src/qt/overviewpage.h \
  src/qt/paymentrequestplus.h \
  src/qt/paymentserver.h \
  src/qt/peertablemodel.h \
  src/qt/platformstyle.h \
  src/qt/qvalidatedlineedit.h \
  src/qt/qvaluecombobox.h \
  src/qt/receivecoinsdialog.h \
  src/qt/receiverequestdialog.h \
  src/qt/recentrequeststablemodel.h \
  src/qt/rpcconsole.h \
  src/qt/sendcoinsdialog.h \
  src/qt/sendcoinsentry.h \
  src/qt/signverifymessagedialog.h \
  src/qt/splashscreen.h \
  src/qt/trafficgraphwidget.h \
  src/qt/transactiondesc.h \
  src/qt/transactiondescdialog.h \
  src/qt/transactionfilterproxy.h \
  src/qt/transactionrecord.h \
  src/qt/transactiontablemodel.h \
  src/qt/transactionview.h \
  src/qt/utilitydialog.h \
  src/qt/walletframe.h \
  src/qt/walletmodel.h \
  src/qt/walletmodeltransaction.h \
  src/qt/walletview.h \
  src/qt/bidpage.h \
  src/qt/p2ppage.h \
  src/qt/p2plpage.h \
  src/qt/assetspage.h \
  src/qt/utilitiespage.h \
  src/qt/blockexplorerpage.h \
  src/qt/exchangebrowserpage.h \
  src/qt/otherpage.h \
  src/qt/winshutdownmonitor.h 

SOURCES += \
  src/amount.cpp \
  src/base58.cpp \
  src/chainparams.cpp \
  src/coins.cpp \
  src/compressor.cpp \
  src/core_read.cpp \
  src/core_write.cpp \
  src/key.cpp \
  src/keystore.cpp \
  src/netbase.cpp \
  src/protocol.cpp \
  src/scheduler.cpp \
  src/script/sign.cpp \
  src/script/standard.cpp \
  src/addrman.cpp \
  src/bloom.cpp \
  src/chain.cpp \
  src/checkpoints.cpp \
  src/httprpc.cpp \
  src/httpserver.cpp \
  src/init.cpp \
  src/dbwrapper.cpp \
  src/loanmanager.cpp \
  src/main.cpp \
  src/merkleblock.cpp \
  src/miner.cpp \
  src/net.cpp \
  src/noui.cpp \
  src/policy/fees.cpp \
  src/policy/policy.cpp \
  src/pow.cpp \
  src/rest.cpp \
  src/rpc/blockchain.cpp \
  src/rpc/mining.cpp \
  src/rpc/misc.cpp \
  src/rpc/loans.cpp \
  src/rpc/net.cpp \
  src/rpc/rawtransaction.cpp \
  src/rpc/server.cpp \
  src/script/sigcache.cpp \
  src/script/ismine.cpp \
  src/smessage.cpp \
  src/timedata.cpp \
  src/torcontrol.cpp \
  src/trust.cpp \
  src/txdb.cpp \
  src/txmempool.cpp \
  src/ui_interface.cpp \
  src/validationinterface.cpp \
  src/versionbits.cpp \
  src/wallet/crypter.cpp \
  src/wallet/db.cpp \
  src/wallet/rpcdump.cpp \
  src/wallet/rpcwallet.cpp \
  src/wallet/wallet.cpp \
  src/wallet/walletdb.cpp \
  src/policy/rbf.cpp \
  src/zmq/zmqabstractnotifier.cpp \
  src/zmq/zmqnotificationinterface.cpp \
  src/zmq/zmqpublishnotifier.cpp \
  src/crypto/hmac_sha256.cpp \
  src/crypto/hmac_sha512.cpp \
  src/crypto/ripemd160.cpp \
  src/crypto/sha1.cpp \
  src/crypto/sha256.cpp \
  src/crypto/sha512.cpp \
  src/consensus/merkle.cpp \
  src/arith_uint256.cpp \
  src/hash.cpp \
  src/primitives/block.cpp \
  src/primitives/transaction.cpp \
  src/pubkey.cpp \
  src/script/uniqreditconsensus.cpp \
  src/script/interpreter.cpp \
  src/script/script.cpp \
  src/script/script_error.cpp \
  src/utilstrencodings.cpp \
  src/support/pagelocker.cpp \
  src/chainparamsbase.cpp \
  src/clientversion.cpp \
  src/compat/glibc_sanity.cpp \
  src/compat/glibcxx_sanity.cpp \
  src/compat/strnlen.cpp \
  src/random.cpp \
  src/rpc/protocol.cpp \
  src/support/cleanse.cpp \
  src/sync.cpp \
  src/util.cpp \
  src/utilmoneystr.cpp \
  src/utilstrencodings.cpp \
  src/utiltime.cpp \
  src/rpc/client.cpp \
  src/qt/bantablemodel.cpp \
  src/qt/uniqreditaddressvalidator.cpp \
  src/qt/uniqreditamountfield.cpp \
  src/qt/uniqreditgui.cpp \
  src/qt/uniqreditunits.cpp \
  src/qt/clientmodel.cpp \
  src/qt/csvmodelwriter.cpp \
  src/qt/guiutil.cpp \
  src/qt/intro.cpp \
  src/qt/networkstyle.cpp \
  src/qt/notificator.cpp \
  src/qt/optionsdialog.cpp \
  src/qt/optionsmodel.cpp \
  src/qt/peertablemodel.cpp \
  src/qt/platformstyle.cpp \
  src/qt/qvalidatedlineedit.cpp \
  src/qt/qvaluecombobox.cpp \
  src/qt/rpcconsole.cpp \
  src/qt/splashscreen.cpp \
  src/qt/trafficgraphwidget.cpp \
  src/qt/utilitydialog.cpp \
  src/qt/winshutdownmonitor.cpp \
  src/qt/addressbookpage.cpp \
  src/qt/addresstablemodel.cpp \
  src/qt/askpassphrasedialog.cpp \
  src/qt/coincontroldialog.cpp \
  src/qt/coincontroltreewidget.cpp \
  src/qt/editaddressdialog.cpp \
  src/qt/openuridialog.cpp \
  src/qt/overviewpage.cpp \
  src/qt/paymentrequestplus.cpp \
  src/qt/paymentserver.cpp \
  src/qt/receivecoinsdialog.cpp \
  src/qt/receiverequestdialog.cpp \
  src/qt/recentrequeststablemodel.cpp \
  src/qt/sendcoinsdialog.cpp \
  src/qt/sendcoinsentry.cpp \
  src/qt/signverifymessagedialog.cpp \
  src/qt/transactiondesc.cpp \
  src/qt/transactiondescdialog.cpp \
  src/qt/transactionfilterproxy.cpp \
  src/qt/transactionrecord.cpp \
  src/qt/transactiontablemodel.cpp \
  src/qt/transactionview.cpp \
  src/qt/walletframe.cpp \
  src/qt/walletmodel.cpp \
  src/qt/walletmodeltransaction.cpp \
  src/qt/bidpage.cpp \
  src/qt/p2ppage.cpp \
  src/qt/p2plpage.cpp \
  src/qt/assetspage.cpp \
  src/qt/utilitiespage.cpp \
  src/qt/blockexplorerpage.cpp \
  src/qt/exchangebrowserpage.cpp \
  src/qt/otherpage.cpp \
  src/qt/walletview.cpp

RESOURCES += \
    src/qt/uniqredit.qrc\
    src/qt/uniqredit_locale.qrc

FORMS += \
  src/qt/forms/addressbookpage.ui \
  src/qt/forms/askpassphrasedialog.ui \
  src/qt/forms/coincontroldialog.ui \
  src/qt/forms/editaddressdialog.ui \
  src/qt/forms/helpmessagedialog.ui \
  src/qt/forms/intro.ui \
  src/qt/forms/openuridialog.ui \
  src/qt/forms/optionsdialog.ui \
  src/qt/forms/overviewpage.ui \
  src/qt/forms/receivecoinsdialog.ui \
  src/qt/forms/receiverequestdialog.ui \
  src/qt/forms/debugwindow.ui \
  src/qt/forms/sendcoinsdialog.ui \
  src/qt/forms/sendcoinsentry.ui \
  src/qt/forms/signverifymessagedialog.ui \
  src/qt/forms/bidpage.ui \
  src/qt/forms/p2ppage.ui \
  src/qt/forms/p2plpage.ui \
  src/qt/forms/assetspage.ui \
  src/qt/forms/blockexplorerpage.ui \
  src/qt/forms/exchangebrowserpage.ui \
  src/qt/forms/otherpage.ui \
  src/qt/forms/utilitiespage.ui \
  src/qt/forms/transactiondescdialog.ui

CODECFORTR = UTF-8

# for lrelease/lupdate
# also add new translations to src/qt/bitcoin.qrc under translations/
TRANSLATIONS = $$files(src/qt/locale/uniqredit_*.ts)

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
isEmpty(QM_DIR):QM_DIR = $$PWD/src/qt/locale
# automatically build translations, so they can be included in resource file
TSQM.name = lrelease ${QMAKE_FILE_IN}
TSQM.input = TRANSLATIONS
TSQM.output = $$QM_DIR/${QMAKE_FILE_BASE}.qm
TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
TSQM.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += TSQM

# "Other files" to show in Qt Creator
OTHER_FILES += \
        doc/*.rst doc/*.txt doc/README README.md res/bitcoin-qt.rc

# platform specific defaults, if not overridden on command line
isEmpty(BOOST_LIB_SUFFIX) {
    macx:BOOST_LIB_SUFFIX = -mt
    windows:BOOST_LIB_SUFFIX = -mgw49-mt-s-1_54
}

isEmpty(BOOST_THREAD_LIB_SUFFIX) {
    BOOST_THREAD_LIB_SUFFIX = $$BOOST_LIB_SUFFIX
}

isEmpty(BDB_LIB_PATH) {
    macx:BDB_LIB_PATH = /opt/local/lib/db48
}


isEmpty(BDB_LIB_SUFFIX) {
    macx:BDB_LIB_SUFFIX = -4.8
}

isEmpty(BDB_INCLUDE_PATH) {
    macx:BDB_INCLUDE_PATH = /opt/local/include/db48
}

isEmpty(BOOST_LIB_PATH) {
    macx:BOOST_LIB_PATH = /opt/local/lib
}

isEmpty(BOOST_INCLUDE_PATH) {
    macx:BOOST_INCLUDE_PATH = /opt/local/include
}

windows:DEFINES += WIN32 WIN32_LEAN_AND_MEAN
windows:RC_FILE = src/qt/res/uniqredit-qt-res.rc

windows:!contains(MINGW_THREAD_BUGFIX, 0) {
    # At least qmake's win32-g++-cross profile is missing the -lmingwthrd
    # thread-safety flag. GCC has -mthreads to enable this, but it doesn't
    # work with static linking. -lmingwthrd must come BEFORE -lmingw, so
    # it is prepended to QMAKE_LIBS_QT_ENTRY.
    # It can be turned off with MINGW_THREAD_BUGFIX=0, just in case it causes
    # any problems on some untested qmake profile now or in the future.
    DEFINES += _MT BOOST_THREAD_PROVIDES_GENERIC_SHARED_MUTEX_ON_WIN
    QMAKE_LIBS_QT_ENTRY = -lmingwthrd $$QMAKE_LIBS_QT_ENTRY
}

macx:HEADERS += qt/macdockiconhandler.h
macx:OBJECTIVE_SOURCES += qt/macdockiconhandler.mm
macx:LIBS += -framework Foundation -framework ApplicationServices -framework AppKit
macx:DEFINES += MAC_OSX MSG_NOSIGNAL=0
macx:ICON = src/qt/res/icons/uniqredit.icns
macx:TARGET = "Bitcredit-Qt"
macx:QMAKE_CFLAGS_THREAD += -pthread
macx:QMAKE_LFLAGS_THREAD += -pthread
macx:QMAKE_CXXFLAGS_THREAD += -pthread
macx:QMAKE_INFO_PLIST = share/qt/Info.plist

# Set libraries and includes at end, to use platform-defined defaults if not overridden
INCLUDEPATH += $$PROTOBUF_INCLUDE_PATH $$SQLITE_INCLUDE_PATH $$LIBPNG_INCLUDE_PATH $$SECP256K1_INCLUDE_PATH $$BOOST_INCLUDE_PATH $$BDB_INCLUDE_PATH $$OPENSSL_INCLUDE_PATH $$QRENCODE_INCLUDE_PATH $$CURL_INCLUDE_PATH
LIBS += $$join(LIBPNG_LIB_PATH,,-L,) $$join(PROTOBUF_LIB_PATH,,-L,) $$join(SQLITE_LIB_PATH,,-L,)$$join(SECP256K1_LIB_PATH,,-L,) $$join(BOOST_LIB_PATH,,-L,) $$join(BDB_LIB_PATH,,-L,) $$join(OPENSSL_LIB_PATH,,-L,) $$join(QRENCODE_LIB_PATH,,-L,) $$join(CURL_LIB_PATH,,-L,)
LIBS += -lssl -lcrypto -ldb_cxx$$BDB_LIB_SUFFIX -lsecp256k1  -lprotobuf -lcurl -lanl -lsqlite3 -lqsqlite
# -lgdi32 has to happen after -lcrypto (see  #681)
windows:LIBS += -lws2_32 -lshlwapi -lmswsock -lole32 -loleaut32 -luuid -lgdi32 -lpthread -static
LIBS += -lboost_system$$BOOST_LIB_SUFFIX -lboost_filesystem$$BOOST_LIB_SUFFIX -lboost_program_options$$BOOST_LIB_SUFFIX -lboost_thread$$BOOST_THREAD_LIB_SUFFIX -lboost_regex$$BOOST_LIB_SUFFIX
windows:LIBS += -lboost_chrono$$BOOST_LIB_SUFFIX

contains(RELEASE, 1) {
    !windows:!macx {
        # Linux: turn dynamic linking back on for c/c++ runtime libraries
        LIBS += -Wl,-Bdynamic
    }
}

wince {
    DEPLOYMENT_PLUGIN += qsqlite
}

!windows:!macx {
    DEFINES += LINUX
    LIBS += -lrt -ldl
}
#QMAKE_EXTRA_COMPILERS += protobuf_cc
system($$QMAKE_LRELEASE -silent $$_PRO_FILE_)
