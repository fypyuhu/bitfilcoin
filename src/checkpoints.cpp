// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double fSigcheckVerificationFactor = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64 nTimeLastCheckpoint;
        int64 nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (  0, uint256("0x937c2c816305b15d5f0a55477e3888cf4c7ae30a13aaedcba089804abff7a886"))
        (  10, uint256("0x6ceb228dcbaceecf03bd68c753644eebef26d3cce61430db47dc141f39ba5507"))
        (  20, uint256("0xd76463183e85c92c6791ae3ca87f7174f7b1ce4cdaffc9a3fb924aa57c788207"))
        (  30, uint256("0xabc7151fd7e8d490fe03df26aebf152ba7b4a9be1cf982e8aa4c1fd6d54b6c31"))
        (  40, uint256("0x16049e975baf72be5961ac587732b1706c2e23705701e1a5d5399a1956c2308e"))
        (  50, uint256("0xd81c2290bb1081ae57bdd7a0874d4ff8c91c54c97ffbe539850578c0ce68a0c6"))
        (  60, uint256("0x90c80d15a6ba9c8d7ee9ed2ed168b3a37b8184974ffd50b903394f86ac8a82b3"))
        (  70, uint256("0xa3b1ffbe9245206c7cd3d0288540c6f352adad892db3cc16f2dcab4bf8ac8ac7"))
        (  80, uint256("0x28ac757a9ffff3885729355df4518d07e98799cd91699e1dc12837a7baf4f9a9"))
        (  90, uint256("0x5c768f448f4428bca7f67aa9faffb094bb674a39ac163b21d8b67b0477f03180"))
        (  100, uint256("0x4f0551b8a2aadaa414c1923b1f40eb85f2159b27a57653fc0c8f0d8073b8cfee"))
        (  110, uint256("0x0e3093f26fb32dbb08d44a5c9a40e7b31f4320e22572f4eef6856349e4f7d90b"))
        ;
    static const CCheckpointData data = {
        &mapCheckpoints,
        1526335548, // * UNIX timestamp of last checkpoint block
        110,    // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        1.0     // * estimated number of transactions per day after checkpoint
    };

    static MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        (   0, uint256("0xc20db40b125102c4bd1cd717ca4717abf1cedf95a4da7fc2e2e6133810bb86ba"))
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1526335514,
        0,
        1.0
    };

    const CCheckpointData &Checkpoints() {
        if (fTestNet)
            return dataTestnet;
        else
            return data;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (!GetBoolArg("-checkpoints", true))
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex) {
        if (pindex==NULL)
            return 0.0;

        int64 nNow = time(NULL);

        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (!GetBoolArg("-checkpoints", true))
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (!GetBoolArg("-checkpoints", true))
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
}
