Bitfilcoin integration/staging tree
================================

http://www.bitfilcoin.com

Copyright (c) 2009-2014 Bitcoin Developers
Copyright (c) 2011-2014 Litecoin Developers
Copyright (c) 2018 Bitfilcoin Developers

What is Bitfilcoin?
----------------

Bitfilcoin is a lite version of Bitcoin using scrypt as a proof-of-work algorithm.
 - 2.5 minute block targets
 - subsidy halves in 840k blocks (~4 years)
 - ~8300 million total coins


For more information, as well as an immediately useable, binary version of
the Bitfilcoin client sofware, see http://www.bitfilcoin.com.

### Automated Testing

Developers are strongly encouraged to write unit tests for new code, and to
submit new unit tests for old code.

Unit tests for the core code are in `src/test/`. To compile and run them:

    cd src; make -f makefile.unix test

Unit tests for the GUI code are in `src/qt/test/`. To compile and run them:

    qmake BITCOIN_QT_TEST=1 -o Makefile.test bitcoin-qt.pro
    make -f Makefile.test
    ./bitfilcoin-qt_test

