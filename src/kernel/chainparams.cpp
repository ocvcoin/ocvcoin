// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <hash.h>
#include <kernel/messagestartchars.h>
#include <logging.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/strencodings.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "49ers picked to win Super Bowl 56 by five NFL Network analysts";
    const CScript genesisOutputScript = CScript() << ParseHex("046175E87A68A78E99658E34E82E922AED486870534AB56194A4F209B2891108E4E1A569F9D7AF0D131A02A2ABB62D542ECE2FEF39C379EA3CE031924360E04C82") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network on which people trade goods and services.
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        m_chain_type = ChainType::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000;
        
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1; 
        consensus.BIP66Height = 1; 
        consensus.CSVHeight = 1; 
        consensus.SegwitHeight = 0; 
        consensus.MinBIP9WarningHeight = 0; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 24 * 60 * 60; // 1 day
        consensus.nPowTargetSpacing = 5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1815; // 90% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE; 
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT; 
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; 

        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000003f04b07");
        consensus.defaultAssumeValid = uint256S("0x0000000014f14ca9c9964ac55d8bd0f7cb621848a20f96530430ce7cb8def701");

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xf9;
        pchMessageStart[1] = 0xbe;
        pchMessageStart[2] = 0xb4;
        pchMessageStart[3] = 0xd7;
        nDefaultPort = 8333;
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 1;
        m_assumed_chain_state_size = 0;

        genesis = CreateGenesisBlock(1630950433, 1, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x62eca3d9086ab1cac7f63e5c0a80a893e8b1b65cca7b1b2bc2470a0f99d89689"));
        assert(genesis.hashMerkleRoot == uint256S("0xb2097d2bfc31e8937aaafe9fa7d634531274463f50e13469ba056cbddc594f1d"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as an addrfetch if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("dnsseed.ocvcoin.com."); 
        

vSeeds.emplace_back("dnsseed.ocvcoin.net.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.");
vSeeds.emplace_back("dnsseed.ocvcoin.xyz.");
vSeeds.emplace_back("dnsseed.ocvcoin.io.");
vSeeds.emplace_back("dnsseed.ocvcoin.me.");
vSeeds.emplace_back("dnsseed.ocvcoin.info.");
vSeeds.emplace_back("dnsseed.ocvcoin.ai.");
vSeeds.emplace_back("dnsseed.ocvcoin.top.");
vSeeds.emplace_back("dnsseed.ocvcoin.in.");
vSeeds.emplace_back("dnsseed.ocvcoin.eu.");
vSeeds.emplace_back("dnsseed.ocvcoin.online.");
vSeeds.emplace_back("dnsseed.ocvcoin.us.");
vSeeds.emplace_back("dnsseed.ocvcoin.dev.");
vSeeds.emplace_back("dnsseed.ocvcoin.gg.");
vSeeds.emplace_back("dnsseed.ocvcoin.tech.");
vSeeds.emplace_back("dnsseed.ocvcoin.biz.");
vSeeds.emplace_back("dnsseed.ocvcoin.cc.");
vSeeds.emplace_back("dnsseed.ocvcoin.de.");
vSeeds.emplace_back("dnsseed.ocvcoin.tv.");
vSeeds.emplace_back("dnsseed.ocvcoin.pw.");
vSeeds.emplace_back("dnsseed.ocvcoin.app.");
vSeeds.emplace_back("dnsseed.ocvcoin.club.");
vSeeds.emplace_back("dnsseed.ocvcoin.pro.");
vSeeds.emplace_back("dnsseed.ocvcoin.shop.");
vSeeds.emplace_back("dnsseed.ocvcoin.uk.");
vSeeds.emplace_back("dnsseed.ocvcoin.site.");
vSeeds.emplace_back("dnsseed.ocvcoin.ca.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.uk.");
vSeeds.emplace_back("dnsseed.ocvcoin.store.");
vSeeds.emplace_back("dnsseed.ocvcoin.space.");
vSeeds.emplace_back("dnsseed.ocvcoin.win.");
vSeeds.emplace_back("dnsseed.ocvcoin.it.");
vSeeds.emplace_back("dnsseed.ocvcoin.link.");
vSeeds.emplace_back("dnsseed.ocvcoin.es.");
vSeeds.emplace_back("dnsseed.ocvcoin.one.");
vSeeds.emplace_back("dnsseed.ocvcoin.cloud.");
vSeeds.emplace_back("dnsseed.ocvcoin.ltd.");
vSeeds.emplace_back("dnsseed.ocvcoin.ru.");
vSeeds.emplace_back("dnsseed.ocvcoin.live.");
vSeeds.emplace_back("dnsseed.ocvcoin.download.");
vSeeds.emplace_back("dnsseed.ocvcoin.work.");
vSeeds.emplace_back("dnsseed.ocvcoin.sh.");
vSeeds.emplace_back("dnsseed.ocvcoin.im.");
vSeeds.emplace_back("dnsseed.ocvcoin.to.");
vSeeds.emplace_back("dnsseed.ocvcoin.ws.");
vSeeds.emplace_back("dnsseed.ocvcoin.art.");
vSeeds.emplace_back("dnsseed.ocvcoin.ch.");
vSeeds.emplace_back("dnsseed.ocvcoin.id.");
vSeeds.emplace_back("dnsseed.ocvcoin.rs.");
vSeeds.emplace_back("dnsseed.ocvcoin.life.");
vSeeds.emplace_back("dnsseed.ocvcoin.al.");
vSeeds.emplace_back("dnsseed.ocvcoin.li.");
vSeeds.emplace_back("dnsseed.ocvcoin.design.");
vSeeds.emplace_back("dnsseed.ocvcoin.st.");
vSeeds.emplace_back("dnsseed.ocvcoin.stream.");
vSeeds.emplace_back("dnsseed.ocvcoin.click.");
vSeeds.emplace_back("dnsseed.ocvcoin.se.");
vSeeds.emplace_back("dnsseed.ocvcoin.email.");
vSeeds.emplace_back("dnsseed.ocvcoin.network.");
vSeeds.emplace_back("dnsseed.ocvcoin.re.");
vSeeds.emplace_back("dnsseed.ocvcoin.host.");
vSeeds.emplace_back("dnsseed.ocvcoin.blog.");
vSeeds.emplace_back("dnsseed.ocvcoin.at.");
vSeeds.emplace_back("dnsseed.ocvcoin.fr.");
vSeeds.emplace_back("dnsseed.ocvcoin.nl.");
vSeeds.emplace_back("dnsseed.ocvcoin.world.");
vSeeds.emplace_back("dnsseed.ocvcoin.be.");
vSeeds.emplace_back("dnsseed.ocvcoin.zone.");
vSeeds.emplace_back("dnsseed.ocvcoin.news.");
vSeeds.emplace_back("dnsseed.ocvcoin.ng.");
vSeeds.emplace_back("dnsseed.ocvcoin.science.");
vSeeds.emplace_back("dnsseed.ocvcoin.asia.");
vSeeds.emplace_back("dnsseed.ocvcoin.lol.");
vSeeds.emplace_back("dnsseed.ocvcoin.ee.");
vSeeds.emplace_back("dnsseed.ocvcoin.website.");
vSeeds.emplace_back("dnsseed.ocvcoin.cx.");
vSeeds.emplace_back("dnsseed.ocvcoin.wtf.");
vSeeds.emplace_back("dnsseed.ocvcoin.xxx.");
vSeeds.emplace_back("dnsseed.ocvcoin.la.");
vSeeds.emplace_back("dnsseed.ocvcoin.moe.");
vSeeds.emplace_back("dnsseed.ocvcoin.digital.");
vSeeds.emplace_back("dnsseed.ocvcoin.studio.");
vSeeds.emplace_back("dnsseed.ocvcoin.su.");
vSeeds.emplace_back("dnsseed.ocvcoin.party.");
vSeeds.emplace_back("dnsseed.ocvcoin.bot.");
vSeeds.emplace_back("dnsseed.ocvcoin.vip.");
vSeeds.emplace_back("dnsseed.ocvcoin.today.");
vSeeds.emplace_back("dnsseed.ocvcoin.trade.");
vSeeds.emplace_back("dnsseed.ocvcoin.ly.");
vSeeds.emplace_back("dnsseed.ocvcoin.mobi.");
vSeeds.emplace_back("dnsseed.ocvcoin.fun.");
vSeeds.emplace_back("dnsseed.ocvcoin.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.ac.");
vSeeds.emplace_back("dnsseed.ocvcoin.mx.");
vSeeds.emplace_back("dnsseed.ocvcoin.group.");
vSeeds.emplace_back("dnsseed.ocvcoin.icu.");
vSeeds.emplace_back("dnsseed.ocvcoin.agency.");
vSeeds.emplace_back("dnsseed.ocvcoin.men.");
vSeeds.emplace_back("dnsseed.ocvcoin.wiki.");
vSeeds.emplace_back("dnsseed.ocvcoin.ooo.");
vSeeds.emplace_back("dnsseed.ocvcoin.is.");
vSeeds.emplace_back("dnsseed.ocvcoin.games.");
vSeeds.emplace_back("dnsseed.ocvcoin.media.");
vSeeds.emplace_back("dnsseed.ocvcoin.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.fm.");
vSeeds.emplace_back("dnsseed.ocvcoin.fi.");
vSeeds.emplace_back("dnsseed.ocvcoin.ro.");
vSeeds.emplace_back("dnsseed.ocvcoin.ie.");
vSeeds.emplace_back("dnsseed.ocvcoin.ae.");
vSeeds.emplace_back("dnsseed.ocvcoin.ninja.");
vSeeds.emplace_back("dnsseed.ocvcoin.best.");
vSeeds.emplace_back("dnsseed.ocvcoin.cat.");
vSeeds.emplace_back("dnsseed.ocvcoin.company.");
vSeeds.emplace_back("dnsseed.ocvcoin.do.");
vSeeds.emplace_back("dnsseed.ocvcoin.tools.");
vSeeds.emplace_back("dnsseed.ocvcoin.global.");
vSeeds.emplace_back("dnsseed.ocvcoin.red.");
vSeeds.emplace_back("dnsseed.ocvcoin.guru.");
vSeeds.emplace_back("dnsseed.ocvcoin.vc.");
vSeeds.emplace_back("dnsseed.ocvcoin.dog.");
vSeeds.emplace_back("dnsseed.ocvcoin.bar.");
vSeeds.emplace_back("dnsseed.ocvcoin.bid.");
vSeeds.emplace_back("dnsseed.ocvcoin.city.");
vSeeds.emplace_back("dnsseed.ocvcoin.codes.");
vSeeds.emplace_back("dnsseed.ocvcoin.works.");
vSeeds.emplace_back("dnsseed.ocvcoin.business.");
vSeeds.emplace_back("dnsseed.ocvcoin.am.");
vSeeds.emplace_back("dnsseed.ocvcoin.services.");
vSeeds.emplace_back("dnsseed.ocvcoin.run.");
vSeeds.emplace_back("dnsseed.ocvcoin.ne.");
vSeeds.emplace_back("dnsseed.ocvcoin.market.");
vSeeds.emplace_back("dnsseed.ocvcoin.ms.");
vSeeds.emplace_back("dnsseed.ocvcoin.center.");
vSeeds.emplace_back("dnsseed.ocvcoin.team.");
vSeeds.emplace_back("dnsseed.ocvcoin.rocks.");
vSeeds.emplace_back("dnsseed.ocvcoin.money.");
vSeeds.emplace_back("dnsseed.ocvcoin.plus.");
vSeeds.emplace_back("dnsseed.ocvcoin.finance.");
vSeeds.emplace_back("dnsseed.ocvcoin.ink.");
vSeeds.emplace_back("dnsseed.ocvcoin.travel.");
vSeeds.emplace_back("dnsseed.ocvcoin.solutions.");
vSeeds.emplace_back("dnsseed.ocvcoin.bio.");
vSeeds.emplace_back("dnsseed.ocvcoin.social.");
vSeeds.emplace_back("dnsseed.ocvcoin.pt.");
vSeeds.emplace_back("dnsseed.ocvcoin.jp.");
vSeeds.emplace_back("dnsseed.ocvcoin.technology.");
vSeeds.emplace_back("dnsseed.ocvcoin.ovh.");
vSeeds.emplace_back("dnsseed.ocvcoin.review.");
vSeeds.emplace_back("dnsseed.ocvcoin.academy.");
vSeeds.emplace_back("dnsseed.ocvcoin.international.");
vSeeds.emplace_back("dnsseed.ocvcoin.university.");
vSeeds.emplace_back("dnsseed.ocvcoin.family.");
vSeeds.emplace_back("dnsseed.ocvcoin.systems.");
vSeeds.emplace_back("dnsseed.ocvcoin.rentals.");
vSeeds.emplace_back("dnsseed.ocvcoin.domains.");
vSeeds.emplace_back("dnsseed.ocvcoin.consulting.");
vSeeds.emplace_back("dnsseed.ocvcoin.buzz.");
vSeeds.emplace_back("dnsseed.ocvcoin.sale.");
vSeeds.emplace_back("dnsseed.ocvcoin.tw.");
vSeeds.emplace_back("dnsseed.ocvcoin.name.");
vSeeds.emplace_back("dnsseed.ocvcoin.marketing.");
vSeeds.emplace_back("dnsseed.ocvcoin.tk.");
vSeeds.emplace_back("dnsseed.ocvcoin.pm.");
vSeeds.emplace_back("dnsseed.ocvcoin.ma.");
vSeeds.emplace_back("dnsseed.ocvcoin.pizza.");
vSeeds.emplace_back("dnsseed.ocvcoin.tel.");
vSeeds.emplace_back("dnsseed.ocvcoin.pub.");
vSeeds.emplace_back("dnsseed.ocvcoin.photography.");
vSeeds.emplace_back("dnsseed.ocvcoin.so.");
vSeeds.emplace_back("dnsseed.ocvcoin.training.");
vSeeds.emplace_back("dnsseed.ocvcoin.cafe.");
vSeeds.emplace_back("dnsseed.ocvcoin.expert.");
vSeeds.emplace_back("dnsseed.ocvcoin.no.");
vSeeds.emplace_back("dnsseed.ocvcoin.recipes.");
vSeeds.emplace_back("dnsseed.ocvcoin.cheap.");
vSeeds.emplace_back("dnsseed.ocvcoin.cool.");
vSeeds.emplace_back("dnsseed.ocvcoin.directory.");
vSeeds.emplace_back("dnsseed.ocvcoin.audio.");
vSeeds.emplace_back("dnsseed.ocvcoin.blue.");
vSeeds.emplace_back("dnsseed.ocvcoin.cash.");
vSeeds.emplace_back("dnsseed.ocvcoin.taxi.");
vSeeds.emplace_back("dnsseed.ocvcoin.casa.");
vSeeds.emplace_back("dnsseed.ocvcoin.engineering.");
vSeeds.emplace_back("dnsseed.ocvcoin.coffee.");
vSeeds.emplace_back("dnsseed.ocvcoin.properties.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.au.");
vSeeds.emplace_back("dnsseed.ocvcoin.tours.");
vSeeds.emplace_back("dnsseed.ocvcoin.ci.");
vSeeds.emplace_back("dnsseed.ocvcoin.nu.");
vSeeds.emplace_back("dnsseed.ocvcoin.boutique.");
vSeeds.emplace_back("dnsseed.ocvcoin.support.");
vSeeds.emplace_back("dnsseed.ocvcoin.foundation.");
vSeeds.emplace_back("dnsseed.ocvcoin.events.");
vSeeds.emplace_back("dnsseed.ocvcoin.fitness.");
vSeeds.emplace_back("dnsseed.ocvcoin.church.");
vSeeds.emplace_back("dnsseed.ocvcoin.capital.");
vSeeds.emplace_back("dnsseed.ocvcoin.gold.");
vSeeds.emplace_back("dnsseed.ocvcoin.delivery.");
vSeeds.emplace_back("dnsseed.ocvcoin.institute.");
vSeeds.emplace_back("dnsseed.ocvcoin.af.");
vSeeds.emplace_back("dnsseed.ocvcoin.software.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.in.");
vSeeds.emplace_back("dnsseed.ocvcoin.reviews.");
vSeeds.emplace_back("dnsseed.ocvcoin.edu.");
vSeeds.emplace_back("dnsseed.ocvcoin.watch.");
vSeeds.emplace_back("dnsseed.ocvcoin.webcam.");
vSeeds.emplace_back("dnsseed.ocvcoin.love.");
vSeeds.emplace_back("dnsseed.ocvcoin.auction.");
vSeeds.emplace_back("dnsseed.ocvcoin.toys.");
vSeeds.emplace_back("dnsseed.ocvcoin.legal.");
vSeeds.emplace_back("dnsseed.ocvcoin.express.");
vSeeds.emplace_back("dnsseed.ocvcoin.partners.");
vSeeds.emplace_back("dnsseed.ocvcoin.si.");
vSeeds.emplace_back("dnsseed.ocvcoin.coach.");
vSeeds.emplace_back("dnsseed.ocvcoin.style.");
vSeeds.emplace_back("dnsseed.ocvcoin.pp.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.date.");
vSeeds.emplace_back("dnsseed.ocvcoin.bz.");
vSeeds.emplace_back("dnsseed.ocvcoin.town.");
vSeeds.emplace_back("dnsseed.ocvcoin.energy.");
vSeeds.emplace_back("dnsseed.ocvcoin.report.");
vSeeds.emplace_back("dnsseed.ocvcoin.photos.");
vSeeds.emplace_back("dnsseed.ocvcoin.inc.");
vSeeds.emplace_back("dnsseed.ocvcoin.loan.");
vSeeds.emplace_back("dnsseed.ocvcoin.ventures.");
vSeeds.emplace_back("dnsseed.ocvcoin.computer.");
vSeeds.emplace_back("dnsseed.ocvcoin.enterprises.");
vSeeds.emplace_back("dnsseed.ocvcoin.restaurant.");
vSeeds.emplace_back("dnsseed.ocvcoin.fund.");
vSeeds.emplace_back("dnsseed.ocvcoin.chat.");
vSeeds.emplace_back("dnsseed.ocvcoin.rip.");
vSeeds.emplace_back("dnsseed.ocvcoin.help.");
vSeeds.emplace_back("dnsseed.ocvcoin.cm.");
vSeeds.emplace_back("dnsseed.ocvcoin.hk.");
vSeeds.emplace_back("dnsseed.ocvcoin.as.");
vSeeds.emplace_back("dnsseed.ocvcoin.credit.");
vSeeds.emplace_back("dnsseed.ocvcoin.racing.");
vSeeds.emplace_back("dnsseed.ocvcoin.sg.");
vSeeds.emplace_back("dnsseed.ocvcoin.gay.");
vSeeds.emplace_back("dnsseed.ocvcoin.cz.");
vSeeds.emplace_back("dnsseed.ocvcoin.cricket.");
vSeeds.emplace_back("dnsseed.ocvcoin.investments.");
vSeeds.emplace_back("dnsseed.ocvcoin.band.");
vSeeds.emplace_back("dnsseed.ocvcoin.ag.");
vSeeds.emplace_back("dnsseed.ocvcoin.et.");
vSeeds.emplace_back("dnsseed.ocvcoin.bike.");
vSeeds.emplace_back("dnsseed.ocvcoin.dating.");
vSeeds.emplace_back("dnsseed.ocvcoin.press.");
vSeeds.emplace_back("dnsseed.ocvcoin.vin.");
vSeeds.emplace_back("dnsseed.ocvcoin.repair.");
vSeeds.emplace_back("dnsseed.ocvcoin.ml.");
vSeeds.emplace_back("dnsseed.ocvcoin.fyi.");
vSeeds.emplace_back("dnsseed.ocvcoin.casino.");
vSeeds.emplace_back("dnsseed.ocvcoin.vet.");
vSeeds.emplace_back("dnsseed.ocvcoin.yoga.");
vSeeds.emplace_back("dnsseed.ocvcoin.land.");
vSeeds.emplace_back("dnsseed.ocvcoin.page.");
vSeeds.emplace_back("dnsseed.ocvcoin.ax.");
vSeeds.emplace_back("dnsseed.ocvcoin.shoes.");
vSeeds.emplace_back("dnsseed.ocvcoin.show.");
vSeeds.emplace_back("dnsseed.ocvcoin.nz.");
vSeeds.emplace_back("dnsseed.ocvcoin.cyou.");
vSeeds.emplace_back("dnsseed.ocvcoin.sc.");
vSeeds.emplace_back("dnsseed.ocvcoin.coupons.");
vSeeds.emplace_back("dnsseed.ocvcoin.kim.");
vSeeds.emplace_back("dnsseed.ocvcoin.deals.");
vSeeds.emplace_back("dnsseed.ocvcoin.guide.");
vSeeds.emplace_back("dnsseed.ocvcoin.black.");
vSeeds.emplace_back("dnsseed.ocvcoin.rest.");
vSeeds.emplace_back("dnsseed.ocvcoin.pe.");
vSeeds.emplace_back("dnsseed.ocvcoin.estate.");
vSeeds.emplace_back("dnsseed.ocvcoin.bargains.");
vSeeds.emplace_back("dnsseed.ocvcoin.accountants.");
vSeeds.emplace_back("dnsseed.ocvcoin.tax.");
vSeeds.emplace_back("dnsseed.ocvcoin.wedding.");
vSeeds.emplace_back("dnsseed.ocvcoin.dance.");
vSeeds.emplace_back("dnsseed.ocvcoin.productions.");
vSeeds.emplace_back("dnsseed.ocvcoin.financial.");
vSeeds.emplace_back("dnsseed.ocvcoin.kitchen.");
vSeeds.emplace_back("dnsseed.ocvcoin.tn.");
vSeeds.emplace_back("dnsseed.ocvcoin.direct.");
vSeeds.emplace_back("dnsseed.ocvcoin.care.");
vSeeds.emplace_back("dnsseed.ocvcoin.sex.");
vSeeds.emplace_back("dnsseed.ocvcoin.promo.");
vSeeds.emplace_back("dnsseed.ocvcoin.lu.");
vSeeds.emplace_back("dnsseed.ocvcoin.moda.");
vSeeds.emplace_back("dnsseed.ocvcoin.golf.");
vSeeds.emplace_back("dnsseed.ocvcoin.tips.");
vSeeds.emplace_back("dnsseed.ocvcoin.sexy.");
vSeeds.emplace_back("dnsseed.ocvcoin.game.");
vSeeds.emplace_back("dnsseed.ocvcoin.haus.");
vSeeds.emplace_back("dnsseed.ocvcoin.tf.");
vSeeds.emplace_back("dnsseed.ocvcoin.wine.");
vSeeds.emplace_back("dnsseed.ocvcoin.pet.");
vSeeds.emplace_back("dnsseed.ocvcoin.gift.");
vSeeds.emplace_back("dnsseed.ocvcoin.blackfriday.");
vSeeds.emplace_back("dnsseed.ocvcoin.school.");
vSeeds.emplace_back("dnsseed.ocvcoin.tennis.");
vSeeds.emplace_back("dnsseed.ocvcoin.md.");
vSeeds.emplace_back("dnsseed.ocvcoin.monster.");
vSeeds.emplace_back("dnsseed.ocvcoin.exchange.");
vSeeds.emplace_back("dnsseed.ocvcoin.video.");
vSeeds.emplace_back("dnsseed.ocvcoin.forsale.");
vSeeds.emplace_back("dnsseed.ocvcoin.photo.");
vSeeds.emplace_back("dnsseed.ocvcoin.actor.");
vSeeds.emplace_back("dnsseed.ocvcoin.community.");
vSeeds.emplace_back("dnsseed.ocvcoin.accountant.");
vSeeds.emplace_back("dnsseed.ocvcoin.movie.");
vSeeds.emplace_back("dnsseed.ocvcoin.yt.");
vSeeds.emplace_back("dnsseed.ocvcoin.ge.");
vSeeds.emplace_back("dnsseed.ocvcoin.cam.");
vSeeds.emplace_back("dnsseed.ocvcoin.mc.");
vSeeds.emplace_back("dnsseed.ocvcoin.ga.");
vSeeds.emplace_back("dnsseed.ocvcoin.parts.");
vSeeds.emplace_back("dnsseed.ocvcoin.education.");
vSeeds.emplace_back("dnsseed.ocvcoin.faith.");
vSeeds.emplace_back("dnsseed.ocvcoin.hosting.");
vSeeds.emplace_back("dnsseed.ocvcoin.bi.");
vSeeds.emplace_back("dnsseed.ocvcoin.bingo.");
vSeeds.emplace_back("dnsseed.ocvcoin.engineer.");
vSeeds.emplace_back("dnsseed.ocvcoin.bet.");
vSeeds.emplace_back("dnsseed.ocvcoin.associates.");
vSeeds.emplace_back("dnsseed.ocvcoin.ski.");
vSeeds.emplace_back("dnsseed.ocvcoin.singles.");
vSeeds.emplace_back("dnsseed.ocvcoin.farm.");
vSeeds.emplace_back("dnsseed.ocvcoin.pink.");
vSeeds.emplace_back("dnsseed.ocvcoin.claims.");
vSeeds.emplace_back("dnsseed.ocvcoin.pics.");
vSeeds.emplace_back("dnsseed.ocvcoin.careers.");
vSeeds.emplace_back("dnsseed.ocvcoin.dk.");
vSeeds.emplace_back("dnsseed.ocvcoin.gy.");
vSeeds.emplace_back("dnsseed.ocvcoin.nyc.");
vSeeds.emplace_back("dnsseed.ocvcoin.menu.");
vSeeds.emplace_back("dnsseed.ocvcoin.house.");
vSeeds.emplace_back("dnsseed.ocvcoin.ad.");
vSeeds.emplace_back("dnsseed.ocvcoin.cl.");
vSeeds.emplace_back("dnsseed.ocvcoin.uno.");
vSeeds.emplace_back("dnsseed.ocvcoin.hu.");
vSeeds.emplace_back("dnsseed.ocvcoin.cards.");
vSeeds.emplace_back("dnsseed.ocvcoin.gl.");
vSeeds.emplace_back("dnsseed.ocvcoin.tube.");
vSeeds.emplace_back("dnsseed.ocvcoin.gs.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.co.");
vSeeds.emplace_back("dnsseed.ocvcoin.doctor.");
vSeeds.emplace_back("dnsseed.ocvcoin.lt.");
vSeeds.emplace_back("dnsseed.ocvcoin.ar.");
vSeeds.emplace_back("dnsseed.ocvcoin.study.");
vSeeds.emplace_back("dnsseed.ocvcoin.rent.");
vSeeds.emplace_back("dnsseed.ocvcoin.vision.");
vSeeds.emplace_back("dnsseed.ocvcoin.mortgage.");
vSeeds.emplace_back("dnsseed.ocvcoin.shopping.");
vSeeds.emplace_back("dnsseed.ocvcoin.gratis.");
vSeeds.emplace_back("dnsseed.ocvcoin.apartments.");
vSeeds.emplace_back("dnsseed.ocvcoin.army.");
vSeeds.emplace_back("dnsseed.ocvcoin.fit.");
vSeeds.emplace_back("dnsseed.ocvcoin.college.");
vSeeds.emplace_back("dnsseed.ocvcoin.aaa.");
vSeeds.emplace_back("dnsseed.ocvcoin.builders.");
vSeeds.emplace_back("dnsseed.ocvcoin.beer.");
vSeeds.emplace_back("dnsseed.ocvcoin.irish.");
vSeeds.emplace_back("dnsseed.ocvcoin.clinic.");
vSeeds.emplace_back("dnsseed.ocvcoin.law.");
vSeeds.emplace_back("dnsseed.ocvcoin.gives.");
vSeeds.emplace_back("dnsseed.ocvcoin.ke.");
vSeeds.emplace_back("dnsseed.ocvcoin.fish.");
vSeeds.emplace_back("dnsseed.ocvcoin.sucks.");
vSeeds.emplace_back("dnsseed.ocvcoin.country.");
vSeeds.emplace_back("dnsseed.ocvcoin.porn.");
vSeeds.emplace_back("dnsseed.ocvcoin.camera.");
vSeeds.emplace_back("dnsseed.ocvcoin.vg.");
vSeeds.emplace_back("dnsseed.ocvcoin.sarl.");
vSeeds.emplace_back("dnsseed.ocvcoin.salon.");
vSeeds.emplace_back("dnsseed.ocvcoin.security.");
vSeeds.emplace_back("dnsseed.ocvcoin.attorney.");
vSeeds.emplace_back("dnsseed.ocvcoin.pictures.");
vSeeds.emplace_back("dnsseed.ocvcoin.hr.");
vSeeds.emplace_back("dnsseed.ocvcoin.solar.");
vSeeds.emplace_back("dnsseed.ocvcoin.mom.");
vSeeds.emplace_back("dnsseed.ocvcoin.equipment.");
vSeeds.emplace_back("dnsseed.ocvcoin.charity.");
vSeeds.emplace_back("dnsseed.ocvcoin.viajes.");
vSeeds.emplace_back("dnsseed.ocvcoin.christmas.");
vSeeds.emplace_back("dnsseed.ocvcoin.na.");
vSeeds.emplace_back("dnsseed.ocvcoin.discount.");
vSeeds.emplace_back("dnsseed.ocvcoin.boats.");
vSeeds.emplace_back("dnsseed.ocvcoin.ph.");
vSeeds.emplace_back("dnsseed.ocvcoin.feedback.");
vSeeds.emplace_back("dnsseed.ocvcoin.sk.");
vSeeds.emplace_back("dnsseed.ocvcoin.creditcard.");
vSeeds.emplace_back("dnsseed.ocvcoin.vegas.");
vSeeds.emplace_back("dnsseed.ocvcoin.baby.");
vSeeds.emplace_back("dnsseed.ocvcoin.clothing.");
vSeeds.emplace_back("dnsseed.ocvcoin.cab.");
vSeeds.emplace_back("dnsseed.ocvcoin.courses.");
vSeeds.emplace_back("dnsseed.ocvcoin.camp.");
vSeeds.emplace_back("dnsseed.ocvcoin.villas.");
vSeeds.emplace_back("dnsseed.ocvcoin.fail.");
vSeeds.emplace_back("dnsseed.ocvcoin.rodeo.");
vSeeds.emplace_back("dnsseed.ocvcoin.london.");
vSeeds.emplace_back("dnsseed.ocvcoin.earth.");
vSeeds.emplace_back("dnsseed.ocvcoin.lgbt.");
vSeeds.emplace_back("dnsseed.ocvcoin.ing.");
vSeeds.emplace_back("dnsseed.ocvcoin.jobs.");
vSeeds.emplace_back("dnsseed.ocvcoin.eco.");
vSeeds.emplace_back("dnsseed.ocvcoin.ht.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.uk.");
vSeeds.emplace_back("dnsseed.ocvcoin.aero.");
vSeeds.emplace_back("dnsseed.ocvcoin.cf.");
vSeeds.emplace_back("dnsseed.ocvcoin.management.");
vSeeds.emplace_back("dnsseed.ocvcoin.berlin.");
vSeeds.emplace_back("dnsseed.ocvcoin.voyage.");
vSeeds.emplace_back("dnsseed.ocvcoin.gr.");
vSeeds.emplace_back("dnsseed.ocvcoin.tienda.");
vSeeds.emplace_back("dnsseed.ocvcoin.sa.");
vSeeds.emplace_back("dnsseed.ocvcoin.reisen.");
vSeeds.emplace_back("dnsseed.ocvcoin.construction.");
vSeeds.emplace_back("dnsseed.ocvcoin.mn.");
vSeeds.emplace_back("dnsseed.ocvcoin.schule.");
vSeeds.emplace_back("dnsseed.ocvcoin.ceo.");
vSeeds.emplace_back("dnsseed.ocvcoin.tl.");
vSeeds.emplace_back("dnsseed.ocvcoin.by.");
vSeeds.emplace_back("dnsseed.ocvcoin.green.");
vSeeds.emplace_back("dnsseed.ocvcoin.car.");
vSeeds.emplace_back("dnsseed.ocvcoin.cleaning.");
vSeeds.emplace_back("dnsseed.ocvcoin.immo.");
vSeeds.emplace_back("dnsseed.ocvcoin.sx.");
vSeeds.emplace_back("dnsseed.ocvcoin.property.");
vSeeds.emplace_back("dnsseed.ocvcoin.sb.");
vSeeds.emplace_back("dnsseed.ocvcoin.gallery.");
vSeeds.emplace_back("dnsseed.ocvcoin.adult.");
vSeeds.emplace_back("dnsseed.ocvcoin.wf.");
vSeeds.emplace_back("dnsseed.ocvcoin.je.");
vSeeds.emplace_back("dnsseed.ocvcoin.lc.");
vSeeds.emplace_back("dnsseed.ocvcoin.build.");
vSeeds.emplace_back("dnsseed.ocvcoin.vacations.");
vSeeds.emplace_back("dnsseed.ocvcoin.furniture.");
vSeeds.emplace_back("dnsseed.ocvcoin.qa.");
vSeeds.emplace_back("dnsseed.ocvcoin.catering.");
vSeeds.emplace_back("dnsseed.ocvcoin.fashion.");
vSeeds.emplace_back("dnsseed.ocvcoin.supplies.");
vSeeds.emplace_back("dnsseed.ocvcoin.tt.");
vSeeds.emplace_back("dnsseed.ocvcoin.th.");
vSeeds.emplace_back("dnsseed.ocvcoin.lawyer.");
vSeeds.emplace_back("dnsseed.ocvcoin.cruises.");
vSeeds.emplace_back("dnsseed.ocvcoin.diamonds.");
vSeeds.emplace_back("dnsseed.ocvcoin.holiday.");
vSeeds.emplace_back("dnsseed.ocvcoin.holdings.");
vSeeds.emplace_back("dnsseed.ocvcoin.ba.");
vSeeds.emplace_back("dnsseed.ocvcoin.exposed.");
vSeeds.emplace_back("dnsseed.ocvcoin.tires.");
vSeeds.emplace_back("dnsseed.ocvcoin.ki.");
vSeeds.emplace_back("dnsseed.ocvcoin.bo.");
vSeeds.emplace_back("dnsseed.ocvcoin.place.");
vSeeds.emplace_back("dnsseed.ocvcoin.healthcare.");
vSeeds.emplace_back("dnsseed.ocvcoin.my.");
vSeeds.emplace_back("dnsseed.ocvcoin.homes.");
vSeeds.emplace_back("dnsseed.ocvcoin.tokyo.");
vSeeds.emplace_back("dnsseed.ocvcoin.cv.");
vSeeds.emplace_back("dnsseed.ocvcoin.airforce.");
vSeeds.emplace_back("dnsseed.ocvcoin.mu.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.za.");
vSeeds.emplace_back("dnsseed.ocvcoin.observer.");
vSeeds.emplace_back("dnsseed.ocvcoin.graphics.");
vSeeds.emplace_back("dnsseed.ocvcoin.archi.");
vSeeds.emplace_back("dnsseed.ocvcoin.mk.");
vSeeds.emplace_back("dnsseed.ocvcoin.gd.");
vSeeds.emplace_back("dnsseed.ocvcoin.gdn.");
vSeeds.emplace_back("dnsseed.ocvcoin.tc.");
vSeeds.emplace_back("dnsseed.ocvcoin.futbol.");
vSeeds.emplace_back("dnsseed.ocvcoin.poker.");
vSeeds.emplace_back("dnsseed.ocvcoin.mo.");
vSeeds.emplace_back("dnsseed.ocvcoin.mba.");
vSeeds.emplace_back("dnsseed.ocvcoin.cooking.");
vSeeds.emplace_back("dnsseed.ocvcoin.auto.");
vSeeds.emplace_back("dnsseed.ocvcoin.om.");
vSeeds.emplace_back("dnsseed.ocvcoin.onl.");
vSeeds.emplace_back("dnsseed.ocvcoin.sbs.");
vSeeds.emplace_back("dnsseed.ocvcoin.garden.");
vSeeds.emplace_back("dnsseed.ocvcoin.immobilien.");
vSeeds.emplace_back("dnsseed.ocvcoin.dentist.");
vSeeds.emplace_back("dnsseed.ocvcoin.surf.");
vSeeds.emplace_back("dnsseed.ocvcoin.au.");
vSeeds.emplace_back("dnsseed.ocvcoin.republican.");
vSeeds.emplace_back("dnsseed.ocvcoin.africa.");
vSeeds.emplace_back("dnsseed.ocvcoin.flights.");
vSeeds.emplace_back("dnsseed.ocvcoin.glass.");
vSeeds.emplace_back("dnsseed.ocvcoin.wang.");
vSeeds.emplace_back("dnsseed.ocvcoin.dental.");
vSeeds.emplace_back("dnsseed.ocvcoin.vodka.");
vSeeds.emplace_back("dnsseed.ocvcoin.jewelry.");
vSeeds.emplace_back("dnsseed.ocvcoin.contractors.");
vSeeds.emplace_back("dnsseed.ocvcoin.condos.");
vSeeds.emplace_back("dnsseed.ocvcoin.fo.");
vSeeds.emplace_back("dnsseed.ocvcoin.miami.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.ir.");
vSeeds.emplace_back("dnsseed.ocvcoin.kr.");
vSeeds.emplace_back("dnsseed.ocvcoin.dj.");
vSeeds.emplace_back("dnsseed.ocvcoin.football.");
vSeeds.emplace_back("dnsseed.ocvcoin.desi.");
vSeeds.emplace_back("dnsseed.ocvcoin.hn.");
vSeeds.emplace_back("dnsseed.ocvcoin.bg.");
vSeeds.emplace_back("dnsseed.ocvcoin.cr.");
vSeeds.emplace_back("dnsseed.ocvcoin.tattoo.");
vSeeds.emplace_back("dnsseed.ocvcoin.lease.");
vSeeds.emplace_back("dnsseed.ocvcoin.industries.");
vSeeds.emplace_back("dnsseed.ocvcoin.fans.");
vSeeds.emplace_back("dnsseed.ocvcoin.limited.");
vSeeds.emplace_back("dnsseed.ocvcoin.degree.");
vSeeds.emplace_back("dnsseed.ocvcoin.supply.");
vSeeds.emplace_back("dnsseed.ocvcoin.llc.");
vSeeds.emplace_back("dnsseed.ocvcoin.vote.");
vSeeds.emplace_back("dnsseed.ocvcoin.ec.");
vSeeds.emplace_back("dnsseed.ocvcoin.cars.");
vSeeds.emplace_back("dnsseed.ocvcoin.ist.");
vSeeds.emplace_back("dnsseed.ocvcoin.horse.");
vSeeds.emplace_back("dnsseed.ocvcoin.ps.");
vSeeds.emplace_back("dnsseed.ocvcoin.loans.");
vSeeds.emplace_back("dnsseed.ocvcoin.democrat.");
vSeeds.emplace_back("dnsseed.ocvcoin.how.");
vSeeds.emplace_back("dnsseed.ocvcoin.maison.");
vSeeds.emplace_back("dnsseed.ocvcoin.theater.");
vSeeds.emplace_back("dnsseed.ocvcoin.scot.");
vSeeds.emplace_back("dnsseed.ocvcoin.flowers.");
vSeeds.emplace_back("dnsseed.ocvcoin.soccer.");
vSeeds.emplace_back("dnsseed.ocvcoin.rehab.");
vSeeds.emplace_back("dnsseed.ocvcoin.pk.");
vSeeds.emplace_back("dnsseed.ocvcoin.vn.");
vSeeds.emplace_back("dnsseed.ocvcoin.limo.");
vSeeds.emplace_back("dnsseed.ocvcoin.health.");
vSeeds.emplace_back("dnsseed.ocvcoin.realty.");
vSeeds.emplace_back("dnsseed.ocvcoin.lv.");
vSeeds.emplace_back("dnsseed.ocvcoin.bb.");
vSeeds.emplace_back("dnsseed.ocvcoin.surgery.");
vSeeds.emplace_back("dnsseed.ocvcoin.film.");
vSeeds.emplace_back("dnsseed.ocvcoin.kz.");
vSeeds.emplace_back("dnsseed.ocvcoin.cd.");
vSeeds.emplace_back("dnsseed.ocvcoin.ky.");
vSeeds.emplace_back("dnsseed.ocvcoin.soy.");
vSeeds.emplace_back("dnsseed.ocvcoin.florist.");
vSeeds.emplace_back("dnsseed.ocvcoin.ngo.");
vSeeds.emplace_back("dnsseed.ocvcoin.quebec.");
vSeeds.emplace_back("dnsseed.ocvcoin.memorial.");
vSeeds.emplace_back("dnsseed.ocvcoin.shiksha.");
vSeeds.emplace_back("dnsseed.ocvcoin.protection.");
vSeeds.emplace_back("dnsseed.ocvcoin.diet.");
vSeeds.emplace_back("dnsseed.ocvcoin.fan.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.mx.");
vSeeds.emplace_back("dnsseed.ocvcoin.gripe.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.tr.");
vSeeds.emplace_back("dnsseed.ocvcoin.new.");
vSeeds.emplace_back("dnsseed.ocvcoin.gifts.");
vSeeds.emplace_back("dnsseed.ocvcoin.zip.");
vSeeds.emplace_back("dnsseed.ocvcoin.my.id.");
vSeeds.emplace_back("dnsseed.ocvcoin.compare.");
vSeeds.emplace_back("dnsseed.ocvcoin.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.plumbing.");
vSeeds.emplace_back("dnsseed.ocvcoin.cfd.");
vSeeds.emplace_back("dnsseed.ocvcoin.kiwi.");
vSeeds.emplace_back("dnsseed.ocvcoin.me.uk.");
vSeeds.emplace_back("dnsseed.ocvcoin.day.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.nz.");
vSeeds.emplace_back("dnsseed.ocvcoin.az.");
vSeeds.emplace_back("dnsseed.ocvcoin.bayern.");
vSeeds.emplace_back("dnsseed.ocvcoin.theatre.");
vSeeds.emplace_back("dnsseed.ocvcoin.voto.");
vSeeds.emplace_back("dnsseed.ocvcoin.ls.");
vSeeds.emplace_back("dnsseed.ocvcoin.hockey.");
vSeeds.emplace_back("dnsseed.ocvcoin.luxury.");
vSeeds.emplace_back("dnsseed.ocvcoin.lighting.");
vSeeds.emplace_back("dnsseed.ocvcoin.bs.");
vSeeds.emplace_back("dnsseed.ocvcoin.gmbh.");
vSeeds.emplace_back("dnsseed.ocvcoin.td.");
vSeeds.emplace_back("dnsseed.ocvcoin.fishing.");
vSeeds.emplace_back("dnsseed.ocvcoin.boston.");
vSeeds.emplace_back("dnsseed.ocvcoin.reise.");
vSeeds.emplace_back("dnsseed.ocvcoin.rich.");
vSeeds.emplace_back("dnsseed.ocvcoin.tm.");
vSeeds.emplace_back("dnsseed.ocvcoin.jetzt.");
vSeeds.emplace_back("dnsseed.ocvcoin.pr.");
vSeeds.emplace_back("dnsseed.ocvcoin.insure.");
vSeeds.emplace_back("dnsseed.ocvcoin.tg.");
vSeeds.emplace_back("dnsseed.ocvcoin.vu.");
vSeeds.emplace_back("dnsseed.ocvcoin.ug.");
vSeeds.emplace_back("dnsseed.ocvcoin.ni.");
vSeeds.emplace_back("dnsseed.ocvcoin.box.");
vSeeds.emplace_back("dnsseed.ocvcoin.organic.");
vSeeds.emplace_back("dnsseed.ocvcoin.hiphop.");
vSeeds.emplace_back("dnsseed.ocvcoin.cymru.");
vSeeds.emplace_back("dnsseed.ocvcoin.lk.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.de.");
vSeeds.emplace_back("dnsseed.ocvcoin.hospital.");
vSeeds.emplace_back("dnsseed.ocvcoin.ao.");
vSeeds.emplace_back("dnsseed.ocvcoin.sn.");
vSeeds.emplace_back("dnsseed.ocvcoin.lat.");
vSeeds.emplace_back("dnsseed.ocvcoin.bank.");
vSeeds.emplace_back("dnsseed.ocvcoin.yokohama.");
vSeeds.emplace_back("dnsseed.ocvcoin.guitars.");
vSeeds.emplace_back("dnsseed.ocvcoin.paris.");
vSeeds.emplace_back("dnsseed.ocvcoin.barcelona.");
vSeeds.emplace_back("dnsseed.ocvcoin.mg.");
vSeeds.emplace_back("dnsseed.ocvcoin.kaufen.");
vSeeds.emplace_back("dnsseed.ocvcoin.qpon.");
vSeeds.emplace_back("dnsseed.ocvcoin.sm.");
vSeeds.emplace_back("dnsseed.ocvcoin.storage.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.il.");
vSeeds.emplace_back("dnsseed.ocvcoin.mp.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ru.");
vSeeds.emplace_back("dnsseed.ocvcoin.edu.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.broker.");
vSeeds.emplace_back("dnsseed.ocvcoin.tickets.");
vSeeds.emplace_back("dnsseed.ocvcoin.bj.");
vSeeds.emplace_back("dnsseed.ocvcoin.autos.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.co.");
vSeeds.emplace_back("dnsseed.ocvcoin.career.");
vSeeds.emplace_back("dnsseed.ocvcoin.cy.");
vSeeds.emplace_back("dnsseed.ocvcoin.nf.");
vSeeds.emplace_back("dnsseed.ocvcoin.trading.");
vSeeds.emplace_back("dnsseed.ocvcoin.abogado.");
vSeeds.emplace_back("dnsseed.ocvcoin.aq.");
vSeeds.emplace_back("dnsseed.ocvcoin.amsterdam.");
vSeeds.emplace_back("dnsseed.ocvcoin.pa.");
vSeeds.emplace_back("dnsseed.ocvcoin.bond.");
vSeeds.emplace_back("dnsseed.ocvcoin.dz.");
vSeeds.emplace_back("dnsseed.ocvcoin.mt.");
vSeeds.emplace_back("dnsseed.ocvcoin.contact.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.in.");
vSeeds.emplace_back("dnsseed.ocvcoin.navy.");
vSeeds.emplace_back("dnsseed.ocvcoin.capetown.");
vSeeds.emplace_back("dnsseed.ocvcoin.channel.");
vSeeds.emplace_back("dnsseed.ocvcoin.markets.");
vSeeds.emplace_back("dnsseed.ocvcoin.forum.");
vSeeds.emplace_back("dnsseed.ocvcoin.bible.");
vSeeds.emplace_back("dnsseed.ocvcoin.bh.");
vSeeds.emplace_back("dnsseed.ocvcoin.sr.");
vSeeds.emplace_back("dnsseed.ocvcoin.voting.");
vSeeds.emplace_back("dnsseed.ocvcoin.juegos.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.iq.");
vSeeds.emplace_back("dnsseed.ocvcoin.wien.");
vSeeds.emplace_back("dnsseed.ocvcoin.wales.");
vSeeds.emplace_back("dnsseed.ocvcoin.srl.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.in.");
vSeeds.emplace_back("dnsseed.ocvcoin.coop.");
vSeeds.emplace_back("dnsseed.ocvcoin.kg.");
vSeeds.emplace_back("dnsseed.ocvcoin.nagoya.");
vSeeds.emplace_back("dnsseed.ocvcoin.cu.");
vSeeds.emplace_back("dnsseed.ocvcoin.mr.");
vSeeds.emplace_back("dnsseed.ocvcoin.radio.");
vSeeds.emplace_back("dnsseed.ocvcoin.aw.");
vSeeds.emplace_back("dnsseed.ocvcoin.melbourne.");
vSeeds.emplace_back("dnsseed.ocvcoin.brussels.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ar.");
vSeeds.emplace_back("dnsseed.ocvcoin.jo.");
vSeeds.emplace_back("dnsseed.ocvcoin.uz.");
vSeeds.emplace_back("dnsseed.ocvcoin.sl.");
vSeeds.emplace_back("dnsseed.ocvcoin.ve.");
vSeeds.emplace_back("dnsseed.ocvcoin.luxe.");
vSeeds.emplace_back("dnsseed.ocvcoin.dm.");
vSeeds.emplace_back("dnsseed.ocvcoin.gi.");
vSeeds.emplace_back("dnsseed.ocvcoin.gq.");
vSeeds.emplace_back("dnsseed.ocvcoin.tz.");
vSeeds.emplace_back("dnsseed.ocvcoin.sy.");
vSeeds.emplace_back("dnsseed.ocvcoin.nrw.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.tw.");
vSeeds.emplace_back("dnsseed.ocvcoin.bf.");
vSeeds.emplace_back("dnsseed.ocvcoin.realestate.");
vSeeds.emplace_back("dnsseed.ocvcoin.cologne.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ag.");
vSeeds.emplace_back("dnsseed.ocvcoin.uy.");
vSeeds.emplace_back("dnsseed.ocvcoin.gp.");
vSeeds.emplace_back("dnsseed.ocvcoin.sv.");
vSeeds.emplace_back("dnsseed.ocvcoin.boo.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.rw.");
vSeeds.emplace_back("dnsseed.ocvcoin.hamburg.");
vSeeds.emplace_back("dnsseed.ocvcoin.meme.");
vSeeds.emplace_back("dnsseed.ocvcoin.ong.");
vSeeds.emplace_back("dnsseed.ocvcoin.bzh.");
vSeeds.emplace_back("dnsseed.ocvcoin.gt.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.ag.");
vSeeds.emplace_back("dnsseed.ocvcoin.ss.");
vSeeds.emplace_back("dnsseed.ocvcoin.ltda.");
vSeeds.emplace_back("dnsseed.ocvcoin.quest.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.es.");
vSeeds.emplace_back("dnsseed.ocvcoin.physio.");
vSeeds.emplace_back("dnsseed.ocvcoin.gm.");
vSeeds.emplace_back("dnsseed.ocvcoin.eus.");
vSeeds.emplace_back("dnsseed.ocvcoin.vlaanderen.");
vSeeds.emplace_back("dnsseed.ocvcoin.sport.");
vSeeds.emplace_back("dnsseed.ocvcoin.durban.");
vSeeds.emplace_back("dnsseed.ocvcoin.hiv.");
vSeeds.emplace_back("dnsseed.ocvcoin.gf.");
vSeeds.emplace_back("dnsseed.ocvcoin.saarland.");
vSeeds.emplace_back("dnsseed.ocvcoin.eg.");
vSeeds.emplace_back("dnsseed.ocvcoin.ye.");
vSeeds.emplace_back("dnsseed.ocvcoin.ruhr.");
vSeeds.emplace_back("dnsseed.ocvcoin.ryukyu.");
vSeeds.emplace_back("dnsseed.ocvcoin.sydney.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.sg.");
vSeeds.emplace_back("dnsseed.ocvcoin.koeln.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.bo.");
vSeeds.emplace_back("dnsseed.ocvcoin.py.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.at.");
vSeeds.emplace_back("dnsseed.ocvcoin.osaka.");
vSeeds.emplace_back("dnsseed.ocvcoin.vi.");
vSeeds.emplace_back("dnsseed.ocvcoin.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.gal.");
vSeeds.emplace_back("dnsseed.ocvcoin.or.at.");
vSeeds.emplace_back("dnsseed.ocvcoin.now.");
vSeeds.emplace_back("dnsseed.ocvcoin.tj.");
vSeeds.emplace_back("dnsseed.ocvcoin.kn.");
vSeeds.emplace_back("dnsseed.ocvcoin.tr.");
vSeeds.emplace_back("dnsseed.ocvcoin.swiss.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.vc.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.id.");
vSeeds.emplace_back("dnsseed.ocvcoin.rio.");
vSeeds.emplace_back("dnsseed.ocvcoin.cg.");
vSeeds.emplace_back("dnsseed.ocvcoin.forex.");
vSeeds.emplace_back("dnsseed.ocvcoin.food.");
vSeeds.emplace_back("dnsseed.ocvcoin.ren.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.nz.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.pt.");
vSeeds.emplace_back("dnsseed.ocvcoin.il.");
vSeeds.emplace_back("dnsseed.ocvcoin.mw.");
vSeeds.emplace_back("dnsseed.ocvcoin.nc.");
vSeeds.emplace_back("dnsseed.ocvcoin.tirol.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.okinawa.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.nz.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.af.");
vSeeds.emplace_back("dnsseed.ocvcoin.info.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.foo.");
vSeeds.emplace_back("dnsseed.ocvcoin.museum.");
vSeeds.emplace_back("dnsseed.ocvcoin.gen.in.");
vSeeds.emplace_back("dnsseed.ocvcoin.alsace.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.hk.");
vSeeds.emplace_back("dnsseed.ocvcoin.mv.");
vSeeds.emplace_back("dnsseed.ocvcoin.sd.");
vSeeds.emplace_back("dnsseed.ocvcoin.trust.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.int.");
vSeeds.emplace_back("dnsseed.ocvcoin.joburg.");
vSeeds.emplace_back("dnsseed.ocvcoin.makeup.");
vSeeds.emplace_back("dnsseed.ocvcoin.hm.");
vSeeds.emplace_back("dnsseed.ocvcoin.firm.in.");
vSeeds.emplace_back("dnsseed.ocvcoin.music.");
vSeeds.emplace_back("dnsseed.ocvcoin.yachts.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.es.");
vSeeds.emplace_back("dnsseed.ocvcoin.gent.");
vSeeds.emplace_back("dnsseed.ocvcoin.data.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.py.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.ru.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ni.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.bz.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.sa.");
vSeeds.emplace_back("dnsseed.ocvcoin.ind.in.");
vSeeds.emplace_back("dnsseed.ocvcoin.beauty.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.th.");
vSeeds.emplace_back("dnsseed.ocvcoin.motorcycles.");
vSeeds.emplace_back("dnsseed.ocvcoin.gw.");
vSeeds.emplace_back("dnsseed.ocvcoin.bw.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.cr.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.insurance.");
vSeeds.emplace_back("dnsseed.ocvcoin.basketball.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.pn.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.pe.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.abc.");
vSeeds.emplace_back("dnsseed.ocvcoin.bm.");
vSeeds.emplace_back("dnsseed.ocvcoin.dad.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.sc.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ph.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ec.");
vSeeds.emplace_back("dnsseed.ocvcoin.istanbul.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.jp.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.cm.");
vSeeds.emplace_back("dnsseed.ocvcoin.frl.");
vSeeds.emplace_back("dnsseed.ocvcoin.nr.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.ke.");
vSeeds.emplace_back("dnsseed.ocvcoin.in.th.");
vSeeds.emplace_back("dnsseed.ocvcoin.whoswho.");
vSeeds.emplace_back("dnsseed.ocvcoin.free.");
vSeeds.emplace_back("dnsseed.ocvcoin.web.tr.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.ru.");
vSeeds.emplace_back("dnsseed.ocvcoin.versicherung.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.sb.");
vSeeds.emplace_back("dnsseed.ocvcoin.bt.");
vSeeds.emplace_back("dnsseed.ocvcoin.you.");
vSeeds.emplace_back("dnsseed.ocvcoin.taipei.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ng.");
vSeeds.emplace_back("dnsseed.ocvcoin.moscow.");
vSeeds.emplace_back("dnsseed.ocvcoin.biz.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.nl.");
vSeeds.emplace_back("dnsseed.ocvcoin.spa.");
vSeeds.emplace_back("dnsseed.ocvcoin.cw.");
vSeeds.emplace_back("dnsseed.ocvcoin.book.");
vSeeds.emplace_back("dnsseed.ocvcoin.gh.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.kr.");
vSeeds.emplace_back("dnsseed.ocvcoin.lotto.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.je.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.ph.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.za.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.pr.");
vSeeds.emplace_back("dnsseed.ocvcoin.nexus.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ve.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.hr.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.cy.");
vSeeds.emplace_back("dnsseed.ocvcoin.id.au.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.za.");
vSeeds.emplace_back("dnsseed.ocvcoin.kids.");
vSeeds.emplace_back("dnsseed.ocvcoin.gn.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.il.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.gg.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ro.");
vSeeds.emplace_back("dnsseed.ocvcoin.mq.");
vSeeds.emplace_back("dnsseed.ocvcoin.dealer.");
vSeeds.emplace_back("dnsseed.ocvcoin.skin.");
vSeeds.emplace_back("dnsseed.ocvcoin.pf.");
vSeeds.emplace_back("dnsseed.ocvcoin.kyoto.");
vSeeds.emplace_back("dnsseed.ocvcoin.google.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.gy.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.my.");
vSeeds.emplace_back("dnsseed.ocvcoin.idv.tw.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.do.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.gt.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.pa.");
vSeeds.emplace_back("dnsseed.ocvcoin.phd.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ai.");
vSeeds.emplace_back("dnsseed.ocvcoin.corsica.");
vSeeds.emplace_back("dnsseed.ocvcoin.meet.");
vSeeds.emplace_back("dnsseed.ocvcoin.gov.");
vSeeds.emplace_back("dnsseed.ocvcoin.reit.");
vSeeds.emplace_back("dnsseed.ocvcoin.krd.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.ve.");
vSeeds.emplace_back("dnsseed.ocvcoin.madrid.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.au.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.lc.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.mx.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.im.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.rs.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.ph.");
vSeeds.emplace_back("dnsseed.ocvcoin.mov.");
vSeeds.emplace_back("dnsseed.ocvcoin.ltd.uk.");
vSeeds.emplace_back("dnsseed.ocvcoin.case.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.pk.");
vSeeds.emplace_back("dnsseed.ocvcoin.rsvp.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.au.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.uy.");
vSeeds.emplace_back("dnsseed.ocvcoin.man.");
vSeeds.emplace_back("dnsseed.ocvcoin.play.");
vSeeds.emplace_back("dnsseed.ocvcoin.dot.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.pe.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ly.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.pe.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.gh.");
vSeeds.emplace_back("dnsseed.ocvcoin.pay.");
vSeeds.emplace_back("dnsseed.ocvcoin.hair.");
vSeeds.emplace_back("dnsseed.ocvcoin.moi.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.mt.");
vSeeds.emplace_back("dnsseed.ocvcoin.biz.id.");
vSeeds.emplace_back("dnsseed.ocvcoin.kiwi.nz.");
vSeeds.emplace_back("dnsseed.ocvcoin.aarp.");
vSeeds.emplace_back("dnsseed.ocvcoin.edu.rs.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.so.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.sv.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.gr.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ht.");
vSeeds.emplace_back("dnsseed.ocvcoin.xin.");
vSeeds.emplace_back("dnsseed.ocvcoin.med.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.hu.");
vSeeds.emplace_back("dnsseed.ocvcoin.zuerich.");
vSeeds.emplace_back("dnsseed.ocvcoin.giving.");
vSeeds.emplace_back("dnsseed.ocvcoin.gop.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.ao.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.bi.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ee.");
vSeeds.emplace_back("dnsseed.ocvcoin.apple.");
vSeeds.emplace_back("dnsseed.ocvcoin.gen.tr.");
vSeeds.emplace_back("dnsseed.ocvcoin.pharmacy.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.lv.");
vSeeds.emplace_back("dnsseed.ocvcoin.wow.");
vSeeds.emplace_back("dnsseed.ocvcoin.rugby.");
vSeeds.emplace_back("dnsseed.ocvcoin.msk.ru.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.tw.");
vSeeds.emplace_back("dnsseed.ocvcoin.zero.");
vSeeds.emplace_back("dnsseed.ocvcoin.biz.tr.");
vSeeds.emplace_back("dnsseed.ocvcoin.va.");
vSeeds.emplace_back("dnsseed.ocvcoin.select.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.vn.");
vSeeds.emplace_back("dnsseed.ocvcoin.prof.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.tt.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.so.");
vSeeds.emplace_back("dnsseed.ocvcoin.waw.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.pk.");
vSeeds.emplace_back("dnsseed.ocvcoin.mz.");
vSeeds.emplace_back("dnsseed.ocvcoin.here.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.my.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.vn.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.bh.");
vSeeds.emplace_back("dnsseed.ocvcoin.gu.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.ug.");
vSeeds.emplace_back("dnsseed.ocvcoin.web.id.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.tz.");
vSeeds.emplace_back("dnsseed.ocvcoin.or.kr.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.nf.");
vSeeds.emplace_back("dnsseed.ocvcoin.esq.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ms.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.by.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.tn.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.pk.");
vSeeds.emplace_back("dnsseed.ocvcoin.bn.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.my.");
vSeeds.emplace_back("dnsseed.ocvcoin.able.");
vSeeds.emplace_back("dnsseed.ocvcoin.ads.");
vSeeds.emplace_back("dnsseed.ocvcoin.eat.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.mk.");
vSeeds.emplace_back("dnsseed.ocvcoin.tv.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.info.tr.");
vSeeds.emplace_back("dnsseed.ocvcoin.in.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.ac.nz.");
vSeeds.emplace_back("dnsseed.ocvcoin.kiev.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.ice.");
vSeeds.emplace_back("dnsseed.ocvcoin.cpa.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.eg.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.ma.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.lb.");
vSeeds.emplace_back("dnsseed.ocvcoin.spb.ru.");
vSeeds.emplace_back("dnsseed.ocvcoin.map.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.kz.");
vSeeds.emplace_back("dnsseed.ocvcoin.amazon.");
vSeeds.emplace_back("dnsseed.ocvcoin.gen.nz.");
vSeeds.emplace_back("dnsseed.ocvcoin.geek.nz.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.fj.");
vSeeds.emplace_back("dnsseed.ocvcoin.watches.");
vSeeds.emplace_back("dnsseed.ocvcoin.km.");
vSeeds.emplace_back("dnsseed.ocvcoin.edu.vn.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.mz.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.kw.");
vSeeds.emplace_back("dnsseed.ocvcoin.abbott.");
vSeeds.emplace_back("dnsseed.ocvcoin.diy.");
vSeeds.emplace_back("dnsseed.ocvcoin.tatar.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.gp.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.na.");
vSeeds.emplace_back("dnsseed.ocvcoin.fox.");
vSeeds.emplace_back("dnsseed.ocvcoin.search.");
vSeeds.emplace_back("dnsseed.ocvcoin.ind.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.arpa.");
vSeeds.emplace_back("dnsseed.ocvcoin.open.");
vSeeds.emplace_back("dnsseed.ocvcoin.fly.");
vSeeds.emplace_back("dnsseed.ocvcoin.eco.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.living.");
vSeeds.emplace_back("dnsseed.ocvcoin.lifestyle.");
vSeeds.emplace_back("dnsseed.ocvcoin.secure.");
vSeeds.emplace_back("dnsseed.ocvcoin.abb.");
vSeeds.emplace_back("dnsseed.ocvcoin.zm.");
vSeeds.emplace_back("dnsseed.ocvcoin.sky.");
vSeeds.emplace_back("dnsseed.ocvcoin.docs.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.vi.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.ar.");
vSeeds.emplace_back("dnsseed.ocvcoin.idv.hk.");
vSeeds.emplace_back("dnsseed.ocvcoin.ne.jp.");
vSeeds.emplace_back("dnsseed.ocvcoin.youtube.");
vSeeds.emplace_back("dnsseed.ocvcoin.goo.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.jo.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.bw.");
vSeeds.emplace_back("dnsseed.ocvcoin.post.");
vSeeds.emplace_back("dnsseed.ocvcoin.star.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.ls.");
vSeeds.emplace_back("dnsseed.ocvcoin.school.nz.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ge.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.lk.");
vSeeds.emplace_back("dnsseed.ocvcoin.lr.");
vSeeds.emplace_back("dnsseed.ocvcoin.aws.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.gi.");
vSeeds.emplace_back("dnsseed.ocvcoin.name.tr.");
vSeeds.emplace_back("dnsseed.ocvcoin.zw.");
vSeeds.emplace_back("dnsseed.ocvcoin.creditunion.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.zw.");
vSeeds.emplace_back("dnsseed.ocvcoin.sh.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.next.");
vSeeds.emplace_back("dnsseed.ocvcoin.kp.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.tj.");
vSeeds.emplace_back("dnsseed.ocvcoin.fj.");
vSeeds.emplace_back("dnsseed.ocvcoin.ping.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ci.");
vSeeds.emplace_back("dnsseed.ocvcoin.analytics.");
vSeeds.emplace_back("dnsseed.ocvcoin.lb.");
vSeeds.emplace_back("dnsseed.ocvcoin.tvs.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.zm.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.sl.");
vSeeds.emplace_back("dnsseed.ocvcoin.buy.");
vSeeds.emplace_back("dnsseed.ocvcoin.vana.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.lk.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.hk.");
vSeeds.emplace_back("dnsseed.ocvcoin.abbvie.");
vSeeds.emplace_back("dnsseed.ocvcoin.drive.");
vSeeds.emplace_back("dnsseed.ocvcoin.hot.");
vSeeds.emplace_back("dnsseed.ocvcoin.hk.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.tw.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.talk.");
vSeeds.emplace_back("dnsseed.ocvcoin.or.jp.");
vSeeds.emplace_back("dnsseed.ocvcoin.mobile.");
vSeeds.emplace_back("dnsseed.ocvcoin.ks.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.room.");
vSeeds.emplace_back("dnsseed.ocvcoin.pro.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.fast.");
vSeeds.emplace_back("dnsseed.ocvcoin.wiki.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.sz.");
vSeeds.emplace_back("dnsseed.ocvcoin.java.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.tr.");
vSeeds.emplace_back("dnsseed.ocvcoin.office.");
vSeeds.emplace_back("dnsseed.ocvcoin.art.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.abudhabi.");
vSeeds.emplace_back("dnsseed.ocvcoin.bentley.");
vSeeds.emplace_back("dnsseed.ocvcoin.cal.");
vSeeds.emplace_back("dnsseed.ocvcoin.smart.");
vSeeds.emplace_back("dnsseed.ocvcoin.gmail.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.et.");
vSeeds.emplace_back("dnsseed.ocvcoin.bj.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.bbc.");
vSeeds.emplace_back("dnsseed.ocvcoin.hi.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.ott.");
vSeeds.emplace_back("dnsseed.ocvcoin.prod.");
vSeeds.emplace_back("dnsseed.ocvcoin.android.");
vSeeds.emplace_back("dnsseed.ocvcoin.gle.");
vSeeds.emplace_back("dnsseed.ocvcoin.ntt.");
vSeeds.emplace_back("dnsseed.ocvcoin.kw.");
vSeeds.emplace_back("dnsseed.ocvcoin.gb.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.qa.");
vSeeds.emplace_back("dnsseed.ocvcoin.joy.");
vSeeds.emplace_back("dnsseed.ocvcoin.re.kr.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.ng.");
vSeeds.emplace_back("dnsseed.ocvcoin.ally.");
vSeeds.emplace_back("dnsseed.ocvcoin.lds.");
vSeeds.emplace_back("dnsseed.ocvcoin.safe.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.ai.");
vSeeds.emplace_back("dnsseed.ocvcoin.fire.");
vSeeds.emplace_back("dnsseed.ocvcoin.org.gr.");
vSeeds.emplace_back("dnsseed.ocvcoin.mil.");
vSeeds.emplace_back("dnsseed.ocvcoin.azure.");
vSeeds.emplace_back("dnsseed.ocvcoin.catholic.");
vSeeds.emplace_back("dnsseed.ocvcoin.ups.");
vSeeds.emplace_back("dnsseed.ocvcoin.nieruchomosci.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.read.");
vSeeds.emplace_back("dnsseed.ocvcoin.netflix.");
vSeeds.emplace_back("dnsseed.ocvcoin.deal.");
vSeeds.emplace_back("dnsseed.ocvcoin.hotels.");
vSeeds.emplace_back("dnsseed.ocvcoin.anz.");
vSeeds.emplace_back("dnsseed.ocvcoin.pioneer.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.ai.");
vSeeds.emplace_back("dnsseed.ocvcoin.warszawa.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.if.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.spot.");
vSeeds.emplace_back("dnsseed.ocvcoin.accenture.");
vSeeds.emplace_back("dnsseed.ocvcoin.alibaba.");
vSeeds.emplace_back("dnsseed.ocvcoin.americanexpress.");
vSeeds.emplace_back("dnsseed.ocvcoin.circle.");
vSeeds.emplace_back("dnsseed.ocvcoin.cv.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.gmo.");
vSeeds.emplace_back("dnsseed.ocvcoin.pe.kr.");
vSeeds.emplace_back("dnsseed.ocvcoin.off.ai.");
vSeeds.emplace_back("dnsseed.ocvcoin.arte.");
vSeeds.emplace_back("dnsseed.ocvcoin.arab.");
vSeeds.emplace_back("dnsseed.ocvcoin.nowruz.");
vSeeds.emplace_back("dnsseed.ocvcoin.ieee.");
vSeeds.emplace_back("dnsseed.ocvcoin.sklep.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.realtor.");
vSeeds.emplace_back("dnsseed.ocvcoin.bv.");
vSeeds.emplace_back("dnsseed.ocvcoin.phone.");
vSeeds.emplace_back("dnsseed.ocvcoin.call.");
vSeeds.emplace_back("dnsseed.ocvcoin.wroclaw.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.mh.");
vSeeds.emplace_back("dnsseed.ocvcoin.shell.");
vSeeds.emplace_back("dnsseed.ocvcoin.sn.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.ps.");
vSeeds.emplace_back("dnsseed.ocvcoin.xbox.");
vSeeds.emplace_back("dnsseed.ocvcoin.barefoot.");
vSeeds.emplace_back("dnsseed.ocvcoin.fj.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.com.nr.");
vSeeds.emplace_back("dnsseed.ocvcoin.george.");
vSeeds.emplace_back("dnsseed.ocvcoin.goog.");
vSeeds.emplace_back("dnsseed.ocvcoin.audi.");
vSeeds.emplace_back("dnsseed.ocvcoin.virgin.");
vSeeds.emplace_back("dnsseed.ocvcoin.nm.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.microsoft.");
vSeeds.emplace_back("dnsseed.ocvcoin.gx.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.edu.lk.");
vSeeds.emplace_back("dnsseed.ocvcoin.booking.");
vSeeds.emplace_back("dnsseed.ocvcoin.airbus.");
vSeeds.emplace_back("dnsseed.ocvcoin.mango.");
vSeeds.emplace_back("dnsseed.ocvcoin.mo.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.aco.");
vSeeds.emplace_back("dnsseed.ocvcoin.bmw.");
vSeeds.emplace_back("dnsseed.ocvcoin.song.");
vSeeds.emplace_back("dnsseed.ocvcoin.sx.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.gz.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.aol.");
vSeeds.emplace_back("dnsseed.ocvcoin.rec.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.like.");
vSeeds.emplace_back("dnsseed.ocvcoin.redstone.");
vSeeds.emplace_back("dnsseed.ocvcoin.wed.");
vSeeds.emplace_back("dnsseed.ocvcoin.bestbuy.");
vSeeds.emplace_back("dnsseed.ocvcoin.yandex.");
vSeeds.emplace_back("dnsseed.ocvcoin.hn.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.gd.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.gs.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.js.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.yn.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.obi.");
vSeeds.emplace_back("dnsseed.ocvcoin.srv.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.dubai.");
vSeeds.emplace_back("dnsseed.ocvcoin.omega.");
vSeeds.emplace_back("dnsseed.ocvcoin.save.");
vSeeds.emplace_back("dnsseed.ocvcoin.hl.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.esp.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.author.");
vSeeds.emplace_back("dnsseed.ocvcoin.tab.");
vSeeds.emplace_back("dnsseed.ocvcoin.godaddy.");
vSeeds.emplace_back("dnsseed.ocvcoin.mini.");
vSeeds.emplace_back("dnsseed.ocvcoin.windows.");
vSeeds.emplace_back("dnsseed.ocvcoin.yun.");
vSeeds.emplace_back("dnsseed.ocvcoin.nico.");
vSeeds.emplace_back("dnsseed.ocvcoin.mint.");
vSeeds.emplace_back("dnsseed.ocvcoin.or.id.");
vSeeds.emplace_back("dnsseed.ocvcoin.dds.");
vSeeds.emplace_back("dnsseed.ocvcoin.beats.");
vSeeds.emplace_back("dnsseed.ocvcoin.ggee.");
vSeeds.emplace_back("dnsseed.ocvcoin.cq.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.gucci.");
vSeeds.emplace_back("dnsseed.ocvcoin.bofa.");
vSeeds.emplace_back("dnsseed.ocvcoin.smile.");
vSeeds.emplace_back("dnsseed.ocvcoin.sj.");
vSeeds.emplace_back("dnsseed.ocvcoin.prime.");
vSeeds.emplace_back("dnsseed.ocvcoin.amex.");
vSeeds.emplace_back("dnsseed.ocvcoin.imb.br.");
vSeeds.emplace_back("dnsseed.ocvcoin.ollo.");
vSeeds.emplace_back("dnsseed.ocvcoin.orange.");
vSeeds.emplace_back("dnsseed.ocvcoin.aetna.");
vSeeds.emplace_back("dnsseed.ocvcoin.got.");
vSeeds.emplace_back("dnsseed.ocvcoin.pid.");
vSeeds.emplace_back("dnsseed.ocvcoin.walmart.");
vSeeds.emplace_back("dnsseed.ocvcoin.ac.cn.");
vSeeds.emplace_back("dnsseed.ocvcoin.seven.");
vSeeds.emplace_back("dnsseed.ocvcoin.pin.");
vSeeds.emplace_back("dnsseed.ocvcoin.comsec.");
vSeeds.emplace_back("dnsseed.ocvcoin.farmers.");
vSeeds.emplace_back("dnsseed.ocvcoin.tokyo.jp.");
vSeeds.emplace_back("dnsseed.ocvcoin.afl.");
vSeeds.emplace_back("dnsseed.ocvcoin.calvinklein.");
vSeeds.emplace_back("dnsseed.ocvcoin.kosher.");
vSeeds.emplace_back("dnsseed.ocvcoin.asda.");
vSeeds.emplace_back("dnsseed.ocvcoin.eurovision.");
vSeeds.emplace_back("dnsseed.ocvcoin.bauhaus.");
vSeeds.emplace_back("dnsseed.ocvcoin.discover.");
vSeeds.emplace_back("dnsseed.ocvcoin.chrome.");
vSeeds.emplace_back("dnsseed.ocvcoin.wtc.");
vSeeds.emplace_back("dnsseed.ocvcoin.brother.");
vSeeds.emplace_back("dnsseed.ocvcoin.locker.");
vSeeds.emplace_back("dnsseed.ocvcoin.moto.");
vSeeds.emplace_back("dnsseed.ocvcoin.name.my.");
vSeeds.emplace_back("dnsseed.ocvcoin.bom.");
vSeeds.emplace_back("dnsseed.ocvcoin.lego.");
vSeeds.emplace_back("dnsseed.ocvcoin.axa.");
vSeeds.emplace_back("dnsseed.ocvcoin.cern.");
vSeeds.emplace_back("dnsseed.ocvcoin.ibm.");
vSeeds.emplace_back("dnsseed.ocvcoin.tui.");
vSeeds.emplace_back("dnsseed.ocvcoin.uol.");
vSeeds.emplace_back("dnsseed.ocvcoin.safety.");
vSeeds.emplace_back("dnsseed.ocvcoin.alipay.");
vSeeds.emplace_back("dnsseed.ocvcoin.kred.");
vSeeds.emplace_back("dnsseed.ocvcoin.williamhill.");
vSeeds.emplace_back("dnsseed.ocvcoin.tkmaxx.");
vSeeds.emplace_back("dnsseed.ocvcoin.lidl.");
vSeeds.emplace_back("dnsseed.ocvcoin.weather.");
vSeeds.emplace_back("dnsseed.ocvcoin.yahoo.");
vSeeds.emplace_back("dnsseed.ocvcoin.bradesco.");
vSeeds.emplace_back("dnsseed.ocvcoin.seek.");
vSeeds.emplace_back("dnsseed.ocvcoin.mit.");
vSeeds.emplace_back("dnsseed.ocvcoin.skype.");
vSeeds.emplace_back("dnsseed.ocvcoin.dhl.");
vSeeds.emplace_back("dnsseed.ocvcoin.agakhan.");
vSeeds.emplace_back("dnsseed.ocvcoin.zara.");
vSeeds.emplace_back("dnsseed.ocvcoin.americanfamily.");
vSeeds.emplace_back("dnsseed.ocvcoin.bloomberg.");
vSeeds.emplace_back("dnsseed.ocvcoin.playstation.");
vSeeds.emplace_back("dnsseed.ocvcoin.canon.");
vSeeds.emplace_back("dnsseed.ocvcoin.ftr.");
vSeeds.emplace_back("dnsseed.ocvcoin.jio.");
vSeeds.emplace_back("dnsseed.ocvcoin.viking.");
vSeeds.emplace_back("dnsseed.ocvcoin.baseball.");
vSeeds.emplace_back("dnsseed.ocvcoin.blockbuster.");
vSeeds.emplace_back("dnsseed.ocvcoin.bnpparibas.");
vSeeds.emplace_back("dnsseed.ocvcoin.aeg.");
vSeeds.emplace_back("dnsseed.ocvcoin.hangout.");
vSeeds.emplace_back("dnsseed.ocvcoin.sbi.");
vSeeds.emplace_back("dnsseed.ocvcoin.schwarz.");
vSeeds.emplace_back("dnsseed.ocvcoin.vanguard.");
vSeeds.emplace_back("dnsseed.ocvcoin.visa.");
vSeeds.emplace_back("dnsseed.ocvcoin.edeka.");
vSeeds.emplace_back("dnsseed.ocvcoin.lamborghini.");
vSeeds.emplace_back("dnsseed.ocvcoin.mma.");
vSeeds.emplace_back("dnsseed.ocvcoin.sakura.");
vSeeds.emplace_back("dnsseed.ocvcoin.globo.");
vSeeds.emplace_back("dnsseed.ocvcoin.sas.");
vSeeds.emplace_back("dnsseed.ocvcoin.aig.");
vSeeds.emplace_back("dnsseed.ocvcoin.llp.");
vSeeds.emplace_back("dnsseed.ocvcoin.airtel.");
vSeeds.emplace_back("dnsseed.ocvcoin.baidu.");
vSeeds.emplace_back("dnsseed.ocvcoin.crown.");
vSeeds.emplace_back("dnsseed.ocvcoin.ferrari.");
vSeeds.emplace_back("dnsseed.ocvcoin.travelersinsurance.");
vSeeds.emplace_back("dnsseed.ocvcoin.tunes.");
vSeeds.emplace_back("dnsseed.ocvcoin.extraspace.");
vSeeds.emplace_back("dnsseed.ocvcoin.hotmail.");
vSeeds.emplace_back("dnsseed.ocvcoin.coupon.");
vSeeds.emplace_back("dnsseed.ocvcoin.barclays.");
vSeeds.emplace_back("dnsseed.ocvcoin.delta.");
vSeeds.emplace_back("dnsseed.ocvcoin.bing.");
vSeeds.emplace_back("dnsseed.ocvcoin.cruise.");
vSeeds.emplace_back("dnsseed.ocvcoin.aquarelle.");
vSeeds.emplace_back("dnsseed.ocvcoin.gmx.");
vSeeds.emplace_back("dnsseed.ocvcoin.seat.");
vSeeds.emplace_back("dnsseed.ocvcoin.ubs.");
vSeeds.emplace_back("dnsseed.ocvcoin.sap.");
vSeeds.emplace_back("dnsseed.ocvcoin.latino.");
vSeeds.emplace_back("dnsseed.ocvcoin.viva.");
vSeeds.emplace_back("dnsseed.ocvcoin.bcn.");
vSeeds.emplace_back("dnsseed.ocvcoin.final.");
vSeeds.emplace_back("dnsseed.ocvcoin.unicom.");
vSeeds.emplace_back("dnsseed.ocvcoin.samsung.");
vSeeds.emplace_back("dnsseed.ocvcoin.origins.");
vSeeds.emplace_back("dnsseed.ocvcoin.erni.");
vSeeds.emplace_back("dnsseed.ocvcoin.pwc.");
vSeeds.emplace_back("dnsseed.ocvcoin.ericsson.");
vSeeds.emplace_back("dnsseed.ocvcoin.mormon.");
vSeeds.emplace_back("dnsseed.ocvcoin.weber.");
vSeeds.emplace_back("dnsseed.ocvcoin.citadel.");
vSeeds.emplace_back("dnsseed.ocvcoin.deloitte.");
vSeeds.emplace_back("dnsseed.ocvcoin.cba.");
vSeeds.emplace_back("dnsseed.ocvcoin.total.");
vSeeds.emplace_back("dnsseed.ocvcoin.imdb.");
vSeeds.emplace_back("dnsseed.ocvcoin.auspost.");
vSeeds.emplace_back("dnsseed.ocvcoin.sony.");
vSeeds.emplace_back("dnsseed.ocvcoin.chanel.");
vSeeds.emplace_back("dnsseed.ocvcoin.oracle.");
vSeeds.emplace_back("dnsseed.ocvcoin.hermes.");
vSeeds.emplace_back("dnsseed.ocvcoin.homedepot.");
vSeeds.emplace_back("dnsseed.ocvcoin.infiniti.");
vSeeds.emplace_back("dnsseed.ocvcoin.datsun.");
vSeeds.emplace_back("dnsseed.ocvcoin.gap.");
vSeeds.emplace_back("dnsseed.ocvcoin.itv.");
vSeeds.emplace_back("dnsseed.ocvcoin.natura.");
vSeeds.emplace_back("dnsseed.ocvcoin.toyota.");
vSeeds.emplace_back("dnsseed.ocvcoin.audible.");
vSeeds.emplace_back("dnsseed.ocvcoin.cisco.");
vSeeds.emplace_back("dnsseed.ocvcoin.sncf.");
vSeeds.emplace_back("dnsseed.ocvcoin.cbn.");
vSeeds.emplace_back("dnsseed.ocvcoin.fage.");
vSeeds.emplace_back("dnsseed.ocvcoin.leclerc.");
vSeeds.emplace_back("dnsseed.ocvcoin.pfizer.");
vSeeds.emplace_back("dnsseed.ocvcoin.stockholm.");
vSeeds.emplace_back("dnsseed.ocvcoin.winners.");
vSeeds.emplace_back("dnsseed.ocvcoin.jpmorgan.");
vSeeds.emplace_back("dnsseed.ocvcoin.kpmg.");
vSeeds.emplace_back("dnsseed.ocvcoin.schmidt.");
vSeeds.emplace_back("dnsseed.ocvcoin.akdn.");
vSeeds.emplace_back("dnsseed.ocvcoin.helsinki.");
vSeeds.emplace_back("dnsseed.ocvcoin.jmp.");
vSeeds.emplace_back("dnsseed.ocvcoin.crs.");
vSeeds.emplace_back("dnsseed.ocvcoin.weibo.");
vSeeds.emplace_back("dnsseed.ocvcoin.bharti.");
vSeeds.emplace_back("dnsseed.ocvcoin.sharp.");
vSeeds.emplace_back("dnsseed.ocvcoin.nike.");
vSeeds.emplace_back("dnsseed.ocvcoin.vivo.");
vSeeds.emplace_back("dnsseed.ocvcoin.walter.");
vSeeds.emplace_back("dnsseed.ocvcoin.weir.");
vSeeds.emplace_back("dnsseed.ocvcoin.itau.");
vSeeds.emplace_back("dnsseed.ocvcoin.allstate.");
vSeeds.emplace_back("dnsseed.ocvcoin.aramco.");
vSeeds.emplace_back("dnsseed.ocvcoin.broadway.");
vSeeds.emplace_back("dnsseed.ocvcoin.bridgestone.");
vSeeds.emplace_back("dnsseed.ocvcoin.hsbc.");
vSeeds.emplace_back("dnsseed.ocvcoin.travelers.");
vSeeds.emplace_back("dnsseed.ocvcoin.capitalone.");
vSeeds.emplace_back("dnsseed.ocvcoin.lamer.");
vSeeds.emplace_back("dnsseed.ocvcoin.ipiranga.");
vSeeds.emplace_back("dnsseed.ocvcoin.honda.");
vSeeds.emplace_back("dnsseed.ocvcoin.lilly.");
vSeeds.emplace_back("dnsseed.ocvcoin.ikano.");
vSeeds.emplace_back("dnsseed.ocvcoin.ismaili.");
vSeeds.emplace_back("dnsseed.ocvcoin.nokia.");
vSeeds.emplace_back("dnsseed.ocvcoin.yodobashi.");
vSeeds.emplace_back("dnsseed.ocvcoin.jaguar.");
vSeeds.emplace_back("dnsseed.ocvcoin.bosch.");
vSeeds.emplace_back("dnsseed.ocvcoin.dell.");
vSeeds.emplace_back("dnsseed.ocvcoin.target.");
vSeeds.emplace_back("dnsseed.ocvcoin.xerox.");
vSeeds.emplace_back("dnsseed.ocvcoin.barclaycard.");
vSeeds.emplace_back("dnsseed.ocvcoin.bms.");
vSeeds.emplace_back("dnsseed.ocvcoin.chase.");
vSeeds.emplace_back("dnsseed.ocvcoin.epson.");
vSeeds.emplace_back("dnsseed.ocvcoin.saxo.");
vSeeds.emplace_back("dnsseed.ocvcoin.shia.");
vSeeds.emplace_back("dnsseed.ocvcoin.nra.");
vSeeds.emplace_back("dnsseed.ocvcoin.dnp.");
vSeeds.emplace_back("dnsseed.ocvcoin.fido.");
vSeeds.emplace_back("dnsseed.ocvcoin.tdk.");
vSeeds.emplace_back("dnsseed.ocvcoin.dish.");
vSeeds.emplace_back("dnsseed.ocvcoin.frontier.");
vSeeds.emplace_back("dnsseed.ocvcoin.icbc.");
vSeeds.emplace_back("dnsseed.ocvcoin.kia.");
vSeeds.emplace_back("dnsseed.ocvcoin.politie.");
vSeeds.emplace_back("dnsseed.ocvcoin.sew.");
vSeeds.emplace_back("dnsseed.ocvcoin.jeep.");
vSeeds.emplace_back("dnsseed.ocvcoin.ril.");
vSeeds.emplace_back("dnsseed.ocvcoin.zappos.");
vSeeds.emplace_back("dnsseed.ocvcoin.cuisinella.");
vSeeds.emplace_back("dnsseed.ocvcoin.dvr.");
vSeeds.emplace_back("dnsseed.ocvcoin.sfr.");
vSeeds.emplace_back("dnsseed.ocvcoin.stc.");
vSeeds.emplace_back("dnsseed.ocvcoin.anquan.");
vSeeds.emplace_back("dnsseed.ocvcoin.caravan.");
vSeeds.emplace_back("dnsseed.ocvcoin.juniper.");
vSeeds.emplace_back("dnsseed.ocvcoin.lifeinsurance.");
vSeeds.emplace_back("dnsseed.ocvcoin.monash.");
vSeeds.emplace_back("dnsseed.ocvcoin.neustar.");
vSeeds.emplace_back("dnsseed.ocvcoin.clubmed.");
vSeeds.emplace_back("dnsseed.ocvcoin.dabur.");
vSeeds.emplace_back("dnsseed.ocvcoin.frogans.");
vSeeds.emplace_back("dnsseed.ocvcoin.kpn.");
vSeeds.emplace_back("dnsseed.ocvcoin.lotte.");
vSeeds.emplace_back("dnsseed.ocvcoin.netbank.");
vSeeds.emplace_back("dnsseed.ocvcoin.gea.");
vSeeds.emplace_back("dnsseed.ocvcoin.wme.");
vSeeds.emplace_back("dnsseed.ocvcoin.hitachi.");
vSeeds.emplace_back("dnsseed.ocvcoin.hyundai.");
vSeeds.emplace_back("dnsseed.ocvcoin.praxi.");
vSeeds.emplace_back("dnsseed.ocvcoin.weatherchannel.");
vSeeds.emplace_back("dnsseed.ocvcoin.amica.");
vSeeds.emplace_back("dnsseed.ocvcoin.commbank.");
vSeeds.emplace_back("dnsseed.ocvcoin.otsuka.");
vSeeds.emplace_back("dnsseed.ocvcoin.tjmaxx.");
vSeeds.emplace_back("dnsseed.ocvcoin.woodside.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.tw.");
vSeeds.emplace_back("dnsseed.ocvcoin.mls.");
vSeeds.emplace_back("dnsseed.ocvcoin.mtn.");
vSeeds.emplace_back("dnsseed.ocvcoin.nissan.");
vSeeds.emplace_back("dnsseed.ocvcoin.shangrila.");
vSeeds.emplace_back("dnsseed.ocvcoin.trv.");
vSeeds.emplace_back("dnsseed.ocvcoin.volvo.");
vSeeds.emplace_back("dnsseed.ocvcoin.hughes.");
vSeeds.emplace_back("dnsseed.ocvcoin.kerryhotels.");
vSeeds.emplace_back("dnsseed.ocvcoin.marriott.");
vSeeds.emplace_back("dnsseed.ocvcoin.suzuki.");
vSeeds.emplace_back("dnsseed.ocvcoin.cipriani.");
vSeeds.emplace_back("dnsseed.ocvcoin.dvag.");
vSeeds.emplace_back("dnsseed.ocvcoin.ferrero.");
vSeeds.emplace_back("dnsseed.ocvcoin.samsclub.");
vSeeds.emplace_back("dnsseed.ocvcoin.statefarm.");
vSeeds.emplace_back("dnsseed.ocvcoin.thd.");
vSeeds.emplace_back("dnsseed.ocvcoin.maif.");
vSeeds.emplace_back("dnsseed.ocvcoin.nec.");
vSeeds.emplace_back("dnsseed.ocvcoin.dclk.");
vSeeds.emplace_back("dnsseed.ocvcoin.locus.");
vSeeds.emplace_back("dnsseed.ocvcoin.progressive.");
vSeeds.emplace_back("dnsseed.ocvcoin.lexus.");
vSeeds.emplace_back("dnsseed.ocvcoin.flickr.");
vSeeds.emplace_back("dnsseed.ocvcoin.imamat.");
vSeeds.emplace_back("dnsseed.ocvcoin.silk.");
vSeeds.emplace_back("dnsseed.ocvcoin.wanggou.");
vSeeds.emplace_back("dnsseed.ocvcoin.citi.");
vSeeds.emplace_back("dnsseed.ocvcoin.grocery.");
vSeeds.emplace_back("dnsseed.ocvcoin.lpl.");
vSeeds.emplace_back("dnsseed.ocvcoin.sandvikcoromant.");
vSeeds.emplace_back("dnsseed.ocvcoin.jot.");
vSeeds.emplace_back("dnsseed.ocvcoin.jprs.");
vSeeds.emplace_back("dnsseed.ocvcoin.sina.");
vSeeds.emplace_back("dnsseed.ocvcoin.athleta.");
vSeeds.emplace_back("dnsseed.ocvcoin.banamex.");
vSeeds.emplace_back("dnsseed.ocvcoin.fujitsu.");
vSeeds.emplace_back("dnsseed.ocvcoin.scb.");
vSeeds.emplace_back("dnsseed.ocvcoin.sohu.");
vSeeds.emplace_back("dnsseed.ocvcoin.toray.");
vSeeds.emplace_back("dnsseed.ocvcoin.fedex.");
vSeeds.emplace_back("dnsseed.ocvcoin.komatsu.");
vSeeds.emplace_back("dnsseed.ocvcoin.redumbrella.");
vSeeds.emplace_back("dnsseed.ocvcoin.tmall.");
vSeeds.emplace_back("dnsseed.ocvcoin.verisign.");
vSeeds.emplace_back("dnsseed.ocvcoin.dtv.");
vSeeds.emplace_back("dnsseed.ocvcoin.hkt.");
vSeeds.emplace_back("dnsseed.ocvcoin.taobao.");
vSeeds.emplace_back("dnsseed.ocvcoin.wolterskluwer.");
vSeeds.emplace_back("dnsseed.ocvcoin.ifm.");
vSeeds.emplace_back("dnsseed.ocvcoin.kddi.");
vSeeds.emplace_back("dnsseed.ocvcoin.lacaixa.");
vSeeds.emplace_back("dnsseed.ocvcoin.nab.");
vSeeds.emplace_back("dnsseed.ocvcoin.richardli.");
vSeeds.emplace_back("dnsseed.ocvcoin.lancaster.");
vSeeds.emplace_back("dnsseed.ocvcoin.landrover.");
vSeeds.emplace_back("dnsseed.ocvcoin.sling.");
vSeeds.emplace_back("dnsseed.ocvcoin.alstom.");
vSeeds.emplace_back("dnsseed.ocvcoin.kuokgroup.");
vSeeds.emplace_back("dnsseed.ocvcoin.pars.");
vSeeds.emplace_back("dnsseed.ocvcoin.pohl.");
vSeeds.emplace_back("dnsseed.ocvcoin.xihuan.");
vSeeds.emplace_back("dnsseed.ocvcoin.allfinanz.");
vSeeds.emplace_back("dnsseed.ocvcoin.panasonic.");
vSeeds.emplace_back("dnsseed.ocvcoin.bbva.");
vSeeds.emplace_back("dnsseed.ocvcoin.ford.");
vSeeds.emplace_back("dnsseed.ocvcoin.schaeffler.");
vSeeds.emplace_back("dnsseed.ocvcoin.softbank.");
vSeeds.emplace_back("dnsseed.ocvcoin.yamaxun.");
vSeeds.emplace_back("dnsseed.ocvcoin.hisamitsu.");
vSeeds.emplace_back("dnsseed.ocvcoin.boehringer.");
vSeeds.emplace_back("dnsseed.ocvcoin.kerryproperties.");
vSeeds.emplace_back("dnsseed.ocvcoin.nhk.");
vSeeds.emplace_back("dnsseed.ocvcoin.genting.");
vSeeds.emplace_back("dnsseed.ocvcoin.rwe.");
vSeeds.emplace_back("dnsseed.ocvcoin.scholarships.");
vSeeds.emplace_back("dnsseed.ocvcoin.fidelity.");
vSeeds.emplace_back("dnsseed.ocvcoin.fresenius.");
vSeeds.emplace_back("dnsseed.ocvcoin.jcb.");
vSeeds.emplace_back("dnsseed.ocvcoin.gallo.");
vSeeds.emplace_back("dnsseed.ocvcoin.goodyear.");
vSeeds.emplace_back("dnsseed.ocvcoin.nfl.");
vSeeds.emplace_back("dnsseed.ocvcoin.pnc.");
vSeeds.emplace_back("dnsseed.ocvcoin.cfa.");
vSeeds.emplace_back("dnsseed.ocvcoin.mckinsey.");
vSeeds.emplace_back("dnsseed.ocvcoin.shaw.");
vSeeds.emplace_back("dnsseed.ocvcoin.citic.");
vSeeds.emplace_back("dnsseed.ocvcoin.dupont.");
vSeeds.emplace_back("dnsseed.ocvcoin.fairwinds.");
vSeeds.emplace_back("dnsseed.ocvcoin.kindle.");
vSeeds.emplace_back("dnsseed.ocvcoin.mlb.");
vSeeds.emplace_back("dnsseed.ocvcoin.chintai.");
vSeeds.emplace_back("dnsseed.ocvcoin.emerck.");
vSeeds.emplace_back("dnsseed.ocvcoin.hbo.");
vSeeds.emplace_back("dnsseed.ocvcoin.pru.");
vSeeds.emplace_back("dnsseed.ocvcoin.vig.");
vSeeds.emplace_back("dnsseed.ocvcoin.msd.");
vSeeds.emplace_back("dnsseed.ocvcoin.pccw.");
vSeeds.emplace_back("dnsseed.ocvcoin.philips.");
vSeeds.emplace_back("dnsseed.ocvcoin.goldpoint.");
vSeeds.emplace_back("dnsseed.ocvcoin.jll.");
vSeeds.emplace_back("dnsseed.ocvcoin.lasalle.");
vSeeds.emplace_back("dnsseed.ocvcoin.tci.");
vSeeds.emplace_back("dnsseed.ocvcoin.toshiba.");
vSeeds.emplace_back("dnsseed.ocvcoin.ubank.");
vSeeds.emplace_back("dnsseed.ocvcoin.gbiz.");
vSeeds.emplace_back("dnsseed.ocvcoin.rogers.");
vSeeds.emplace_back("dnsseed.ocvcoin.dunlop.");
vSeeds.emplace_back("dnsseed.ocvcoin.hdfc.");
vSeeds.emplace_back("dnsseed.ocvcoin.cbre.");
vSeeds.emplace_back("dnsseed.ocvcoin.nba.");
vSeeds.emplace_back("dnsseed.ocvcoin.sanofi.");
vSeeds.emplace_back("dnsseed.ocvcoin.mtr.");
vSeeds.emplace_back("dnsseed.ocvcoin.tatamotors.");
vSeeds.emplace_back("dnsseed.ocvcoin.lincoln.");
vSeeds.emplace_back("dnsseed.ocvcoin.firestone.");
vSeeds.emplace_back("dnsseed.ocvcoin.statebank.");
vSeeds.emplace_back("dnsseed.ocvcoin.amfam.");
vSeeds.emplace_back("dnsseed.ocvcoin.mitsubishi.");
vSeeds.emplace_back("dnsseed.ocvcoin.shouji.");
vSeeds.emplace_back("dnsseed.ocvcoin.stada.");
vSeeds.emplace_back("dnsseed.ocvcoin.temasek.");
vSeeds.emplace_back("dnsseed.ocvcoin.rexroth.");
vSeeds.emplace_back("dnsseed.ocvcoin.norton.");
vSeeds.emplace_back("dnsseed.ocvcoin.sandvik.");
vSeeds.emplace_back("dnsseed.ocvcoin.bcg.");
vSeeds.emplace_back("dnsseed.ocvcoin.hyatt.");
vSeeds.emplace_back("dnsseed.ocvcoin.kfh.");
vSeeds.emplace_back("dnsseed.ocvcoin.olayan.");
vSeeds.emplace_back("dnsseed.ocvcoin.web.lk.");
vSeeds.emplace_back("dnsseed.ocvcoin.kerrylogistics.");
vSeeds.emplace_back("dnsseed.ocvcoin.pramerica.");
vSeeds.emplace_back("dnsseed.ocvcoin.sener.");
vSeeds.emplace_back("dnsseed.ocvcoin.firmdale.");
vSeeds.emplace_back("dnsseed.ocvcoin.nowtv.");
vSeeds.emplace_back("dnsseed.ocvcoin.reliance.");
vSeeds.emplace_back("dnsseed.ocvcoin.bbt.");
vSeeds.emplace_back("dnsseed.ocvcoin.lipsy.");
vSeeds.emplace_back("dnsseed.ocvcoin.nikon.");
vSeeds.emplace_back("dnsseed.ocvcoin.nissay.");
vSeeds.emplace_back("dnsseed.ocvcoin.tjx.");
vSeeds.emplace_back("dnsseed.ocvcoin.grainger.");
vSeeds.emplace_back("dnsseed.ocvcoin.latrobe.");
vSeeds.emplace_back("dnsseed.ocvcoin.hotel.lk.");
vSeeds.emplace_back("dnsseed.ocvcoin.lundbeck.");
vSeeds.emplace_back("dnsseed.ocvcoin.intuit.");
vSeeds.emplace_back("dnsseed.ocvcoin.gallup.");
vSeeds.emplace_back("dnsseed.ocvcoin.guge.");
vSeeds.emplace_back("dnsseed.ocvcoin.nextdirect.");
vSeeds.emplace_back("dnsseed.ocvcoin.tushu.");
vSeeds.emplace_back("dnsseed.ocvcoin.clinique.");
vSeeds.emplace_back("dnsseed.ocvcoin.lefrak.");
vSeeds.emplace_back("dnsseed.ocvcoin.tiaa.");
vSeeds.emplace_back("dnsseed.ocvcoin.jnj.");
vSeeds.emplace_back("dnsseed.ocvcoin.swatch.");
vSeeds.emplace_back("dnsseed.ocvcoin.homesense.");
vSeeds.emplace_back("dnsseed.ocvcoin.teva.");
vSeeds.emplace_back("dnsseed.ocvcoin.hdfcbank.");
vSeeds.emplace_back("dnsseed.ocvcoin.prudential.");
vSeeds.emplace_back("dnsseed.ocvcoin.mattel.");
vSeeds.emplace_back("dnsseed.ocvcoin.staples.");
vSeeds.emplace_back("dnsseed.ocvcoin.pictet.");
vSeeds.emplace_back("dnsseed.ocvcoin.ricoh.");
vSeeds.emplace_back("dnsseed.ocvcoin.flir.");
vSeeds.emplace_back("dnsseed.ocvcoin.marshalls.");
vSeeds.emplace_back("dnsseed.ocvcoin.lanxess.");
vSeeds.emplace_back("dnsseed.ocvcoin.stcgroup.");
vSeeds.emplace_back("dnsseed.ocvcoin.bostik.");
vSeeds.emplace_back("dnsseed.ocvcoin.lplfinancial.");
vSeeds.emplace_back("dnsseed.ocvcoin.homegoods.");
vSeeds.emplace_back("dnsseed.ocvcoin.merckmsd.");
vSeeds.emplace_back("dnsseed.ocvcoin.olayangroup.");
vSeeds.emplace_back("dnsseed.ocvcoin.inf.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.cc.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--3ds443g.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--55qx5d.");
vSeeds.emplace_back("dnsseed.ocvcoin.ltd.ua.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--6qq986b3xl.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--fiqs8s.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--6frz82g.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--mk1bu44c.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--tckwe.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--vhquv.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--fiq228c5hs.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--q9jyb4c.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--t60b56a.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--5tzm5g.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--9dbq2a.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--czrs0t.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--rhqv96g.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--unup4y.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--fjq720a.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--vuq861b.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--ses554g.");
vSeeds.emplace_back("dnsseed.ocvcoin.sch.id.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--io0a7i.");
vSeeds.emplace_back("dnsseed.ocvcoin.net.id.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--czr694b.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--czru2d.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--hxt814e.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--3bst00m.");
vSeeds.emplace_back("dnsseed.ocvcoin.co.pl.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--qxam.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--xhq521b.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--1qqw23a.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--otu796d.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--45q11c.");
vSeeds.emplace_back("dnsseed.ocvcoin.xn--imr513n.");


        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,115);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,110);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "ocv";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_main), std::end(chainparams_seed_main));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                				
				
				
				
{ 1, uint256S("0x2cf1efbbbfdb6f808ed133559b520de56e8130a42f7f0ec3618bff41b1417c60")},
{ 287, uint256S("0x31858291de35c4db0cb050d91ba797ae7bdaac3568c55428b8ac88cba52d229d")},
{ 288, uint256S("0x0000160d97d486e1f710ce44a8c8ec0042c382b8b8ba6fcb7ae3c5ab7f38ccfb")},
{ 289, uint256S("0x000052f0422c1d38f2ceb3bd39f5e46d79140c6bb157f30b434ced966f20bb87")},
{ 999, uint256S("0x00003d47eebadc63c1488eb929fe3400a5095651f3e47547fdd856cbd8e7e918")},
{ 1999, uint256S("0x00001b58a88a47b8b00ecd4edd9cded8e370f741359e0949565229134365a8f8")},
{ 2999, uint256S("0x000012de76ef7e51d1ecfc76eb6cc61f4ceb6edbe1e914752dd0ba76ec5d2d1a")},
{ 3999, uint256S("0x0000048d43d2cd329db076409c8ce1a98df02d0a1411f683cb89b4119738d026")},
{ 4999, uint256S("0x00000c057e7cda89e043aced6a071354375d9ea9a03ffe848c5e561326a4ede9")},
{ 5999, uint256S("0x000010e310fdca829edc683d97e03ab1cb915250915c70feedd21cee5769a1ef")},
{ 6999, uint256S("0x000001393742b9fbf5d3239083abbdfa4b577ecbe464fc08b5887f7e67729ed3")},
{ 7999, uint256S("0x000002811d0158bc62f91d47ec9cacb18a806e4b7c4ef84c80c08c4b61488ea5")},
{ 8999, uint256S("0x000000a47d407950ed39bcfaa86864cdb1f79578e47f6ef13dfe66bbcc950a5d")},
{ 9999, uint256S("0x0000002c3496c26d5b16b3bef58d75fe81843e082c79e355c1572cb4e1be5d87")},
{ 10999, uint256S("0x00000010674d3839bb55b28224049fc8f3fe265521fbfd00c892394473a69f17")},
{ 11999, uint256S("0x0000002735c02085889c34d96fae74157a96af24def37f63406f6e7c93d2d022")},
{ 12999, uint256S("0x0000002d15a34bd53357b7fc950d2c333bd29cdcb935584d289e008cb99f087b")},
{ 13999, uint256S("0x000000203a4accc4127f582469e47e7aed67bc03d4c33c0365f58cc0b555a499")},
{ 14999, uint256S("0x00000025ede26ae8ea4d8d363d82c4c1142b387f5edc53815574d4759df58c16")},
{ 15999, uint256S("0x00000035218d2b343e52684fdd1b7ee1dc2bd43d44f5fb0b6dddda21bc7e848b")},
{ 16999, uint256S("0x00000030f18db98de2028259ade9f6001f71fd7fe11023941733eeb8da899e46")},
{ 17999, uint256S("0x00000003040881489580cc38a65941784cca351152316ed4e890893cd41a86e7")},
{ 18999, uint256S("0x0000000933c9570c9af9736e97cf6fa1f9b303a56b27a705bea20d3ff049aaa5")},
{ 19999, uint256S("0x0000000b2b4c445468a94e0f72e2798ea923743fe50945cce437e750349af6cf")},
{ 20999, uint256S("0x0000000ce679dbd0c241a39abd168e8693c3d94156624688be9564e1d0761cd7")},
{ 21999, uint256S("0x000000014ba2c5220c9d3130487bd996cdb474fb388bfcef849f1e0fb0acb537")},
{ 22999, uint256S("0x00000005f0ffa76a27388820aae97dc1ad4c5e03265569724b499ed4bd6198e6")},
{ 23999, uint256S("0x000000016dcee307ba47684e1f26d9514b9dab85bbf0785b2ad27fdec9aebf5a")},
{ 24999, uint256S("0x0000000141aff607c03f4ceb723d07305f832061f9526c2e1a3b81cb2a98cebe")},
{ 25999, uint256S("0x000000035e5c7e9d168e0a6f0c95dcb93c5e32c8d51cce153900cc3de46bb385")},
{ 26999, uint256S("0x000000110d8ce947eef4c41dd9ee5da338b86083e8cfdeaed779861e97a2bfdf")},
{ 27999, uint256S("0x0000002919fbebc3c2a58ae79872840490a781ef5af22a5438aa7ad9a26f3f83")},
{ 28999, uint256S("0x00000007c2d65b8c3eb7372e6f6cb4ac09233061af99a7c34684f4095c74e6c2")},
{ 29999, uint256S("0x0000000fc65fd49413666879f2e588a659181d63816c89d76fbd2fdadad19329")},
{ 30999, uint256S("0x000000061cd12e1d47f0f979514a6a06062033a9585c7fe1b44b0d079019f410")},
{ 31999, uint256S("0x00000010037b1fcf4c2922dc70bf63dbbf44baa05ea3db9d8f67450fe15d3acc")},
{ 32999, uint256S("0x00000002d3b772784610936e5f94de9ad0ada851032770815ef761cc65df1daa")},
{ 33999, uint256S("0x00000008a6895f3de75f5bd98005a08f34b1a2461111b9cb66abcb0f20c8b6b8")},
{ 34999, uint256S("0x000000178dee8b1dc2450f60a0fdf300d6250d16bae25cba83505e5893ffe74c")},
{ 35999, uint256S("0x000000137bfaae78d772ccffe2b2389208ac32932604ae71859323f9d44939e8")},
{ 36999, uint256S("0x0000000553ff8ef8e43e5456192d580e9ccade7368b24e7cf58382306f880952")},
{ 37999, uint256S("0x00000020006fe60367317bce33dcab9d0b4e9c871fb4651a2ff8d100641a8db7")},
{ 38999, uint256S("0x00000010847a76e9f351e81e37a32a7c22d308a1d412512a4c5b32c263894837")},
{ 39999, uint256S("0x0000001f2c21cffed7dbff740b4435b758709f2a20c51031770ea91aa9cb1028")},
{ 40999, uint256S("0x0000002333d82ffea709a46eac7b186b1c5f3cac4f3497e3502e66f46e441e5e")},
{ 41999, uint256S("0x0000000cd653681506c7247a939ad84eda0c89056cd1a5b0e306c6aa8d75737f")},
{ 42999, uint256S("0x0000002519f7519edac5f51858e33338f6cb028757726ad78aeae7ad93e2486d")},
{ 43999, uint256S("0x00000016e9b92157bbb3c1f6e40fff90455d0f65d1417449afa167aa7d1a900f")},
{ 44999, uint256S("0x0000000a902012bccf08a7cf1a6eb47b51d4500e4a1ce3151a2b87c9a5d0125e")},
{ 45999, uint256S("0x0000001d912ad1f659c8c3ddd2ac01761255b98e6446e94f48a77fb09d853673")},
{ 46999, uint256S("0x000000035d9a332385c50d1582be4446c6ffdd2c6a4dfbb5d5dbebda9a1bb876")},
{ 47999, uint256S("0x00000002880f2c07747160a735f4384a8d9cb8cf00ca4680238c756e8ba48666")},
{ 48999, uint256S("0x0000000609ef27e22053825beb3634a773565df3e59ab8633073f93d3ebbd32a")},
{ 49999, uint256S("0x00000007906c3290a32ac73b793d8756bca7bb4537575c3e27ea3d47ab8313f0")},
{ 50999, uint256S("0x000000047a7b75e4a4a9583c2808e8bacc56464c8ba7d32b75d1ddef18f4757e")},
{ 51999, uint256S("0x00000000164aa3215b3bf3dab1e1a884bcec74cecea95ca5feef99b0b1016034")},
{ 52999, uint256S("0x000000010f58d32262a2627f2844f2e20fcc8738633ae9d71e75c1b619ac11f2")},
{ 53999, uint256S("0x000000016e8f69b005fb36d22c7c0b67f6b1cb2b36abac2b14ad8cd122a3b37e")},
{ 54999, uint256S("0x00000002ebe048b621cf6daeef0c0ad0529bef3adceb4ac87cd9e6f572b69aee")},
{ 55999, uint256S("0x00000001d4498a19a528b8c1ab8752b204f2951876f2c379ab51ff52c625dea4")},
{ 56999, uint256S("0x000000014101005ecf48550fd4f4f47ea892f8e09a12c7dd0cfabd7b2d284c32")},
{ 57999, uint256S("0x0000000322165430ed7fe74b380c296a8f0e6f7d58fbcb64fd14a12e6706a4c8")},
{ 58999, uint256S("0x000000020b048215a7dbc72ceb0ad7f51a665014c5b0a847483862c4b8fe80eb")},
{ 59999, uint256S("0x0000000318a1d657b589cec4f93b2dd4d8575ec12f567388058e5c9da7bb7a19")},
{ 60999, uint256S("0x0000000107e6539be34685205fc4353addc62b6e0643c4f8b2a308f10f518b4a")},
{ 61999, uint256S("0x00000002f1870f0d0d99eadc87dc8003c1c2a46e085c49f98a8f3b98c62ab356")},
{ 62999, uint256S("0x00000001e900fab1e886da7ecabd939c96c87df7a445ca73d86cb70b312f29cc")},
{ 63999, uint256S("0x0000000265ed24d4287b230cf32043fa4f4a1c08369e711fb7b70f24ca8e0413")},
{ 64999, uint256S("0x000000010152196e6c0e8fbeb0701bd981333fbf2ab37ba7e1f29fd6ea06fbd5")},
{ 65999, uint256S("0x00000005a90d422a72b456499cd66a22d86c195af08a2a01128da37af7135b12")},
{ 66999, uint256S("0x00000000cc5cc9d90bdad7fe448bff56679ccf2b22d9197fabd4c4ded9b51186")},
{ 67999, uint256S("0x00000001f0705eef4cb74585edc35fa05e9de57ceb6780b8d9ea42ef786237bf")},
{ 68999, uint256S("0x000000055dcfe8ff614a21dbe7f73f39b497fcd2a301488275437fd2a5625d72")},
{ 69999, uint256S("0x0000000768148df0e058669dc31cf7e9d21ef299d62ab60b87ebdb8a69a57f61")},
{ 70999, uint256S("0x00000005938eae0031dea32ed6160a573daea2d031f4eecb2c08f563034b6b2a")},
{ 71999, uint256S("0x00000003e9940bebdacf92eec0e0a3b1a670dc7137f578d87d1a5f8608a2b354")},
{ 72999, uint256S("0x00000000539892ea9f0fa87965b06c6859da028a6f6dac34d1a108a74af7cc37")},
{ 73999, uint256S("0x0000000f1a8b2ab52152d67be3bfa9c8116a3d0a62b9420ecf7e82316a38549d")},
{ 74999, uint256S("0x0000000214ec9c693f519d3a56e13c887e3483c177fb86be5f1b7289665c8ccd")},
{ 75999, uint256S("0x0000000c70ee77298628e1e12101a7e5ab6dcaa48efe1a3587c983ececcb1fce")},
{ 76999, uint256S("0x000000088c2483b5d4cd2edacd51beabc63312302b2e4c8cc3ed1ba7b63756dc")},
{ 77999, uint256S("0x00000011ae221ed3b19f3908b48105a5c53dfccb5540ce3b1ec9bd60f8b0f988")},
{ 78999, uint256S("0x000000043cf5f4997076dc491ef31fd8bea3d81e59a6938b2a52322190b1794b")},
{ 79999, uint256S("0x00000011a8eedf83345fc309f95d9df447d233f7b89b6fdb6585fe423002eafe")},
{ 80999, uint256S("0x00000002664b9b9ea39334de6895aaf38ea1aed3708b6cab0c6e21ed18af6fe9")},
{ 81999, uint256S("0x0000000456e0c4075f527f32573210afa88310fc5aeee3fa046dff603b89d791")},
{ 82999, uint256S("0x000000041b0214917284a0dbfa64bb544d7fcf7c771bba8c41c7677a84bddba7")},
{ 83999, uint256S("0x00000009d38d40dbe227a0fd1e9611a8d55bc3f2e687ee3d766899a3c2dc02de")},
{ 84999, uint256S("0x00000011d36d44b717f4c8ab57daffded39ef4ccfa885c099a51a06dc84e23c6")},
{ 85999, uint256S("0x0000000ff1fdc5fb0e0f84aa9c9673679a543398fea8344e4c22a38c5c455a5e")},
{ 86999, uint256S("0x0000000210d295be7d0216388a49dfed088945ab8b04b1e3953a18d9b877d088")},
{ 87999, uint256S("0x0000000b5c2900728f6d5953e89fd45bb51314a3cbcae73a8a3e6979a331ffab")},
{ 88999, uint256S("0x000000027b012d5d9936c836e4909a3935a223ca15d1460c56a84015eb2553ac")},
{ 89999, uint256S("0x000000045ec3a1418df0adb31d7b7de60f7a648655df5bd453c2a0c4d93a78e0")},
{ 90999, uint256S("0x0000000008cb55b2e743d6dbfc5ac73c2898a329b2455e52142e852f8e6cfdd9")},
{ 91999, uint256S("0x000000045d5ce80dd246019bcd90c893d14995083ee63eb5ac09c0ed8e9d1b1b")},
{ 92999, uint256S("0x000000063fa48dcc115be1e2c249c227f1e6c3d472de5e1a5c46c28bc5b2a83a")},
{ 93999, uint256S("0x00000006847a9ce7c67266600bfb4962ede3953e9e5d9aeda5bfa8224cc733be")},
{ 94999, uint256S("0x0000000043702d2496b4d9a5541fdcafdf97b153096f4a677dce246730cbe44a")},
{ 95999, uint256S("0x0000000536268bd94ad10cd28e108a3abfa2fece27a6ab58fc2654ecde89bae5")},
{ 96999, uint256S("0x0000000a61cfecf589fa280bcdc9db88328dfcceaee1dc4857f3b3295bef9677")},
{ 97999, uint256S("0x00000003d6a079b97afd4145e44ac7035c21490758bd2c87ebb93257047889eb")},
{ 98999, uint256S("0x00000001d5242ebf5810afa3bddb0b80c039e6259496ea0582d160f6f40e5b07")},
{ 99999, uint256S("0x000000064de755d575a1b761bb248a5da7cf61d8ab82db51501c53ff3f313d79")},
{ 100999, uint256S("0x00000001cf90c022b4d6a34eee3a1364c22b24612a144717431b69db8e9a7f0b")},
{ 101999, uint256S("0x0000000b319f01f7ced0a55bc70dd323dd326b2bcbce9b3d1de261eb85ec889b")},
{ 102999, uint256S("0x0000000aa172fbb3cd59db39310d7c1a55c484ad77faeb2b2de2f71ad371183f")},
{ 103999, uint256S("0x00000007ecc2940dc2c9172abb86a62aa90b82d11fb063796c9fd2353c26ac11")},
{ 104999, uint256S("0x000000066fefc90b3b72138a92a0107ae00d5ba6c07bda9c81f489db72b528c7")},
{ 105999, uint256S("0x00000008272bc187e2610da62d0f1a33e4f448c4173455e9610394b5899c08f4")},
{ 106999, uint256S("0x0000000ca7a6c46fd377bfbe6eebb7cc934a86bf6fb8d4bd3117d17737f705a8")},
{ 107999, uint256S("0x00000006712bc91c032f4f264ff593d277753b7a435afed9d578f9de88975212")},
{ 108999, uint256S("0x0000000cccb726f7c93ed89c30d538d12a1a37c074c330649c64b57a4ddcbb6c")},
{ 109999, uint256S("0x0000000d01c2bd1b0bb840ede3ef4278e5b15f23c2ecc5ba11ae09a6f3c827af")},
{ 110999, uint256S("0x0000000875d6a1da3d2a8db4aa4576f9c998f236679a7ab90e292b54d8033a79")},
{ 111999, uint256S("0x000000021ea5a56ddab7c08c422180394c29c7b78c36a5a256df7ed390cf0b2d")},
{ 112999, uint256S("0x00000004cdf9b1ff8bf89151c32db92739192efd0d7f3966396bce3fb0b2a5a5")},
{ 113999, uint256S("0x0000000e0596c17f034cb1ad276d2ce3736772444c06731e308321afbae062c5")},
{ 114999, uint256S("0x00000003db413c2abaf8a6b197a84be6020ba19f22156c3e4932189208d3d699")},
{ 115999, uint256S("0x00000014b5f95f9781e6ac6c8df492053c777b234a778f036a7b866ad291059f")},
{ 116999, uint256S("0x0000000158f70e97ec74113831a2e8a31d4f76f12b564e79fbb775ebf6e7ca97")},
{ 117999, uint256S("0x0000000bede4669360436fac8ce6786082a8f6b50e0b62450170d4c1972e5aaa")},
{ 118999, uint256S("0x000000036a5ec4778027185f992ac4768fa62f9338fc4998313a9adfea255f31")},
{ 119999, uint256S("0x00000017a5262229e2a1345a409c8e7d0eefec0f3bf9a6d13eeb22a48c4f2558")},
{ 120999, uint256S("0x000000053754f0741a1a9e66c2825fdcbd730aa9572368d3c83ab11cfbe8e407")},
{ 121999, uint256S("0x0000000e960c07d33582443fafc3749052d7bbc038993785b27dc9717390f644")},
{ 122999, uint256S("0x00000005b9200de8f26f7954b4d58119563c08641c4fda4da961e8c8618bd4cf")},
{ 123999, uint256S("0x00000001be58d3891e0153bfde61314e02b247456c216462f1161fb595164f5e")},
{ 124999, uint256S("0x0000001aa76b9711adcfb8276a02b3ddf75c69d01b215c26b7c6ae5a0144a0be")},
{ 125999, uint256S("0x00000001cf31f40183fcc4ad8ca03590bf222a89f1b3c41c1cd428d47ff9b831")},
{ 126999, uint256S("0x00000009c27d0dc7e105a6d1ec9c8e31a65726050a328c1e16147367054a8da3")},
{ 127999, uint256S("0x000000053ea5764173f4aee0e6bad9971d50704c9ae0bdcc5b514b52c7559be8")},
{ 128999, uint256S("0x00000012f4c5cc692fcb8e1f8222206c2b5768c2ccff76592c85411c53f6dce0")},
{ 129999, uint256S("0x0000001079b407674e9a11d79ef3af76d4f897416a44a9235b4395c1372fc1aa")},
{ 130999, uint256S("0x00000006e15f1724b448a9b6389c6ba56f985e67b2737225773fcd80e54b2825")},
{ 131999, uint256S("0x00000007a73647482b166abf0fc45c912805f6ccad708ac0ba6e78573455a59e")},
{ 132999, uint256S("0x000000163ed7379801cd4b8464f4a3b870570264929276be241951ad8b5562bc")},
{ 133999, uint256S("0x000000128e382e638b6a2016b70258dd93dbf712e931c928afb4578f5f3f923c")},
{ 134999, uint256S("0x0000001540d7a473c51b18ec4721544a0f972a76cd336fff54b66f760edfa7c8")},
{ 135999, uint256S("0x0000001778352d4134cb6c1154dc684ef06507e808e6fca0ce5d9f976cf3a92a")},
{ 136999, uint256S("0x000000065c38af27466bc144fc3bc2c898880ca831934458412a15295f13e40c")},
{ 137999, uint256S("0x0000000df7dca626ff23eef9a0247d12924f3542672ef08c757b76cbf2c5b19b")},
{ 138999, uint256S("0x00000000a2bf529dde1b02650dd41930135fb18aafe56c77647992c11a6005ca")},
{ 139999, uint256S("0x00000005724d757f7443f19b70cb717dd843d3c125308baf1ac81e208f06d9fc")},
{ 140999, uint256S("0x0000000235659b54622a3bfb943bd17688a45cd6bfb29c81265913f9d0ca7114")},
{ 141999, uint256S("0x000000077b41d53ad8f9eb8a547f69e13016b9a6f4bfd4d6a3a8ba57fcf358d8")},
{ 142999, uint256S("0x0000000560993937ae9b86821453bf6c1a47e796b47400b223997a651044d78f")},
{ 143999, uint256S("0x00000000049620dcc28e5e4e8d1866e15ca2f9e4b7fbc623827c5e69ef00ac44")},
{ 144999, uint256S("0x000000060bf9eb72b8a37c6c72045464b3983bd491b44ce0a446c7d9f2413a96")},
{ 145999, uint256S("0x00000008d9cb7f2064704a69cf6f08d86f0daf20b735091f02120966223576cf")},
{ 146999, uint256S("0x0000000312c53635b2edbdf9f354dfcaf41a1c8c6f796892d22e7bab4d62ec46")},
{ 147999, uint256S("0x000000014a9f4e5b83a07d30ccd988d22e9efa5077d18a66fd7d19e509c7c739")},
{ 148999, uint256S("0x000000001fea8eeabad5de52e153760eaf7b7f098ab4ded3e06ea724bef24d62")},
{ 149999, uint256S("0x00000001348b9027a0c783a550c325f8278c6fc70747e0ac21748cff5020ded4")},
{ 150999, uint256S("0x00000000679873ffc27774e1489735b050c8203ae23b4201dba56380b39d7c87")},
{ 151999, uint256S("0x00000000da7694b67254f23542e5330047547bf20b5325ee3ac1e460a4106ddc")},
{ 152999, uint256S("0x00000000bf01475e6ad1cbb4576903787bf742a8f4118c098c1b07f6923731a8")},
{ 153999, uint256S("0x0000000026a57eb2563d3325fedae5db5b53f8986f953de64c4b582febed810d")},
{ 154999, uint256S("0x0000000130c39cbeab0488dcfa2c2c32224563e3dcaf2d78cac613086ce8ca49")},
{ 155999, uint256S("0x00000000c54914d2639e2c2b0346a5d0f4cfa7c1e5a22a60e1be7b49fd33e601")},
{ 156999, uint256S("0x0000000120f53be59abd9e8686e885a1ccbd5f65b544806f31ef3dbde2f59e6c")},
{ 157999, uint256S("0x00000000b25926c2dedb0fa2a96160d559bcb9b59d38a81afe8c40d4938601d9")},
{ 158999, uint256S("0x0000000122ba4ddac14767135104ac3f01616220703953a64074bad0dd1346af")},
{ 159999, uint256S("0x000000016d7973c1f7305da0d5ba0c1f1345afff83fd7f0667d24ca0c8183c37")},
{ 160999, uint256S("0x000000005e6b883a62e69980120399f35f93b142feee684e10ad95a60a9c3379")},
{ 161999, uint256S("0x000000017743864bb7f4dee35acf8bdd23dae299a53bd799a0f0dd1d8d7d4de5")},
{ 162999, uint256S("0x000000022d9a8ae24e68a254dafd8f1d835b24ea18f9fefffba05b57a0c28aff")},
{ 163999, uint256S("0x00000000d79b89728324816f9175103213fda05b42c494f716fa9670bb734964")},
{ 164999, uint256S("0x00000000012d75190c031df10faf7a4d46a048debfb0932e56a8ad4a04ad7c8e")},
{ 165999, uint256S("0x000000017efd784b50915dc3c0f1ae7b93c365d148fcc338879127550f16aa06")},
{ 166999, uint256S("0x000000021191ff6aaa6a1af1ea583199185f9aca1b475268b23fb45495e11638")},
{ 167999, uint256S("0x00000002815da5fd71b0c5ac140cf26808af49b8953c1334add5a52c2408f658")},
{ 168999, uint256S("0x00000000a61f581386b1160aaddda7b702ab625de26572861cddc20df9551694")},
{ 169999, uint256S("0x000000026a00cc134b7c0592d07337b36c17dad740735c3acfefd37862a1af9a")},
{ 170999, uint256S("0x00000001fae65380d4d05263e53fe6a306370127cc8794bb276918658a8788b8")},
{ 171999, uint256S("0x00000001b14f1fe1f545b0ffd9ac634696ebc65cc52bb0ff5dc9df9b2e159a2a")},
{ 172999, uint256S("0x00000001ec9406152e11393046ca63a33855ef85ba62af306ad69a3972192abe")},
{ 173999, uint256S("0x000000020d63fc52c4bc035672688de9165ecedfed58840e360fe08b7c75320e")},
{ 174999, uint256S("0x000000002948e16462d59a5204ce05e3e18e421135be430bf4926d859f319cb9")},
{ 175999, uint256S("0x00000001246db5e059440f520975873b2e395eb17c619349f928c37a35a61f0b")},
{ 176999, uint256S("0x00000001e6e5ca6326ba1c1d8449ac0e1956f4f46bdf1c9275b5d684e5fe35e8")},
{ 177999, uint256S("0x00000001111e6a46de7dc6bfdcc5890ae08954961aa3bd0b2d6be5109f19f398")},
{ 178999, uint256S("0x0000000049cf7ad6d08587dca41487291b25acc7088738d0668310810a7faf76")},
{ 179999, uint256S("0x0000000173fd89ca345989e6b31c60a7aadc60642b026162eb9d516b1e7f7930")},
{ 180999, uint256S("0x0000000140a5207e46378164f37740798d822f33fa9b9607e2d9095cc8dd0ddd")},
{ 181999, uint256S("0x00000001946ce6e2b73748f488abd13567fbfbb637dbdc30b57736cc68fac03d")},
{ 182999, uint256S("0x0000000043dc2e27a1a9d4987e89ef3614dc0441005139beed82734ae11d7f23")},
{ 183999, uint256S("0x000000000bf32af0a6d12072e89315f634d8366d37e0142f7c9a8c8e3cbf9dfe")},
{ 184999, uint256S("0x000000004cb87d4d6bf70009f504e51c707d0c4d963cf77c721924d3d16944fa")},
{ 185999, uint256S("0x0000000001da49d3dfa3d2de6298f61bf7339a75828b56d6d57913659d45aa19")},
{ 186999, uint256S("0x0000000017923c4c39e2be2e6edea908117e902bf67cbedd3e42a8fc42d40cf6")},
{ 187999, uint256S("0x00000000143d557864f3e8219684f19015264f6c621781363f3fc481c92fd4a2")},
{ 188999, uint256S("0x0000000002003f0e5e16313ddedd0cc11110d9b69e3e9a7601adee545a4c860c")},
{ 189999, uint256S("0x000000001a12df2d066de744ca81ad3b7cceb0b6bd916de030e3e6010c0bd4e4")},
{ 190999, uint256S("0x000000000ee011913bf22c4f88532fcfe6065c89399c4bce6700fe476bd3405a")},
{ 191999, uint256S("0x00000000098e149cd7dc0f418617d87afd883d6710021b17eaee5175abde9186")},
{ 192999, uint256S("0x000000000883b4a71eb59eb174de3e8fcb24ed78470ff5a9e4571feeb2183b2d")},
{ 193999, uint256S("0x0000000008cb524282f4c93482f897b405d19a3b72c188249c4cdaea6a059bd4")},
{ 194999, uint256S("0x000000002d76c948d2f5b269b7bcfa15101fbaf3ff2d92812a32f08dd384ce86")},
{ 195999, uint256S("0x0000000004a0f6e1b21c0ef6ee53f201db20f83edca49e4b827e81a8e444a739")},
{ 196999, uint256S("0x0000000006d00007b0ca7ccc460837f761567cfff9a9c84ec9b63f2cf07079b8")},
{ 197999, uint256S("0x000000005e7f2449f48bbff11fe8e2bd92baae7d9474bf9231064f6679e593fe")},
{ 198999, uint256S("0x000000000c9c50a7ea00d24082943f8c6e9d3d45190fe0f9865d4b46eafaa4c3")},
{ 199999, uint256S("0x0000000045d540b58e21d6a1901001b903a1f4df9975a2f5bec72041146498d4")},
{ 200999, uint256S("0x0000000032de9e6d69d337c22bbe36cf49305f5a4556094b58ac27a967937bb8")},
{ 201999, uint256S("0x0000000026804a5108061eddbd9601486bf5a2dbaa2802c8f1efa1dea1355689")},
{ 202999, uint256S("0x000000000ede98d7f41d2607aacff8c66d18454af1af2e431cea07f15fdc5ee6")},
{ 203999, uint256S("0x000000007a33af8f012e36b66c627b4d361627c1ca9442241fa6f34815958608")},
{ 204999, uint256S("0x000000002145cfb6d4cc1eea3fe58a19cbc93286f2138eacad1c3b1ad1716dd9")},
{ 205999, uint256S("0x000000004dd369b8a7dc89f4023f899fce364c03f19fb8a051fd8c26d56059b8")},
{ 206999, uint256S("0x000000004842297d557f2f3249c5dd9a368148b29934567546ec7344a30963ca")},
{ 207999, uint256S("0x000000002dc74567d8ab9a728ba4a51a5b431cdb47723d39bd42daa0f0cf80da")},
{ 208999, uint256S("0x0000000067d6f6d3621b89261d5023fa3568fb5c236e3e50c8e5171c75b8315a")},
{ 209999, uint256S("0x0000000034e54b2bbbc7d89ab81067b1c166b62c3cf8f88022cf5681baf61656")},
{ 210999, uint256S("0x0000000039b3a0cd1f95f5cdade465b859bad0f29e007de9b7643b0d2f748b55")},
{ 211999, uint256S("0x0000000097b13f328e826fc50c4991389525e91c660db6210ceb578e3562aa07")},
{ 212999, uint256S("0x0000000015359d2925577d2c24ae90071d993d11b4981010cdd350fe2f6f0034")},
{ 213999, uint256S("0x0000000022023256804271426419abd604e5e01d55dd37ce6a9366e5f31c3cdb")},
{ 214999, uint256S("0x0000000039105da03399da71d44dfc1d415e918c1a3e6c172588543a07df8464")},
{ 215999, uint256S("0x00000000238238af471b9b58fd487c6b8a676e63f4240058c73a58a04f8e148d")},
{ 216999, uint256S("0x0000000010d9f269ecbc293eb5e76e35c4a9e6c1e0abafcfd1f04b9cd78b5d2c")},
				
				
{ 217999, uint256S("0x000000003e4b49762913e860807723322e9dd42b7a5add82aac64d7e7cd0008b")},
{ 218999, uint256S("0x000000000504bcdf78379104dc92b3537e6178b4e376cba8f77f408c235c018a")},
{ 219999, uint256S("0x000000002589c60b6b357b8bc367e5fa32619c23f8302306072557afdc0ad389")},
{ 220999, uint256S("0x000000004ce3b318c0c56ad944c6b317604ddc9d1c1f8201cb2818a9c3608a95")},
{ 221999, uint256S("0x0000000026a44218b7568bddcea83c53c8e8e6dc1b5d140173a0efbf1ccfa9ff")},
{ 222999, uint256S("0x0000000019c2256769d0978385319715685a8abc5fd4eb89dddeda1f52955262")},
{ 223999, uint256S("0x000000001e0f857a1b47089c8e6aaf3ca236837dcafe0dc5f1c3606824779f10")},
{ 224999, uint256S("0x000000000bc091c1597f3167030baf0a998267d07f35a7a7f1fa04aac715c4fe")},
{ 225999, uint256S("0x00000000f4483c4232bdb4589af181248b7f5cbaa1a1eb24f4c359f140b5a48a")},
{ 226999, uint256S("0x0000000040fcd981bdc8717daf3b3ba7dd8bb35fbddccc0e98e040c08a8ceae4")},
{ 227999, uint256S("0x00000000312539dc8a99fb0e812d04e0b47cf9967a879971c044aad1215eade1")},
{ 228999, uint256S("0x000000012a00d3a33bdaec088ee3a024641e15f3b7a6ff6917bc548bf1fe84ab")},
{ 229999, uint256S("0x000000006dceb4c46375670087c677ba8a9c13c3a26261934fa8733b697c6ca8")},
{ 230999, uint256S("0x00000000043fdfa98e097549c3ad41cad2a010b4e08a2e78912a1685de41671e")},
{ 231999, uint256S("0x00000000e7a749cb899f377d991fed3e8e524ba53cde78b638804af975ea7d72")},
{ 232999, uint256S("0x000000012156b406c5f48c2d6cc408b704c135adf283cf8ac2e38ff9e1c98f50")},
{ 233999, uint256S("0x000000015b581e2a88986d356d983de30956a3b428a94c5b0c47e9727844f1c7")},
{ 234999, uint256S("0x000000013c5bdbf0c98627d279a07d3b3b42af50fef472158c188cc8c4874674")},
{ 235999, uint256S("0x0000000014de2c67bbaf9edce7c9a83bb8b22ca6fc1c7f64b83a4c018bb54f85")},
{ 236999, uint256S("0x000000003cc1da29e40fb0f8606a247c810fd647621d1dce1940db344b96022a")},
{ 237999, uint256S("0x000000000a81025168598690dcb3b36ca3685d3e2704cab2ff46f5d4f19e41fc")},
{ 238999, uint256S("0x0000000071bb1407bbb84e275b89b44ec8c5d65f556091d1b9c01d5d952d5725")},
{ 239999, uint256S("0x000000004cee1dd3ea9c75c317d5405ac5e4c2ca24d5b4d6865c660b552d684b")},
{ 240999, uint256S("0x000000007f2154886608a9084928ba7bdfd9e7c0df9b9e76a6613c2f2bb53f21")},
{ 241999, uint256S("0x00000001393f71385369b8db389a82125dc05a5a6a4b3c9da742f81159ef7c66")},
{ 242999, uint256S("0x000000010f8289422d5067e18f65c49bbad0847f8a8cbb02d29c1e58351f9039")},
{ 243999, uint256S("0x00000000929f7b883652236a99bbc50a03ddb6936b46e06d786732b374f98eef")},
{ 244999, uint256S("0x0000000012b5b11636978230a9bffe4cc15a03a5f4d5a4fae3145fd4a43131d4")},
{ 245999, uint256S("0x00000000d7344dc839a20870ad080e2c8566cb63cb850ea31820dddd33c21523")},
{ 246999, uint256S("0x00000000825a9e5e969e9bb1dfeea2476693995cfc8018716421264786eeddb7")},
{ 247999, uint256S("0x0000000046d974b66a5dd4b067054274919bee7c4f6ba6de34411447c2027d6a")},
{ 248999, uint256S("0x00000000c1c62e14f625fa2ada53a9b8ba0f639a7de38b1315f33c091ebb0045")},
{ 249999, uint256S("0x00000001039ae54ab6e85827ef70dced0a32b102a6265ec17660f633674b6eec")},
{ 250999, uint256S("0x0000000090eb1cd0c621ef517a85a07e7db0adb3f63480c5bea7e67ff0388b01")},
{ 251999, uint256S("0x000000003dc54d9d237508d3de2b23700fb5e92d87dd60baf27ccb5cdeec3646")},
{ 252999, uint256S("0x00000000985b8a0c52de24fcb783bff9c94adbe979a9177353fb7d6db2781e64")},
{ 253999, uint256S("0x00000000fdcc7c37b92919b0a57a26430a442e3dc8d95e9597e0534c1a799777")},
{ 254999, uint256S("0x000000004c41308bb549f4bcf13faf58a09fd4acf267cd9b4e65e3746c4dde37")},
{ 255999, uint256S("0x00000000b5f57252a1eb3a65996f9a42fcf8243b1b3144d511bb8396ea8a8245")},
{ 256999, uint256S("0x000000007fdebb5d0d9a642226f218ccc833f104b47ab16bab4138d85d6f6412")},
{ 257999, uint256S("0x000000009087a5414c3e72e02312a5a748f60c8733b99463669aeedf487ebfc7")},
{ 258999, uint256S("0x0000000117ea9cbcf8c5271a7fd77f35832c5759369395db2eb6cf824a0a15e5")},				
				
				
				
            }
        };

        m_assumeutxo_data = {
            // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats  0000000014f14ca9c9964ac55d8bd0f7cb621848a20f96530430ce7cb8def701
            .nTime    = 1702818668,
            .nTxCount = 295398,
            .dTxRate  = 0.004995486133571073,
        };
    }
};

/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        m_chain_type = ChainType::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000;
        
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1; 
        consensus.BIP66Height = 1; 
        consensus.CSVHeight = 1; 
        consensus.SegwitHeight = 0; 
        consensus.MinBIP9WarningHeight = 0; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 24 * 60 * 60; // 1 day
        consensus.nPowTargetSpacing = 5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE; 
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT; 
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart[0] = 0x0b;
        pchMessageStart[1] = 0x11;
        pchMessageStart[2] = 0x09;
        pchMessageStart[3] = 0x05;
        nDefaultPort = 18333;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 1;
        m_assumed_chain_state_size = 0;

        genesis = CreateGenesisBlock(1630950470, 1, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x054165d4f666bf097b2461d1bd84b3eec64c336c30fcb5d809e83a6fea95c391"));
        assert(genesis.hashMerkleRoot == uint256S("0xb2097d2bfc31e8937aaafe9fa7d634531274463f50e13469ba056cbddc594f1d"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
		vSeeds.emplace_back("testnet-seed.ocvcoin.com.");





vSeeds.emplace_back("testnet-seed.ocvcoin.net.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xyz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.io.");
vSeeds.emplace_back("testnet-seed.ocvcoin.me.");
vSeeds.emplace_back("testnet-seed.ocvcoin.info.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ai.");
vSeeds.emplace_back("testnet-seed.ocvcoin.top.");
vSeeds.emplace_back("testnet-seed.ocvcoin.in.");
vSeeds.emplace_back("testnet-seed.ocvcoin.eu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.online.");
vSeeds.emplace_back("testnet-seed.ocvcoin.us.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dev.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tech.");
vSeeds.emplace_back("testnet-seed.ocvcoin.biz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.de.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.app.");
vSeeds.emplace_back("testnet-seed.ocvcoin.club.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pro.");
vSeeds.emplace_back("testnet-seed.ocvcoin.shop.");
vSeeds.emplace_back("testnet-seed.ocvcoin.uk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.site.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ca.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.uk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.store.");
vSeeds.emplace_back("testnet-seed.ocvcoin.space.");
vSeeds.emplace_back("testnet-seed.ocvcoin.win.");
vSeeds.emplace_back("testnet-seed.ocvcoin.it.");
vSeeds.emplace_back("testnet-seed.ocvcoin.link.");
vSeeds.emplace_back("testnet-seed.ocvcoin.es.");
vSeeds.emplace_back("testnet-seed.ocvcoin.one.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cloud.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ltd.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ru.");
vSeeds.emplace_back("testnet-seed.ocvcoin.live.");
vSeeds.emplace_back("testnet-seed.ocvcoin.download.");
vSeeds.emplace_back("testnet-seed.ocvcoin.work.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sh.");
vSeeds.emplace_back("testnet-seed.ocvcoin.im.");
vSeeds.emplace_back("testnet-seed.ocvcoin.to.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ws.");
vSeeds.emplace_back("testnet-seed.ocvcoin.art.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ch.");
vSeeds.emplace_back("testnet-seed.ocvcoin.id.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.life.");
vSeeds.emplace_back("testnet-seed.ocvcoin.al.");
vSeeds.emplace_back("testnet-seed.ocvcoin.li.");
vSeeds.emplace_back("testnet-seed.ocvcoin.design.");
vSeeds.emplace_back("testnet-seed.ocvcoin.st.");
vSeeds.emplace_back("testnet-seed.ocvcoin.stream.");
vSeeds.emplace_back("testnet-seed.ocvcoin.click.");
vSeeds.emplace_back("testnet-seed.ocvcoin.se.");
vSeeds.emplace_back("testnet-seed.ocvcoin.email.");
vSeeds.emplace_back("testnet-seed.ocvcoin.network.");
vSeeds.emplace_back("testnet-seed.ocvcoin.re.");
vSeeds.emplace_back("testnet-seed.ocvcoin.host.");
vSeeds.emplace_back("testnet-seed.ocvcoin.blog.");
vSeeds.emplace_back("testnet-seed.ocvcoin.at.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.world.");
vSeeds.emplace_back("testnet-seed.ocvcoin.be.");
vSeeds.emplace_back("testnet-seed.ocvcoin.zone.");
vSeeds.emplace_back("testnet-seed.ocvcoin.news.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ng.");
vSeeds.emplace_back("testnet-seed.ocvcoin.science.");
vSeeds.emplace_back("testnet-seed.ocvcoin.asia.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lol.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ee.");
vSeeds.emplace_back("testnet-seed.ocvcoin.website.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cx.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wtf.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xxx.");
vSeeds.emplace_back("testnet-seed.ocvcoin.la.");
vSeeds.emplace_back("testnet-seed.ocvcoin.moe.");
vSeeds.emplace_back("testnet-seed.ocvcoin.digital.");
vSeeds.emplace_back("testnet-seed.ocvcoin.studio.");
vSeeds.emplace_back("testnet-seed.ocvcoin.su.");
vSeeds.emplace_back("testnet-seed.ocvcoin.party.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bot.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vip.");
vSeeds.emplace_back("testnet-seed.ocvcoin.today.");
vSeeds.emplace_back("testnet-seed.ocvcoin.trade.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ly.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mobi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fun.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ac.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mx.");
vSeeds.emplace_back("testnet-seed.ocvcoin.group.");
vSeeds.emplace_back("testnet-seed.ocvcoin.icu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.agency.");
vSeeds.emplace_back("testnet-seed.ocvcoin.men.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wiki.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ooo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.is.");
vSeeds.emplace_back("testnet-seed.ocvcoin.games.");
vSeeds.emplace_back("testnet-seed.ocvcoin.media.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ro.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ie.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ae.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ninja.");
vSeeds.emplace_back("testnet-seed.ocvcoin.best.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cat.");
vSeeds.emplace_back("testnet-seed.ocvcoin.company.");
vSeeds.emplace_back("testnet-seed.ocvcoin.do.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tools.");
vSeeds.emplace_back("testnet-seed.ocvcoin.global.");
vSeeds.emplace_back("testnet-seed.ocvcoin.red.");
vSeeds.emplace_back("testnet-seed.ocvcoin.guru.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dog.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bar.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bid.");
vSeeds.emplace_back("testnet-seed.ocvcoin.city.");
vSeeds.emplace_back("testnet-seed.ocvcoin.codes.");
vSeeds.emplace_back("testnet-seed.ocvcoin.works.");
vSeeds.emplace_back("testnet-seed.ocvcoin.business.");
vSeeds.emplace_back("testnet-seed.ocvcoin.am.");
vSeeds.emplace_back("testnet-seed.ocvcoin.services.");
vSeeds.emplace_back("testnet-seed.ocvcoin.run.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ne.");
vSeeds.emplace_back("testnet-seed.ocvcoin.market.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ms.");
vSeeds.emplace_back("testnet-seed.ocvcoin.center.");
vSeeds.emplace_back("testnet-seed.ocvcoin.team.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rocks.");
vSeeds.emplace_back("testnet-seed.ocvcoin.money.");
vSeeds.emplace_back("testnet-seed.ocvcoin.plus.");
vSeeds.emplace_back("testnet-seed.ocvcoin.finance.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ink.");
vSeeds.emplace_back("testnet-seed.ocvcoin.travel.");
vSeeds.emplace_back("testnet-seed.ocvcoin.solutions.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bio.");
vSeeds.emplace_back("testnet-seed.ocvcoin.social.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.technology.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ovh.");
vSeeds.emplace_back("testnet-seed.ocvcoin.review.");
vSeeds.emplace_back("testnet-seed.ocvcoin.academy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.international.");
vSeeds.emplace_back("testnet-seed.ocvcoin.university.");
vSeeds.emplace_back("testnet-seed.ocvcoin.family.");
vSeeds.emplace_back("testnet-seed.ocvcoin.systems.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rentals.");
vSeeds.emplace_back("testnet-seed.ocvcoin.domains.");
vSeeds.emplace_back("testnet-seed.ocvcoin.consulting.");
vSeeds.emplace_back("testnet-seed.ocvcoin.buzz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sale.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.name.");
vSeeds.emplace_back("testnet-seed.ocvcoin.marketing.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ma.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pizza.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tel.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pub.");
vSeeds.emplace_back("testnet-seed.ocvcoin.photography.");
vSeeds.emplace_back("testnet-seed.ocvcoin.so.");
vSeeds.emplace_back("testnet-seed.ocvcoin.training.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cafe.");
vSeeds.emplace_back("testnet-seed.ocvcoin.expert.");
vSeeds.emplace_back("testnet-seed.ocvcoin.no.");
vSeeds.emplace_back("testnet-seed.ocvcoin.recipes.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cheap.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cool.");
vSeeds.emplace_back("testnet-seed.ocvcoin.directory.");
vSeeds.emplace_back("testnet-seed.ocvcoin.audio.");
vSeeds.emplace_back("testnet-seed.ocvcoin.blue.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cash.");
vSeeds.emplace_back("testnet-seed.ocvcoin.taxi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.casa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.engineering.");
vSeeds.emplace_back("testnet-seed.ocvcoin.coffee.");
vSeeds.emplace_back("testnet-seed.ocvcoin.properties.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.au.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tours.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ci.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.boutique.");
vSeeds.emplace_back("testnet-seed.ocvcoin.support.");
vSeeds.emplace_back("testnet-seed.ocvcoin.foundation.");
vSeeds.emplace_back("testnet-seed.ocvcoin.events.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fitness.");
vSeeds.emplace_back("testnet-seed.ocvcoin.church.");
vSeeds.emplace_back("testnet-seed.ocvcoin.capital.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gold.");
vSeeds.emplace_back("testnet-seed.ocvcoin.delivery.");
vSeeds.emplace_back("testnet-seed.ocvcoin.institute.");
vSeeds.emplace_back("testnet-seed.ocvcoin.af.");
vSeeds.emplace_back("testnet-seed.ocvcoin.software.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.in.");
vSeeds.emplace_back("testnet-seed.ocvcoin.reviews.");
vSeeds.emplace_back("testnet-seed.ocvcoin.edu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.watch.");
vSeeds.emplace_back("testnet-seed.ocvcoin.webcam.");
vSeeds.emplace_back("testnet-seed.ocvcoin.love.");
vSeeds.emplace_back("testnet-seed.ocvcoin.auction.");
vSeeds.emplace_back("testnet-seed.ocvcoin.toys.");
vSeeds.emplace_back("testnet-seed.ocvcoin.legal.");
vSeeds.emplace_back("testnet-seed.ocvcoin.express.");
vSeeds.emplace_back("testnet-seed.ocvcoin.partners.");
vSeeds.emplace_back("testnet-seed.ocvcoin.si.");
vSeeds.emplace_back("testnet-seed.ocvcoin.coach.");
vSeeds.emplace_back("testnet-seed.ocvcoin.style.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pp.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.date.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.town.");
vSeeds.emplace_back("testnet-seed.ocvcoin.energy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.report.");
vSeeds.emplace_back("testnet-seed.ocvcoin.photos.");
vSeeds.emplace_back("testnet-seed.ocvcoin.inc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.loan.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ventures.");
vSeeds.emplace_back("testnet-seed.ocvcoin.computer.");
vSeeds.emplace_back("testnet-seed.ocvcoin.enterprises.");
vSeeds.emplace_back("testnet-seed.ocvcoin.restaurant.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fund.");
vSeeds.emplace_back("testnet-seed.ocvcoin.chat.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rip.");
vSeeds.emplace_back("testnet-seed.ocvcoin.help.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.as.");
vSeeds.emplace_back("testnet-seed.ocvcoin.credit.");
vSeeds.emplace_back("testnet-seed.ocvcoin.racing.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gay.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cricket.");
vSeeds.emplace_back("testnet-seed.ocvcoin.investments.");
vSeeds.emplace_back("testnet-seed.ocvcoin.band.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ag.");
vSeeds.emplace_back("testnet-seed.ocvcoin.et.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bike.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dating.");
vSeeds.emplace_back("testnet-seed.ocvcoin.press.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vin.");
vSeeds.emplace_back("testnet-seed.ocvcoin.repair.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ml.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fyi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.casino.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vet.");
vSeeds.emplace_back("testnet-seed.ocvcoin.yoga.");
vSeeds.emplace_back("testnet-seed.ocvcoin.land.");
vSeeds.emplace_back("testnet-seed.ocvcoin.page.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ax.");
vSeeds.emplace_back("testnet-seed.ocvcoin.shoes.");
vSeeds.emplace_back("testnet-seed.ocvcoin.show.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cyou.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.coupons.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kim.");
vSeeds.emplace_back("testnet-seed.ocvcoin.deals.");
vSeeds.emplace_back("testnet-seed.ocvcoin.guide.");
vSeeds.emplace_back("testnet-seed.ocvcoin.black.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rest.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pe.");
vSeeds.emplace_back("testnet-seed.ocvcoin.estate.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bargains.");
vSeeds.emplace_back("testnet-seed.ocvcoin.accountants.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tax.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wedding.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dance.");
vSeeds.emplace_back("testnet-seed.ocvcoin.productions.");
vSeeds.emplace_back("testnet-seed.ocvcoin.financial.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kitchen.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.direct.");
vSeeds.emplace_back("testnet-seed.ocvcoin.care.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sex.");
vSeeds.emplace_back("testnet-seed.ocvcoin.promo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.moda.");
vSeeds.emplace_back("testnet-seed.ocvcoin.golf.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tips.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sexy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.game.");
vSeeds.emplace_back("testnet-seed.ocvcoin.haus.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tf.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wine.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pet.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gift.");
vSeeds.emplace_back("testnet-seed.ocvcoin.blackfriday.");
vSeeds.emplace_back("testnet-seed.ocvcoin.school.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tennis.");
vSeeds.emplace_back("testnet-seed.ocvcoin.md.");
vSeeds.emplace_back("testnet-seed.ocvcoin.monster.");
vSeeds.emplace_back("testnet-seed.ocvcoin.exchange.");
vSeeds.emplace_back("testnet-seed.ocvcoin.video.");
vSeeds.emplace_back("testnet-seed.ocvcoin.forsale.");
vSeeds.emplace_back("testnet-seed.ocvcoin.photo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.actor.");
vSeeds.emplace_back("testnet-seed.ocvcoin.community.");
vSeeds.emplace_back("testnet-seed.ocvcoin.accountant.");
vSeeds.emplace_back("testnet-seed.ocvcoin.movie.");
vSeeds.emplace_back("testnet-seed.ocvcoin.yt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ge.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cam.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ga.");
vSeeds.emplace_back("testnet-seed.ocvcoin.parts.");
vSeeds.emplace_back("testnet-seed.ocvcoin.education.");
vSeeds.emplace_back("testnet-seed.ocvcoin.faith.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hosting.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bingo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.engineer.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bet.");
vSeeds.emplace_back("testnet-seed.ocvcoin.associates.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ski.");
vSeeds.emplace_back("testnet-seed.ocvcoin.singles.");
vSeeds.emplace_back("testnet-seed.ocvcoin.farm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pink.");
vSeeds.emplace_back("testnet-seed.ocvcoin.claims.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pics.");
vSeeds.emplace_back("testnet-seed.ocvcoin.careers.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nyc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.menu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.house.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ad.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.uno.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cards.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tube.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.co.");
vSeeds.emplace_back("testnet-seed.ocvcoin.doctor.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ar.");
vSeeds.emplace_back("testnet-seed.ocvcoin.study.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rent.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vision.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mortgage.");
vSeeds.emplace_back("testnet-seed.ocvcoin.shopping.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gratis.");
vSeeds.emplace_back("testnet-seed.ocvcoin.apartments.");
vSeeds.emplace_back("testnet-seed.ocvcoin.army.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fit.");
vSeeds.emplace_back("testnet-seed.ocvcoin.college.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aaa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.builders.");
vSeeds.emplace_back("testnet-seed.ocvcoin.beer.");
vSeeds.emplace_back("testnet-seed.ocvcoin.irish.");
vSeeds.emplace_back("testnet-seed.ocvcoin.clinic.");
vSeeds.emplace_back("testnet-seed.ocvcoin.law.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gives.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ke.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fish.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sucks.");
vSeeds.emplace_back("testnet-seed.ocvcoin.country.");
vSeeds.emplace_back("testnet-seed.ocvcoin.porn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.camera.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sarl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.salon.");
vSeeds.emplace_back("testnet-seed.ocvcoin.security.");
vSeeds.emplace_back("testnet-seed.ocvcoin.attorney.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pictures.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.solar.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mom.");
vSeeds.emplace_back("testnet-seed.ocvcoin.equipment.");
vSeeds.emplace_back("testnet-seed.ocvcoin.charity.");
vSeeds.emplace_back("testnet-seed.ocvcoin.viajes.");
vSeeds.emplace_back("testnet-seed.ocvcoin.christmas.");
vSeeds.emplace_back("testnet-seed.ocvcoin.na.");
vSeeds.emplace_back("testnet-seed.ocvcoin.discount.");
vSeeds.emplace_back("testnet-seed.ocvcoin.boats.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ph.");
vSeeds.emplace_back("testnet-seed.ocvcoin.feedback.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.creditcard.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vegas.");
vSeeds.emplace_back("testnet-seed.ocvcoin.baby.");
vSeeds.emplace_back("testnet-seed.ocvcoin.clothing.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cab.");
vSeeds.emplace_back("testnet-seed.ocvcoin.courses.");
vSeeds.emplace_back("testnet-seed.ocvcoin.camp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.villas.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fail.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rodeo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.london.");
vSeeds.emplace_back("testnet-seed.ocvcoin.earth.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lgbt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ing.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jobs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.eco.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ht.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.uk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aero.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cf.");
vSeeds.emplace_back("testnet-seed.ocvcoin.management.");
vSeeds.emplace_back("testnet-seed.ocvcoin.berlin.");
vSeeds.emplace_back("testnet-seed.ocvcoin.voyage.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tienda.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.reisen.");
vSeeds.emplace_back("testnet-seed.ocvcoin.construction.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.schule.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ceo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.by.");
vSeeds.emplace_back("testnet-seed.ocvcoin.green.");
vSeeds.emplace_back("testnet-seed.ocvcoin.car.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cleaning.");
vSeeds.emplace_back("testnet-seed.ocvcoin.immo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sx.");
vSeeds.emplace_back("testnet-seed.ocvcoin.property.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sb.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gallery.");
vSeeds.emplace_back("testnet-seed.ocvcoin.adult.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wf.");
vSeeds.emplace_back("testnet-seed.ocvcoin.je.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.build.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vacations.");
vSeeds.emplace_back("testnet-seed.ocvcoin.furniture.");
vSeeds.emplace_back("testnet-seed.ocvcoin.qa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.catering.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fashion.");
vSeeds.emplace_back("testnet-seed.ocvcoin.supplies.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.th.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lawyer.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cruises.");
vSeeds.emplace_back("testnet-seed.ocvcoin.diamonds.");
vSeeds.emplace_back("testnet-seed.ocvcoin.holiday.");
vSeeds.emplace_back("testnet-seed.ocvcoin.holdings.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ba.");
vSeeds.emplace_back("testnet-seed.ocvcoin.exposed.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tires.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ki.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.place.");
vSeeds.emplace_back("testnet-seed.ocvcoin.healthcare.");
vSeeds.emplace_back("testnet-seed.ocvcoin.my.");
vSeeds.emplace_back("testnet-seed.ocvcoin.homes.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tokyo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.airforce.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.za.");
vSeeds.emplace_back("testnet-seed.ocvcoin.observer.");
vSeeds.emplace_back("testnet-seed.ocvcoin.graphics.");
vSeeds.emplace_back("testnet-seed.ocvcoin.archi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gd.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gdn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.futbol.");
vSeeds.emplace_back("testnet-seed.ocvcoin.poker.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mba.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cooking.");
vSeeds.emplace_back("testnet-seed.ocvcoin.auto.");
vSeeds.emplace_back("testnet-seed.ocvcoin.om.");
vSeeds.emplace_back("testnet-seed.ocvcoin.onl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sbs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.garden.");
vSeeds.emplace_back("testnet-seed.ocvcoin.immobilien.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dentist.");
vSeeds.emplace_back("testnet-seed.ocvcoin.surf.");
vSeeds.emplace_back("testnet-seed.ocvcoin.au.");
vSeeds.emplace_back("testnet-seed.ocvcoin.republican.");
vSeeds.emplace_back("testnet-seed.ocvcoin.africa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.flights.");
vSeeds.emplace_back("testnet-seed.ocvcoin.glass.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wang.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dental.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vodka.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jewelry.");
vSeeds.emplace_back("testnet-seed.ocvcoin.contractors.");
vSeeds.emplace_back("testnet-seed.ocvcoin.condos.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.miami.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ir.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dj.");
vSeeds.emplace_back("testnet-seed.ocvcoin.football.");
vSeeds.emplace_back("testnet-seed.ocvcoin.desi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tattoo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lease.");
vSeeds.emplace_back("testnet-seed.ocvcoin.industries.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fans.");
vSeeds.emplace_back("testnet-seed.ocvcoin.limited.");
vSeeds.emplace_back("testnet-seed.ocvcoin.degree.");
vSeeds.emplace_back("testnet-seed.ocvcoin.supply.");
vSeeds.emplace_back("testnet-seed.ocvcoin.llc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vote.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ec.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cars.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ist.");
vSeeds.emplace_back("testnet-seed.ocvcoin.horse.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ps.");
vSeeds.emplace_back("testnet-seed.ocvcoin.loans.");
vSeeds.emplace_back("testnet-seed.ocvcoin.democrat.");
vSeeds.emplace_back("testnet-seed.ocvcoin.how.");
vSeeds.emplace_back("testnet-seed.ocvcoin.maison.");
vSeeds.emplace_back("testnet-seed.ocvcoin.theater.");
vSeeds.emplace_back("testnet-seed.ocvcoin.scot.");
vSeeds.emplace_back("testnet-seed.ocvcoin.flowers.");
vSeeds.emplace_back("testnet-seed.ocvcoin.soccer.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rehab.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.limo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.health.");
vSeeds.emplace_back("testnet-seed.ocvcoin.realty.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bb.");
vSeeds.emplace_back("testnet-seed.ocvcoin.surgery.");
vSeeds.emplace_back("testnet-seed.ocvcoin.film.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cd.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ky.");
vSeeds.emplace_back("testnet-seed.ocvcoin.soy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.florist.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ngo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.quebec.");
vSeeds.emplace_back("testnet-seed.ocvcoin.memorial.");
vSeeds.emplace_back("testnet-seed.ocvcoin.shiksha.");
vSeeds.emplace_back("testnet-seed.ocvcoin.protection.");
vSeeds.emplace_back("testnet-seed.ocvcoin.diet.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fan.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.mx.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gripe.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.tr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.new.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gifts.");
vSeeds.emplace_back("testnet-seed.ocvcoin.zip.");
vSeeds.emplace_back("testnet-seed.ocvcoin.my.id.");
vSeeds.emplace_back("testnet-seed.ocvcoin.compare.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.plumbing.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cfd.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kiwi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.me.uk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.day.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.nz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.az.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bayern.");
vSeeds.emplace_back("testnet-seed.ocvcoin.theatre.");
vSeeds.emplace_back("testnet-seed.ocvcoin.voto.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ls.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hockey.");
vSeeds.emplace_back("testnet-seed.ocvcoin.luxury.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lighting.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gmbh.");
vSeeds.emplace_back("testnet-seed.ocvcoin.td.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fishing.");
vSeeds.emplace_back("testnet-seed.ocvcoin.boston.");
vSeeds.emplace_back("testnet-seed.ocvcoin.reise.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rich.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jetzt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.insure.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ug.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ni.");
vSeeds.emplace_back("testnet-seed.ocvcoin.box.");
vSeeds.emplace_back("testnet-seed.ocvcoin.organic.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hiphop.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cymru.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.de.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hospital.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ao.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lat.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bank.");
vSeeds.emplace_back("testnet-seed.ocvcoin.yokohama.");
vSeeds.emplace_back("testnet-seed.ocvcoin.guitars.");
vSeeds.emplace_back("testnet-seed.ocvcoin.paris.");
vSeeds.emplace_back("testnet-seed.ocvcoin.barcelona.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kaufen.");
vSeeds.emplace_back("testnet-seed.ocvcoin.qpon.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.storage.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.il.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ru.");
vSeeds.emplace_back("testnet-seed.ocvcoin.edu.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.broker.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tickets.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bj.");
vSeeds.emplace_back("testnet-seed.ocvcoin.autos.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.co.");
vSeeds.emplace_back("testnet-seed.ocvcoin.career.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nf.");
vSeeds.emplace_back("testnet-seed.ocvcoin.trading.");
vSeeds.emplace_back("testnet-seed.ocvcoin.abogado.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aq.");
vSeeds.emplace_back("testnet-seed.ocvcoin.amsterdam.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bond.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.contact.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.in.");
vSeeds.emplace_back("testnet-seed.ocvcoin.navy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.capetown.");
vSeeds.emplace_back("testnet-seed.ocvcoin.channel.");
vSeeds.emplace_back("testnet-seed.ocvcoin.markets.");
vSeeds.emplace_back("testnet-seed.ocvcoin.forum.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bible.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bh.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.voting.");
vSeeds.emplace_back("testnet-seed.ocvcoin.juegos.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.iq.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wien.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wales.");
vSeeds.emplace_back("testnet-seed.ocvcoin.srl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.in.");
vSeeds.emplace_back("testnet-seed.ocvcoin.coop.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nagoya.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.radio.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.melbourne.");
vSeeds.emplace_back("testnet-seed.ocvcoin.brussels.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ar.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.uz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ve.");
vSeeds.emplace_back("testnet-seed.ocvcoin.luxe.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gq.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nrw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.tw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bf.");
vSeeds.emplace_back("testnet-seed.ocvcoin.realestate.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cologne.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ag.");
vSeeds.emplace_back("testnet-seed.ocvcoin.uy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.boo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hamburg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.meme.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ong.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bzh.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.ag.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ss.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ltda.");
vSeeds.emplace_back("testnet-seed.ocvcoin.quest.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.es.");
vSeeds.emplace_back("testnet-seed.ocvcoin.physio.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.eus.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vlaanderen.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sport.");
vSeeds.emplace_back("testnet-seed.ocvcoin.durban.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hiv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gf.");
vSeeds.emplace_back("testnet-seed.ocvcoin.saarland.");
vSeeds.emplace_back("testnet-seed.ocvcoin.eg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ye.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ruhr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ryukyu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sydney.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.sg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.koeln.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.bo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.py.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.at.");
vSeeds.emplace_back("testnet-seed.ocvcoin.osaka.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gal.");
vSeeds.emplace_back("testnet-seed.ocvcoin.or.at.");
vSeeds.emplace_back("testnet-seed.ocvcoin.now.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tj.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.swiss.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.vc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.id.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rio.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.forex.");
vSeeds.emplace_back("testnet-seed.ocvcoin.food.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ren.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.nz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.pt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.il.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tirol.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.okinawa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.nz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.af.");
vSeeds.emplace_back("testnet-seed.ocvcoin.info.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.foo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.museum.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gen.in.");
vSeeds.emplace_back("testnet-seed.ocvcoin.alsace.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.hk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sd.");
vSeeds.emplace_back("testnet-seed.ocvcoin.trust.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.int.");
vSeeds.emplace_back("testnet-seed.ocvcoin.joburg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.makeup.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.firm.in.");
vSeeds.emplace_back("testnet-seed.ocvcoin.music.");
vSeeds.emplace_back("testnet-seed.ocvcoin.yachts.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.es.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gent.");
vSeeds.emplace_back("testnet-seed.ocvcoin.data.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.py.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.ru.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ni.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.bz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.sa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ind.in.");
vSeeds.emplace_back("testnet-seed.ocvcoin.beauty.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.th.");
vSeeds.emplace_back("testnet-seed.ocvcoin.motorcycles.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.cr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.insurance.");
vSeeds.emplace_back("testnet-seed.ocvcoin.basketball.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.pe.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.abc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dad.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.sc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ph.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ec.");
vSeeds.emplace_back("testnet-seed.ocvcoin.istanbul.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.jp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.cm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.frl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.ke.");
vSeeds.emplace_back("testnet-seed.ocvcoin.in.th.");
vSeeds.emplace_back("testnet-seed.ocvcoin.whoswho.");
vSeeds.emplace_back("testnet-seed.ocvcoin.free.");
vSeeds.emplace_back("testnet-seed.ocvcoin.web.tr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.ru.");
vSeeds.emplace_back("testnet-seed.ocvcoin.versicherung.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.sb.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.you.");
vSeeds.emplace_back("testnet-seed.ocvcoin.taipei.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ng.");
vSeeds.emplace_back("testnet-seed.ocvcoin.moscow.");
vSeeds.emplace_back("testnet-seed.ocvcoin.biz.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.nl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.spa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.book.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gh.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.kr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lotto.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.je.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.ph.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.za.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.pr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nexus.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ve.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.hr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.cy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.id.au.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.za.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kids.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.il.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.gg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ro.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mq.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dealer.");
vSeeds.emplace_back("testnet-seed.ocvcoin.skin.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pf.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kyoto.");
vSeeds.emplace_back("testnet-seed.ocvcoin.google.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.gy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.my.");
vSeeds.emplace_back("testnet-seed.ocvcoin.idv.tw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.do.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.gt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.pa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.phd.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ai.");
vSeeds.emplace_back("testnet-seed.ocvcoin.corsica.");
vSeeds.emplace_back("testnet-seed.ocvcoin.meet.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gov.");
vSeeds.emplace_back("testnet-seed.ocvcoin.reit.");
vSeeds.emplace_back("testnet-seed.ocvcoin.krd.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.ve.");
vSeeds.emplace_back("testnet-seed.ocvcoin.madrid.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.au.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.lc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.mx.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.im.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.rs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.ph.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mov.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ltd.uk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.case.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.pk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rsvp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.au.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.uy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.man.");
vSeeds.emplace_back("testnet-seed.ocvcoin.play.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dot.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.pe.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ly.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.pe.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.gh.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pay.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hair.");
vSeeds.emplace_back("testnet-seed.ocvcoin.moi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.mt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.biz.id.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kiwi.nz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aarp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.edu.rs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.so.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.sv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.gr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ht.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xin.");
vSeeds.emplace_back("testnet-seed.ocvcoin.med.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.hu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.zuerich.");
vSeeds.emplace_back("testnet-seed.ocvcoin.giving.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gop.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.ao.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.bi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ee.");
vSeeds.emplace_back("testnet-seed.ocvcoin.apple.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gen.tr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pharmacy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.lv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wow.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rugby.");
vSeeds.emplace_back("testnet-seed.ocvcoin.msk.ru.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.tw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.zero.");
vSeeds.emplace_back("testnet-seed.ocvcoin.biz.tr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.va.");
vSeeds.emplace_back("testnet-seed.ocvcoin.select.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.vn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.prof.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.tt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.so.");
vSeeds.emplace_back("testnet-seed.ocvcoin.waw.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.pk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.here.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.my.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.vn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.bh.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.ug.");
vSeeds.emplace_back("testnet-seed.ocvcoin.web.id.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.tz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.or.kr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.nf.");
vSeeds.emplace_back("testnet-seed.ocvcoin.esq.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ms.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.by.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.tn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.pk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.my.");
vSeeds.emplace_back("testnet-seed.ocvcoin.able.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ads.");
vSeeds.emplace_back("testnet-seed.ocvcoin.eat.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.mk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tv.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.info.tr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.in.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ac.nz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kiev.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ice.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cpa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.eg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.ma.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.lb.");
vSeeds.emplace_back("testnet-seed.ocvcoin.spb.ru.");
vSeeds.emplace_back("testnet-seed.ocvcoin.map.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.kz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.amazon.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gen.nz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.geek.nz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.fj.");
vSeeds.emplace_back("testnet-seed.ocvcoin.watches.");
vSeeds.emplace_back("testnet-seed.ocvcoin.km.");
vSeeds.emplace_back("testnet-seed.ocvcoin.edu.vn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.mz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.kw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.abbott.");
vSeeds.emplace_back("testnet-seed.ocvcoin.diy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tatar.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.gp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.na.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fox.");
vSeeds.emplace_back("testnet-seed.ocvcoin.search.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ind.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.arpa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.open.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fly.");
vSeeds.emplace_back("testnet-seed.ocvcoin.eco.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.living.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lifestyle.");
vSeeds.emplace_back("testnet-seed.ocvcoin.secure.");
vSeeds.emplace_back("testnet-seed.ocvcoin.abb.");
vSeeds.emplace_back("testnet-seed.ocvcoin.zm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sky.");
vSeeds.emplace_back("testnet-seed.ocvcoin.docs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.vi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.ar.");
vSeeds.emplace_back("testnet-seed.ocvcoin.idv.hk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ne.jp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.youtube.");
vSeeds.emplace_back("testnet-seed.ocvcoin.goo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.jo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.bw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.post.");
vSeeds.emplace_back("testnet-seed.ocvcoin.star.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.ls.");
vSeeds.emplace_back("testnet-seed.ocvcoin.school.nz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ge.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.lk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aws.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.gi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.name.tr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.zw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.creditunion.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.zw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sh.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.next.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.tj.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fj.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ping.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ci.");
vSeeds.emplace_back("testnet-seed.ocvcoin.analytics.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lb.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tvs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.zm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.sl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.buy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vana.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.lk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.hk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.abbvie.");
vSeeds.emplace_back("testnet-seed.ocvcoin.drive.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hot.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hk.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tw.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.talk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.or.jp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mobile.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ks.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.room.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pro.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fast.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wiki.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.java.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.tr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.office.");
vSeeds.emplace_back("testnet-seed.ocvcoin.art.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.abudhabi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bentley.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cal.");
vSeeds.emplace_back("testnet-seed.ocvcoin.smart.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gmail.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.et.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bj.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bbc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hi.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ott.");
vSeeds.emplace_back("testnet-seed.ocvcoin.prod.");
vSeeds.emplace_back("testnet-seed.ocvcoin.android.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gle.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ntt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gb.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.qa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.joy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.re.kr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.ng.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ally.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lds.");
vSeeds.emplace_back("testnet-seed.ocvcoin.safe.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.ai.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fire.");
vSeeds.emplace_back("testnet-seed.ocvcoin.org.gr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mil.");
vSeeds.emplace_back("testnet-seed.ocvcoin.azure.");
vSeeds.emplace_back("testnet-seed.ocvcoin.catholic.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ups.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nieruchomosci.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.read.");
vSeeds.emplace_back("testnet-seed.ocvcoin.netflix.");
vSeeds.emplace_back("testnet-seed.ocvcoin.deal.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hotels.");
vSeeds.emplace_back("testnet-seed.ocvcoin.anz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pioneer.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.ai.");
vSeeds.emplace_back("testnet-seed.ocvcoin.warszawa.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.if.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.spot.");
vSeeds.emplace_back("testnet-seed.ocvcoin.accenture.");
vSeeds.emplace_back("testnet-seed.ocvcoin.alibaba.");
vSeeds.emplace_back("testnet-seed.ocvcoin.americanexpress.");
vSeeds.emplace_back("testnet-seed.ocvcoin.circle.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cv.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gmo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pe.kr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.off.ai.");
vSeeds.emplace_back("testnet-seed.ocvcoin.arte.");
vSeeds.emplace_back("testnet-seed.ocvcoin.arab.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nowruz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ieee.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sklep.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.realtor.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.phone.");
vSeeds.emplace_back("testnet-seed.ocvcoin.call.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wroclaw.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mh.");
vSeeds.emplace_back("testnet-seed.ocvcoin.shell.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sn.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.ps.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xbox.");
vSeeds.emplace_back("testnet-seed.ocvcoin.barefoot.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fj.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.com.nr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.george.");
vSeeds.emplace_back("testnet-seed.ocvcoin.goog.");
vSeeds.emplace_back("testnet-seed.ocvcoin.audi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.virgin.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nm.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.microsoft.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gx.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.edu.lk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.booking.");
vSeeds.emplace_back("testnet-seed.ocvcoin.airbus.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mango.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mo.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aco.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bmw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.song.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sx.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gz.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aol.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rec.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.like.");
vSeeds.emplace_back("testnet-seed.ocvcoin.redstone.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wed.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bestbuy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.yandex.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hn.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gd.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gs.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.js.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.yn.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.obi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.srv.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dubai.");
vSeeds.emplace_back("testnet-seed.ocvcoin.omega.");
vSeeds.emplace_back("testnet-seed.ocvcoin.save.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hl.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.esp.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.author.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tab.");
vSeeds.emplace_back("testnet-seed.ocvcoin.godaddy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mini.");
vSeeds.emplace_back("testnet-seed.ocvcoin.windows.");
vSeeds.emplace_back("testnet-seed.ocvcoin.yun.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nico.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mint.");
vSeeds.emplace_back("testnet-seed.ocvcoin.or.id.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dds.");
vSeeds.emplace_back("testnet-seed.ocvcoin.beats.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ggee.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cq.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gucci.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bofa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.smile.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sj.");
vSeeds.emplace_back("testnet-seed.ocvcoin.prime.");
vSeeds.emplace_back("testnet-seed.ocvcoin.amex.");
vSeeds.emplace_back("testnet-seed.ocvcoin.imb.br.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ollo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.orange.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aetna.");
vSeeds.emplace_back("testnet-seed.ocvcoin.got.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pid.");
vSeeds.emplace_back("testnet-seed.ocvcoin.walmart.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ac.cn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.seven.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pin.");
vSeeds.emplace_back("testnet-seed.ocvcoin.comsec.");
vSeeds.emplace_back("testnet-seed.ocvcoin.farmers.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tokyo.jp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.afl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.calvinklein.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kosher.");
vSeeds.emplace_back("testnet-seed.ocvcoin.asda.");
vSeeds.emplace_back("testnet-seed.ocvcoin.eurovision.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bauhaus.");
vSeeds.emplace_back("testnet-seed.ocvcoin.discover.");
vSeeds.emplace_back("testnet-seed.ocvcoin.chrome.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wtc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.brother.");
vSeeds.emplace_back("testnet-seed.ocvcoin.locker.");
vSeeds.emplace_back("testnet-seed.ocvcoin.moto.");
vSeeds.emplace_back("testnet-seed.ocvcoin.name.my.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bom.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lego.");
vSeeds.emplace_back("testnet-seed.ocvcoin.axa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cern.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ibm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tui.");
vSeeds.emplace_back("testnet-seed.ocvcoin.uol.");
vSeeds.emplace_back("testnet-seed.ocvcoin.safety.");
vSeeds.emplace_back("testnet-seed.ocvcoin.alipay.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kred.");
vSeeds.emplace_back("testnet-seed.ocvcoin.williamhill.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tkmaxx.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lidl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.weather.");
vSeeds.emplace_back("testnet-seed.ocvcoin.yahoo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bradesco.");
vSeeds.emplace_back("testnet-seed.ocvcoin.seek.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mit.");
vSeeds.emplace_back("testnet-seed.ocvcoin.skype.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dhl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.agakhan.");
vSeeds.emplace_back("testnet-seed.ocvcoin.zara.");
vSeeds.emplace_back("testnet-seed.ocvcoin.americanfamily.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bloomberg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.playstation.");
vSeeds.emplace_back("testnet-seed.ocvcoin.canon.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ftr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jio.");
vSeeds.emplace_back("testnet-seed.ocvcoin.viking.");
vSeeds.emplace_back("testnet-seed.ocvcoin.baseball.");
vSeeds.emplace_back("testnet-seed.ocvcoin.blockbuster.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bnpparibas.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aeg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hangout.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sbi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.schwarz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vanguard.");
vSeeds.emplace_back("testnet-seed.ocvcoin.visa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.edeka.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lamborghini.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mma.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sakura.");
vSeeds.emplace_back("testnet-seed.ocvcoin.globo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sas.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aig.");
vSeeds.emplace_back("testnet-seed.ocvcoin.llp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.airtel.");
vSeeds.emplace_back("testnet-seed.ocvcoin.baidu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.crown.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ferrari.");
vSeeds.emplace_back("testnet-seed.ocvcoin.travelersinsurance.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tunes.");
vSeeds.emplace_back("testnet-seed.ocvcoin.extraspace.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hotmail.");
vSeeds.emplace_back("testnet-seed.ocvcoin.coupon.");
vSeeds.emplace_back("testnet-seed.ocvcoin.barclays.");
vSeeds.emplace_back("testnet-seed.ocvcoin.delta.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bing.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cruise.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aquarelle.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gmx.");
vSeeds.emplace_back("testnet-seed.ocvcoin.seat.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ubs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sap.");
vSeeds.emplace_back("testnet-seed.ocvcoin.latino.");
vSeeds.emplace_back("testnet-seed.ocvcoin.viva.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bcn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.final.");
vSeeds.emplace_back("testnet-seed.ocvcoin.unicom.");
vSeeds.emplace_back("testnet-seed.ocvcoin.samsung.");
vSeeds.emplace_back("testnet-seed.ocvcoin.origins.");
vSeeds.emplace_back("testnet-seed.ocvcoin.erni.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pwc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ericsson.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mormon.");
vSeeds.emplace_back("testnet-seed.ocvcoin.weber.");
vSeeds.emplace_back("testnet-seed.ocvcoin.citadel.");
vSeeds.emplace_back("testnet-seed.ocvcoin.deloitte.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cba.");
vSeeds.emplace_back("testnet-seed.ocvcoin.total.");
vSeeds.emplace_back("testnet-seed.ocvcoin.imdb.");
vSeeds.emplace_back("testnet-seed.ocvcoin.auspost.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sony.");
vSeeds.emplace_back("testnet-seed.ocvcoin.chanel.");
vSeeds.emplace_back("testnet-seed.ocvcoin.oracle.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hermes.");
vSeeds.emplace_back("testnet-seed.ocvcoin.homedepot.");
vSeeds.emplace_back("testnet-seed.ocvcoin.infiniti.");
vSeeds.emplace_back("testnet-seed.ocvcoin.datsun.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gap.");
vSeeds.emplace_back("testnet-seed.ocvcoin.itv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.natura.");
vSeeds.emplace_back("testnet-seed.ocvcoin.toyota.");
vSeeds.emplace_back("testnet-seed.ocvcoin.audible.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cisco.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sncf.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cbn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fage.");
vSeeds.emplace_back("testnet-seed.ocvcoin.leclerc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pfizer.");
vSeeds.emplace_back("testnet-seed.ocvcoin.stockholm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.winners.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jpmorgan.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kpmg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.schmidt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.akdn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.helsinki.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jmp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.crs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.weibo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bharti.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sharp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nike.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vivo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.walter.");
vSeeds.emplace_back("testnet-seed.ocvcoin.weir.");
vSeeds.emplace_back("testnet-seed.ocvcoin.itau.");
vSeeds.emplace_back("testnet-seed.ocvcoin.allstate.");
vSeeds.emplace_back("testnet-seed.ocvcoin.aramco.");
vSeeds.emplace_back("testnet-seed.ocvcoin.broadway.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bridgestone.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hsbc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.travelers.");
vSeeds.emplace_back("testnet-seed.ocvcoin.capitalone.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lamer.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ipiranga.");
vSeeds.emplace_back("testnet-seed.ocvcoin.honda.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lilly.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ikano.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ismaili.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nokia.");
vSeeds.emplace_back("testnet-seed.ocvcoin.yodobashi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jaguar.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bosch.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dell.");
vSeeds.emplace_back("testnet-seed.ocvcoin.target.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xerox.");
vSeeds.emplace_back("testnet-seed.ocvcoin.barclaycard.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bms.");
vSeeds.emplace_back("testnet-seed.ocvcoin.chase.");
vSeeds.emplace_back("testnet-seed.ocvcoin.epson.");
vSeeds.emplace_back("testnet-seed.ocvcoin.saxo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.shia.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nra.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dnp.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fido.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tdk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dish.");
vSeeds.emplace_back("testnet-seed.ocvcoin.frontier.");
vSeeds.emplace_back("testnet-seed.ocvcoin.icbc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kia.");
vSeeds.emplace_back("testnet-seed.ocvcoin.politie.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sew.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jeep.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ril.");
vSeeds.emplace_back("testnet-seed.ocvcoin.zappos.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cuisinella.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dvr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sfr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.stc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.anquan.");
vSeeds.emplace_back("testnet-seed.ocvcoin.caravan.");
vSeeds.emplace_back("testnet-seed.ocvcoin.juniper.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lifeinsurance.");
vSeeds.emplace_back("testnet-seed.ocvcoin.monash.");
vSeeds.emplace_back("testnet-seed.ocvcoin.neustar.");
vSeeds.emplace_back("testnet-seed.ocvcoin.clubmed.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dabur.");
vSeeds.emplace_back("testnet-seed.ocvcoin.frogans.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kpn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lotte.");
vSeeds.emplace_back("testnet-seed.ocvcoin.netbank.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gea.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wme.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hitachi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hyundai.");
vSeeds.emplace_back("testnet-seed.ocvcoin.praxi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.weatherchannel.");
vSeeds.emplace_back("testnet-seed.ocvcoin.amica.");
vSeeds.emplace_back("testnet-seed.ocvcoin.commbank.");
vSeeds.emplace_back("testnet-seed.ocvcoin.otsuka.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tjmaxx.");
vSeeds.emplace_back("testnet-seed.ocvcoin.woodside.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.tw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mls.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mtn.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nissan.");
vSeeds.emplace_back("testnet-seed.ocvcoin.shangrila.");
vSeeds.emplace_back("testnet-seed.ocvcoin.trv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.volvo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hughes.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kerryhotels.");
vSeeds.emplace_back("testnet-seed.ocvcoin.marriott.");
vSeeds.emplace_back("testnet-seed.ocvcoin.suzuki.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cipriani.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dvag.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ferrero.");
vSeeds.emplace_back("testnet-seed.ocvcoin.samsclub.");
vSeeds.emplace_back("testnet-seed.ocvcoin.statefarm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.thd.");
vSeeds.emplace_back("testnet-seed.ocvcoin.maif.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nec.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dclk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.locus.");
vSeeds.emplace_back("testnet-seed.ocvcoin.progressive.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lexus.");
vSeeds.emplace_back("testnet-seed.ocvcoin.flickr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.imamat.");
vSeeds.emplace_back("testnet-seed.ocvcoin.silk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wanggou.");
vSeeds.emplace_back("testnet-seed.ocvcoin.citi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.grocery.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lpl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sandvikcoromant.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jot.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jprs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sina.");
vSeeds.emplace_back("testnet-seed.ocvcoin.athleta.");
vSeeds.emplace_back("testnet-seed.ocvcoin.banamex.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fujitsu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.scb.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sohu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.toray.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fedex.");
vSeeds.emplace_back("testnet-seed.ocvcoin.komatsu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.redumbrella.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tmall.");
vSeeds.emplace_back("testnet-seed.ocvcoin.verisign.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dtv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hkt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.taobao.");
vSeeds.emplace_back("testnet-seed.ocvcoin.wolterskluwer.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ifm.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kddi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lacaixa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nab.");
vSeeds.emplace_back("testnet-seed.ocvcoin.richardli.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lancaster.");
vSeeds.emplace_back("testnet-seed.ocvcoin.landrover.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sling.");
vSeeds.emplace_back("testnet-seed.ocvcoin.alstom.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kuokgroup.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pars.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pohl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xihuan.");
vSeeds.emplace_back("testnet-seed.ocvcoin.allfinanz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.panasonic.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bbva.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ford.");
vSeeds.emplace_back("testnet-seed.ocvcoin.schaeffler.");
vSeeds.emplace_back("testnet-seed.ocvcoin.softbank.");
vSeeds.emplace_back("testnet-seed.ocvcoin.yamaxun.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hisamitsu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.boehringer.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kerryproperties.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nhk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.genting.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rwe.");
vSeeds.emplace_back("testnet-seed.ocvcoin.scholarships.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fidelity.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fresenius.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jcb.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gallo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.goodyear.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nfl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pnc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cfa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mckinsey.");
vSeeds.emplace_back("testnet-seed.ocvcoin.shaw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.citic.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dupont.");
vSeeds.emplace_back("testnet-seed.ocvcoin.fairwinds.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kindle.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mlb.");
vSeeds.emplace_back("testnet-seed.ocvcoin.chintai.");
vSeeds.emplace_back("testnet-seed.ocvcoin.emerck.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hbo.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pru.");
vSeeds.emplace_back("testnet-seed.ocvcoin.vig.");
vSeeds.emplace_back("testnet-seed.ocvcoin.msd.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pccw.");
vSeeds.emplace_back("testnet-seed.ocvcoin.philips.");
vSeeds.emplace_back("testnet-seed.ocvcoin.goldpoint.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jll.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lasalle.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tci.");
vSeeds.emplace_back("testnet-seed.ocvcoin.toshiba.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ubank.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gbiz.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rogers.");
vSeeds.emplace_back("testnet-seed.ocvcoin.dunlop.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hdfc.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cbre.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nba.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sanofi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mtr.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tatamotors.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lincoln.");
vSeeds.emplace_back("testnet-seed.ocvcoin.firestone.");
vSeeds.emplace_back("testnet-seed.ocvcoin.statebank.");
vSeeds.emplace_back("testnet-seed.ocvcoin.amfam.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mitsubishi.");
vSeeds.emplace_back("testnet-seed.ocvcoin.shouji.");
vSeeds.emplace_back("testnet-seed.ocvcoin.stada.");
vSeeds.emplace_back("testnet-seed.ocvcoin.temasek.");
vSeeds.emplace_back("testnet-seed.ocvcoin.rexroth.");
vSeeds.emplace_back("testnet-seed.ocvcoin.norton.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sandvik.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bcg.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hyatt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kfh.");
vSeeds.emplace_back("testnet-seed.ocvcoin.olayan.");
vSeeds.emplace_back("testnet-seed.ocvcoin.web.lk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.kerrylogistics.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pramerica.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sener.");
vSeeds.emplace_back("testnet-seed.ocvcoin.firmdale.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nowtv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.reliance.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bbt.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lipsy.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nikon.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nissay.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tjx.");
vSeeds.emplace_back("testnet-seed.ocvcoin.grainger.");
vSeeds.emplace_back("testnet-seed.ocvcoin.latrobe.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hotel.lk.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lundbeck.");
vSeeds.emplace_back("testnet-seed.ocvcoin.intuit.");
vSeeds.emplace_back("testnet-seed.ocvcoin.gallup.");
vSeeds.emplace_back("testnet-seed.ocvcoin.guge.");
vSeeds.emplace_back("testnet-seed.ocvcoin.nextdirect.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tushu.");
vSeeds.emplace_back("testnet-seed.ocvcoin.clinique.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lefrak.");
vSeeds.emplace_back("testnet-seed.ocvcoin.tiaa.");
vSeeds.emplace_back("testnet-seed.ocvcoin.jnj.");
vSeeds.emplace_back("testnet-seed.ocvcoin.swatch.");
vSeeds.emplace_back("testnet-seed.ocvcoin.homesense.");
vSeeds.emplace_back("testnet-seed.ocvcoin.teva.");
vSeeds.emplace_back("testnet-seed.ocvcoin.hdfcbank.");
vSeeds.emplace_back("testnet-seed.ocvcoin.prudential.");
vSeeds.emplace_back("testnet-seed.ocvcoin.mattel.");
vSeeds.emplace_back("testnet-seed.ocvcoin.staples.");
vSeeds.emplace_back("testnet-seed.ocvcoin.pictet.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ricoh.");
vSeeds.emplace_back("testnet-seed.ocvcoin.flir.");
vSeeds.emplace_back("testnet-seed.ocvcoin.marshalls.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lanxess.");
vSeeds.emplace_back("testnet-seed.ocvcoin.stcgroup.");
vSeeds.emplace_back("testnet-seed.ocvcoin.bostik.");
vSeeds.emplace_back("testnet-seed.ocvcoin.lplfinancial.");
vSeeds.emplace_back("testnet-seed.ocvcoin.homegoods.");
vSeeds.emplace_back("testnet-seed.ocvcoin.merckmsd.");
vSeeds.emplace_back("testnet-seed.ocvcoin.olayangroup.");
vSeeds.emplace_back("testnet-seed.ocvcoin.inf.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.cc.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--3ds443g.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--55qx5d.");
vSeeds.emplace_back("testnet-seed.ocvcoin.ltd.ua.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--6qq986b3xl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--fiqs8s.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--6frz82g.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--mk1bu44c.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--tckwe.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--vhquv.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--fiq228c5hs.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--q9jyb4c.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--t60b56a.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--5tzm5g.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--9dbq2a.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--czrs0t.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--rhqv96g.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--unup4y.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--fjq720a.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--vuq861b.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--ses554g.");
vSeeds.emplace_back("testnet-seed.ocvcoin.sch.id.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--io0a7i.");
vSeeds.emplace_back("testnet-seed.ocvcoin.net.id.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--czr694b.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--czru2d.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--hxt814e.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--3bst00m.");
vSeeds.emplace_back("testnet-seed.ocvcoin.co.pl.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--qxam.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--xhq521b.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--1qqw23a.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--otu796d.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--45q11c.");
vSeeds.emplace_back("testnet-seed.ocvcoin.xn--imr513n.");





        

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tb";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_test), std::end(chainparams_seed_test));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {0, uint256S("0x054165d4f666bf097b2461d1bd84b3eec64c336c30fcb5d809e83a6fea95c391")},
            }
        };

        m_assumeutxo_data = {
            
        };

        chainTxData = ChainTxData{
            
            .nTime    = 0,
            .nTxCount = 0,
            .dTxRate  = 0,
        };
    }
};

/**
 * Signet: test network with an additional consensus parameter (see BIP325).
 */
class SigNetParams : public CChainParams {
public:
    explicit SigNetParams(const SigNetOptions& options)
    {
        std::vector<uint8_t> bin;
        vSeeds.clear();

        if (!options.challenge) {
            bin = ParseHex("5121031D5D16B9EC1E69A63709C0743BE786C9F3DC8974991BDF7BA6EDB7200A747E1621031870715E79CBFE0A69B7AF0135E7300726A0FDA6B3EE032BDFC010CC5DB381AF52ae");
			
			vSeeds.emplace_back("signet-seed.ocvcoin.com.");



vSeeds.emplace_back("signet-seed.ocvcoin.net.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.");
vSeeds.emplace_back("signet-seed.ocvcoin.xyz.");
vSeeds.emplace_back("signet-seed.ocvcoin.io.");
vSeeds.emplace_back("signet-seed.ocvcoin.me.");
vSeeds.emplace_back("signet-seed.ocvcoin.info.");
vSeeds.emplace_back("signet-seed.ocvcoin.ai.");
vSeeds.emplace_back("signet-seed.ocvcoin.top.");
vSeeds.emplace_back("signet-seed.ocvcoin.in.");
vSeeds.emplace_back("signet-seed.ocvcoin.eu.");
vSeeds.emplace_back("signet-seed.ocvcoin.online.");
vSeeds.emplace_back("signet-seed.ocvcoin.us.");
vSeeds.emplace_back("signet-seed.ocvcoin.dev.");
vSeeds.emplace_back("signet-seed.ocvcoin.gg.");
vSeeds.emplace_back("signet-seed.ocvcoin.tech.");
vSeeds.emplace_back("signet-seed.ocvcoin.biz.");
vSeeds.emplace_back("signet-seed.ocvcoin.cc.");
vSeeds.emplace_back("signet-seed.ocvcoin.de.");
vSeeds.emplace_back("signet-seed.ocvcoin.tv.");
vSeeds.emplace_back("signet-seed.ocvcoin.pw.");
vSeeds.emplace_back("signet-seed.ocvcoin.app.");
vSeeds.emplace_back("signet-seed.ocvcoin.club.");
vSeeds.emplace_back("signet-seed.ocvcoin.pro.");
vSeeds.emplace_back("signet-seed.ocvcoin.shop.");
vSeeds.emplace_back("signet-seed.ocvcoin.uk.");
vSeeds.emplace_back("signet-seed.ocvcoin.site.");
vSeeds.emplace_back("signet-seed.ocvcoin.ca.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.uk.");
vSeeds.emplace_back("signet-seed.ocvcoin.store.");
vSeeds.emplace_back("signet-seed.ocvcoin.space.");
vSeeds.emplace_back("signet-seed.ocvcoin.win.");
vSeeds.emplace_back("signet-seed.ocvcoin.it.");
vSeeds.emplace_back("signet-seed.ocvcoin.link.");
vSeeds.emplace_back("signet-seed.ocvcoin.es.");
vSeeds.emplace_back("signet-seed.ocvcoin.one.");
vSeeds.emplace_back("signet-seed.ocvcoin.cloud.");
vSeeds.emplace_back("signet-seed.ocvcoin.ltd.");
vSeeds.emplace_back("signet-seed.ocvcoin.ru.");
vSeeds.emplace_back("signet-seed.ocvcoin.live.");
vSeeds.emplace_back("signet-seed.ocvcoin.download.");
vSeeds.emplace_back("signet-seed.ocvcoin.work.");
vSeeds.emplace_back("signet-seed.ocvcoin.sh.");
vSeeds.emplace_back("signet-seed.ocvcoin.im.");
vSeeds.emplace_back("signet-seed.ocvcoin.to.");
vSeeds.emplace_back("signet-seed.ocvcoin.ws.");
vSeeds.emplace_back("signet-seed.ocvcoin.art.");
vSeeds.emplace_back("signet-seed.ocvcoin.ch.");
vSeeds.emplace_back("signet-seed.ocvcoin.id.");
vSeeds.emplace_back("signet-seed.ocvcoin.rs.");
vSeeds.emplace_back("signet-seed.ocvcoin.life.");
vSeeds.emplace_back("signet-seed.ocvcoin.al.");
vSeeds.emplace_back("signet-seed.ocvcoin.li.");
vSeeds.emplace_back("signet-seed.ocvcoin.design.");
vSeeds.emplace_back("signet-seed.ocvcoin.st.");
vSeeds.emplace_back("signet-seed.ocvcoin.stream.");
vSeeds.emplace_back("signet-seed.ocvcoin.click.");
vSeeds.emplace_back("signet-seed.ocvcoin.se.");
vSeeds.emplace_back("signet-seed.ocvcoin.email.");
vSeeds.emplace_back("signet-seed.ocvcoin.network.");
vSeeds.emplace_back("signet-seed.ocvcoin.re.");
vSeeds.emplace_back("signet-seed.ocvcoin.host.");
vSeeds.emplace_back("signet-seed.ocvcoin.blog.");
vSeeds.emplace_back("signet-seed.ocvcoin.at.");
vSeeds.emplace_back("signet-seed.ocvcoin.fr.");
vSeeds.emplace_back("signet-seed.ocvcoin.nl.");
vSeeds.emplace_back("signet-seed.ocvcoin.world.");
vSeeds.emplace_back("signet-seed.ocvcoin.be.");
vSeeds.emplace_back("signet-seed.ocvcoin.zone.");
vSeeds.emplace_back("signet-seed.ocvcoin.news.");
vSeeds.emplace_back("signet-seed.ocvcoin.ng.");
vSeeds.emplace_back("signet-seed.ocvcoin.science.");
vSeeds.emplace_back("signet-seed.ocvcoin.asia.");
vSeeds.emplace_back("signet-seed.ocvcoin.lol.");
vSeeds.emplace_back("signet-seed.ocvcoin.ee.");
vSeeds.emplace_back("signet-seed.ocvcoin.website.");
vSeeds.emplace_back("signet-seed.ocvcoin.cx.");
vSeeds.emplace_back("signet-seed.ocvcoin.wtf.");
vSeeds.emplace_back("signet-seed.ocvcoin.xxx.");
vSeeds.emplace_back("signet-seed.ocvcoin.la.");
vSeeds.emplace_back("signet-seed.ocvcoin.moe.");
vSeeds.emplace_back("signet-seed.ocvcoin.digital.");
vSeeds.emplace_back("signet-seed.ocvcoin.studio.");
vSeeds.emplace_back("signet-seed.ocvcoin.su.");
vSeeds.emplace_back("signet-seed.ocvcoin.party.");
vSeeds.emplace_back("signet-seed.ocvcoin.bot.");
vSeeds.emplace_back("signet-seed.ocvcoin.vip.");
vSeeds.emplace_back("signet-seed.ocvcoin.today.");
vSeeds.emplace_back("signet-seed.ocvcoin.trade.");
vSeeds.emplace_back("signet-seed.ocvcoin.ly.");
vSeeds.emplace_back("signet-seed.ocvcoin.mobi.");
vSeeds.emplace_back("signet-seed.ocvcoin.fun.");
vSeeds.emplace_back("signet-seed.ocvcoin.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.ac.");
vSeeds.emplace_back("signet-seed.ocvcoin.mx.");
vSeeds.emplace_back("signet-seed.ocvcoin.group.");
vSeeds.emplace_back("signet-seed.ocvcoin.icu.");
vSeeds.emplace_back("signet-seed.ocvcoin.agency.");
vSeeds.emplace_back("signet-seed.ocvcoin.men.");
vSeeds.emplace_back("signet-seed.ocvcoin.wiki.");
vSeeds.emplace_back("signet-seed.ocvcoin.ooo.");
vSeeds.emplace_back("signet-seed.ocvcoin.is.");
vSeeds.emplace_back("signet-seed.ocvcoin.games.");
vSeeds.emplace_back("signet-seed.ocvcoin.media.");
vSeeds.emplace_back("signet-seed.ocvcoin.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.fm.");
vSeeds.emplace_back("signet-seed.ocvcoin.fi.");
vSeeds.emplace_back("signet-seed.ocvcoin.ro.");
vSeeds.emplace_back("signet-seed.ocvcoin.ie.");
vSeeds.emplace_back("signet-seed.ocvcoin.ae.");
vSeeds.emplace_back("signet-seed.ocvcoin.ninja.");
vSeeds.emplace_back("signet-seed.ocvcoin.best.");
vSeeds.emplace_back("signet-seed.ocvcoin.cat.");
vSeeds.emplace_back("signet-seed.ocvcoin.company.");
vSeeds.emplace_back("signet-seed.ocvcoin.do.");
vSeeds.emplace_back("signet-seed.ocvcoin.tools.");
vSeeds.emplace_back("signet-seed.ocvcoin.global.");
vSeeds.emplace_back("signet-seed.ocvcoin.red.");
vSeeds.emplace_back("signet-seed.ocvcoin.guru.");
vSeeds.emplace_back("signet-seed.ocvcoin.vc.");
vSeeds.emplace_back("signet-seed.ocvcoin.dog.");
vSeeds.emplace_back("signet-seed.ocvcoin.bar.");
vSeeds.emplace_back("signet-seed.ocvcoin.bid.");
vSeeds.emplace_back("signet-seed.ocvcoin.city.");
vSeeds.emplace_back("signet-seed.ocvcoin.codes.");
vSeeds.emplace_back("signet-seed.ocvcoin.works.");
vSeeds.emplace_back("signet-seed.ocvcoin.business.");
vSeeds.emplace_back("signet-seed.ocvcoin.am.");
vSeeds.emplace_back("signet-seed.ocvcoin.services.");
vSeeds.emplace_back("signet-seed.ocvcoin.run.");
vSeeds.emplace_back("signet-seed.ocvcoin.ne.");
vSeeds.emplace_back("signet-seed.ocvcoin.market.");
vSeeds.emplace_back("signet-seed.ocvcoin.ms.");
vSeeds.emplace_back("signet-seed.ocvcoin.center.");
vSeeds.emplace_back("signet-seed.ocvcoin.team.");
vSeeds.emplace_back("signet-seed.ocvcoin.rocks.");
vSeeds.emplace_back("signet-seed.ocvcoin.money.");
vSeeds.emplace_back("signet-seed.ocvcoin.plus.");
vSeeds.emplace_back("signet-seed.ocvcoin.finance.");
vSeeds.emplace_back("signet-seed.ocvcoin.ink.");
vSeeds.emplace_back("signet-seed.ocvcoin.travel.");
vSeeds.emplace_back("signet-seed.ocvcoin.solutions.");
vSeeds.emplace_back("signet-seed.ocvcoin.bio.");
vSeeds.emplace_back("signet-seed.ocvcoin.social.");
vSeeds.emplace_back("signet-seed.ocvcoin.pt.");
vSeeds.emplace_back("signet-seed.ocvcoin.jp.");
vSeeds.emplace_back("signet-seed.ocvcoin.technology.");
vSeeds.emplace_back("signet-seed.ocvcoin.ovh.");
vSeeds.emplace_back("signet-seed.ocvcoin.review.");
vSeeds.emplace_back("signet-seed.ocvcoin.academy.");
vSeeds.emplace_back("signet-seed.ocvcoin.international.");
vSeeds.emplace_back("signet-seed.ocvcoin.university.");
vSeeds.emplace_back("signet-seed.ocvcoin.family.");
vSeeds.emplace_back("signet-seed.ocvcoin.systems.");
vSeeds.emplace_back("signet-seed.ocvcoin.rentals.");
vSeeds.emplace_back("signet-seed.ocvcoin.domains.");
vSeeds.emplace_back("signet-seed.ocvcoin.consulting.");
vSeeds.emplace_back("signet-seed.ocvcoin.buzz.");
vSeeds.emplace_back("signet-seed.ocvcoin.sale.");
vSeeds.emplace_back("signet-seed.ocvcoin.tw.");
vSeeds.emplace_back("signet-seed.ocvcoin.name.");
vSeeds.emplace_back("signet-seed.ocvcoin.marketing.");
vSeeds.emplace_back("signet-seed.ocvcoin.tk.");
vSeeds.emplace_back("signet-seed.ocvcoin.pm.");
vSeeds.emplace_back("signet-seed.ocvcoin.ma.");
vSeeds.emplace_back("signet-seed.ocvcoin.pizza.");
vSeeds.emplace_back("signet-seed.ocvcoin.tel.");
vSeeds.emplace_back("signet-seed.ocvcoin.pub.");
vSeeds.emplace_back("signet-seed.ocvcoin.photography.");
vSeeds.emplace_back("signet-seed.ocvcoin.so.");
vSeeds.emplace_back("signet-seed.ocvcoin.training.");
vSeeds.emplace_back("signet-seed.ocvcoin.cafe.");
vSeeds.emplace_back("signet-seed.ocvcoin.expert.");
vSeeds.emplace_back("signet-seed.ocvcoin.no.");
vSeeds.emplace_back("signet-seed.ocvcoin.recipes.");
vSeeds.emplace_back("signet-seed.ocvcoin.cheap.");
vSeeds.emplace_back("signet-seed.ocvcoin.cool.");
vSeeds.emplace_back("signet-seed.ocvcoin.directory.");
vSeeds.emplace_back("signet-seed.ocvcoin.audio.");
vSeeds.emplace_back("signet-seed.ocvcoin.blue.");
vSeeds.emplace_back("signet-seed.ocvcoin.cash.");
vSeeds.emplace_back("signet-seed.ocvcoin.taxi.");
vSeeds.emplace_back("signet-seed.ocvcoin.casa.");
vSeeds.emplace_back("signet-seed.ocvcoin.engineering.");
vSeeds.emplace_back("signet-seed.ocvcoin.coffee.");
vSeeds.emplace_back("signet-seed.ocvcoin.properties.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.au.");
vSeeds.emplace_back("signet-seed.ocvcoin.tours.");
vSeeds.emplace_back("signet-seed.ocvcoin.ci.");
vSeeds.emplace_back("signet-seed.ocvcoin.nu.");
vSeeds.emplace_back("signet-seed.ocvcoin.boutique.");
vSeeds.emplace_back("signet-seed.ocvcoin.support.");
vSeeds.emplace_back("signet-seed.ocvcoin.foundation.");
vSeeds.emplace_back("signet-seed.ocvcoin.events.");
vSeeds.emplace_back("signet-seed.ocvcoin.fitness.");
vSeeds.emplace_back("signet-seed.ocvcoin.church.");
vSeeds.emplace_back("signet-seed.ocvcoin.capital.");
vSeeds.emplace_back("signet-seed.ocvcoin.gold.");
vSeeds.emplace_back("signet-seed.ocvcoin.delivery.");
vSeeds.emplace_back("signet-seed.ocvcoin.institute.");
vSeeds.emplace_back("signet-seed.ocvcoin.af.");
vSeeds.emplace_back("signet-seed.ocvcoin.software.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.in.");
vSeeds.emplace_back("signet-seed.ocvcoin.reviews.");
vSeeds.emplace_back("signet-seed.ocvcoin.edu.");
vSeeds.emplace_back("signet-seed.ocvcoin.watch.");
vSeeds.emplace_back("signet-seed.ocvcoin.webcam.");
vSeeds.emplace_back("signet-seed.ocvcoin.love.");
vSeeds.emplace_back("signet-seed.ocvcoin.auction.");
vSeeds.emplace_back("signet-seed.ocvcoin.toys.");
vSeeds.emplace_back("signet-seed.ocvcoin.legal.");
vSeeds.emplace_back("signet-seed.ocvcoin.express.");
vSeeds.emplace_back("signet-seed.ocvcoin.partners.");
vSeeds.emplace_back("signet-seed.ocvcoin.si.");
vSeeds.emplace_back("signet-seed.ocvcoin.coach.");
vSeeds.emplace_back("signet-seed.ocvcoin.style.");
vSeeds.emplace_back("signet-seed.ocvcoin.pp.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.date.");
vSeeds.emplace_back("signet-seed.ocvcoin.bz.");
vSeeds.emplace_back("signet-seed.ocvcoin.town.");
vSeeds.emplace_back("signet-seed.ocvcoin.energy.");
vSeeds.emplace_back("signet-seed.ocvcoin.report.");
vSeeds.emplace_back("signet-seed.ocvcoin.photos.");
vSeeds.emplace_back("signet-seed.ocvcoin.inc.");
vSeeds.emplace_back("signet-seed.ocvcoin.loan.");
vSeeds.emplace_back("signet-seed.ocvcoin.ventures.");
vSeeds.emplace_back("signet-seed.ocvcoin.computer.");
vSeeds.emplace_back("signet-seed.ocvcoin.enterprises.");
vSeeds.emplace_back("signet-seed.ocvcoin.restaurant.");
vSeeds.emplace_back("signet-seed.ocvcoin.fund.");
vSeeds.emplace_back("signet-seed.ocvcoin.chat.");
vSeeds.emplace_back("signet-seed.ocvcoin.rip.");
vSeeds.emplace_back("signet-seed.ocvcoin.help.");
vSeeds.emplace_back("signet-seed.ocvcoin.cm.");
vSeeds.emplace_back("signet-seed.ocvcoin.hk.");
vSeeds.emplace_back("signet-seed.ocvcoin.as.");
vSeeds.emplace_back("signet-seed.ocvcoin.credit.");
vSeeds.emplace_back("signet-seed.ocvcoin.racing.");
vSeeds.emplace_back("signet-seed.ocvcoin.sg.");
vSeeds.emplace_back("signet-seed.ocvcoin.gay.");
vSeeds.emplace_back("signet-seed.ocvcoin.cz.");
vSeeds.emplace_back("signet-seed.ocvcoin.cricket.");
vSeeds.emplace_back("signet-seed.ocvcoin.investments.");
vSeeds.emplace_back("signet-seed.ocvcoin.band.");
vSeeds.emplace_back("signet-seed.ocvcoin.ag.");
vSeeds.emplace_back("signet-seed.ocvcoin.et.");
vSeeds.emplace_back("signet-seed.ocvcoin.bike.");
vSeeds.emplace_back("signet-seed.ocvcoin.dating.");
vSeeds.emplace_back("signet-seed.ocvcoin.press.");
vSeeds.emplace_back("signet-seed.ocvcoin.vin.");
vSeeds.emplace_back("signet-seed.ocvcoin.repair.");
vSeeds.emplace_back("signet-seed.ocvcoin.ml.");
vSeeds.emplace_back("signet-seed.ocvcoin.fyi.");
vSeeds.emplace_back("signet-seed.ocvcoin.casino.");
vSeeds.emplace_back("signet-seed.ocvcoin.vet.");
vSeeds.emplace_back("signet-seed.ocvcoin.yoga.");
vSeeds.emplace_back("signet-seed.ocvcoin.land.");
vSeeds.emplace_back("signet-seed.ocvcoin.page.");
vSeeds.emplace_back("signet-seed.ocvcoin.ax.");
vSeeds.emplace_back("signet-seed.ocvcoin.shoes.");
vSeeds.emplace_back("signet-seed.ocvcoin.show.");
vSeeds.emplace_back("signet-seed.ocvcoin.nz.");
vSeeds.emplace_back("signet-seed.ocvcoin.cyou.");
vSeeds.emplace_back("signet-seed.ocvcoin.sc.");
vSeeds.emplace_back("signet-seed.ocvcoin.coupons.");
vSeeds.emplace_back("signet-seed.ocvcoin.kim.");
vSeeds.emplace_back("signet-seed.ocvcoin.deals.");
vSeeds.emplace_back("signet-seed.ocvcoin.guide.");
vSeeds.emplace_back("signet-seed.ocvcoin.black.");
vSeeds.emplace_back("signet-seed.ocvcoin.rest.");
vSeeds.emplace_back("signet-seed.ocvcoin.pe.");
vSeeds.emplace_back("signet-seed.ocvcoin.estate.");
vSeeds.emplace_back("signet-seed.ocvcoin.bargains.");
vSeeds.emplace_back("signet-seed.ocvcoin.accountants.");
vSeeds.emplace_back("signet-seed.ocvcoin.tax.");
vSeeds.emplace_back("signet-seed.ocvcoin.wedding.");
vSeeds.emplace_back("signet-seed.ocvcoin.dance.");
vSeeds.emplace_back("signet-seed.ocvcoin.productions.");
vSeeds.emplace_back("signet-seed.ocvcoin.financial.");
vSeeds.emplace_back("signet-seed.ocvcoin.kitchen.");
vSeeds.emplace_back("signet-seed.ocvcoin.tn.");
vSeeds.emplace_back("signet-seed.ocvcoin.direct.");
vSeeds.emplace_back("signet-seed.ocvcoin.care.");
vSeeds.emplace_back("signet-seed.ocvcoin.sex.");
vSeeds.emplace_back("signet-seed.ocvcoin.promo.");
vSeeds.emplace_back("signet-seed.ocvcoin.lu.");
vSeeds.emplace_back("signet-seed.ocvcoin.moda.");
vSeeds.emplace_back("signet-seed.ocvcoin.golf.");
vSeeds.emplace_back("signet-seed.ocvcoin.tips.");
vSeeds.emplace_back("signet-seed.ocvcoin.sexy.");
vSeeds.emplace_back("signet-seed.ocvcoin.game.");
vSeeds.emplace_back("signet-seed.ocvcoin.haus.");
vSeeds.emplace_back("signet-seed.ocvcoin.tf.");
vSeeds.emplace_back("signet-seed.ocvcoin.wine.");
vSeeds.emplace_back("signet-seed.ocvcoin.pet.");
vSeeds.emplace_back("signet-seed.ocvcoin.gift.");
vSeeds.emplace_back("signet-seed.ocvcoin.blackfriday.");
vSeeds.emplace_back("signet-seed.ocvcoin.school.");
vSeeds.emplace_back("signet-seed.ocvcoin.tennis.");
vSeeds.emplace_back("signet-seed.ocvcoin.md.");
vSeeds.emplace_back("signet-seed.ocvcoin.monster.");
vSeeds.emplace_back("signet-seed.ocvcoin.exchange.");
vSeeds.emplace_back("signet-seed.ocvcoin.video.");
vSeeds.emplace_back("signet-seed.ocvcoin.forsale.");
vSeeds.emplace_back("signet-seed.ocvcoin.photo.");
vSeeds.emplace_back("signet-seed.ocvcoin.actor.");
vSeeds.emplace_back("signet-seed.ocvcoin.community.");
vSeeds.emplace_back("signet-seed.ocvcoin.accountant.");
vSeeds.emplace_back("signet-seed.ocvcoin.movie.");
vSeeds.emplace_back("signet-seed.ocvcoin.yt.");
vSeeds.emplace_back("signet-seed.ocvcoin.ge.");
vSeeds.emplace_back("signet-seed.ocvcoin.cam.");
vSeeds.emplace_back("signet-seed.ocvcoin.mc.");
vSeeds.emplace_back("signet-seed.ocvcoin.ga.");
vSeeds.emplace_back("signet-seed.ocvcoin.parts.");
vSeeds.emplace_back("signet-seed.ocvcoin.education.");
vSeeds.emplace_back("signet-seed.ocvcoin.faith.");
vSeeds.emplace_back("signet-seed.ocvcoin.hosting.");
vSeeds.emplace_back("signet-seed.ocvcoin.bi.");
vSeeds.emplace_back("signet-seed.ocvcoin.bingo.");
vSeeds.emplace_back("signet-seed.ocvcoin.engineer.");
vSeeds.emplace_back("signet-seed.ocvcoin.bet.");
vSeeds.emplace_back("signet-seed.ocvcoin.associates.");
vSeeds.emplace_back("signet-seed.ocvcoin.ski.");
vSeeds.emplace_back("signet-seed.ocvcoin.singles.");
vSeeds.emplace_back("signet-seed.ocvcoin.farm.");
vSeeds.emplace_back("signet-seed.ocvcoin.pink.");
vSeeds.emplace_back("signet-seed.ocvcoin.claims.");
vSeeds.emplace_back("signet-seed.ocvcoin.pics.");
vSeeds.emplace_back("signet-seed.ocvcoin.careers.");
vSeeds.emplace_back("signet-seed.ocvcoin.dk.");
vSeeds.emplace_back("signet-seed.ocvcoin.gy.");
vSeeds.emplace_back("signet-seed.ocvcoin.nyc.");
vSeeds.emplace_back("signet-seed.ocvcoin.menu.");
vSeeds.emplace_back("signet-seed.ocvcoin.house.");
vSeeds.emplace_back("signet-seed.ocvcoin.ad.");
vSeeds.emplace_back("signet-seed.ocvcoin.cl.");
vSeeds.emplace_back("signet-seed.ocvcoin.uno.");
vSeeds.emplace_back("signet-seed.ocvcoin.hu.");
vSeeds.emplace_back("signet-seed.ocvcoin.cards.");
vSeeds.emplace_back("signet-seed.ocvcoin.gl.");
vSeeds.emplace_back("signet-seed.ocvcoin.tube.");
vSeeds.emplace_back("signet-seed.ocvcoin.gs.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.co.");
vSeeds.emplace_back("signet-seed.ocvcoin.doctor.");
vSeeds.emplace_back("signet-seed.ocvcoin.lt.");
vSeeds.emplace_back("signet-seed.ocvcoin.ar.");
vSeeds.emplace_back("signet-seed.ocvcoin.study.");
vSeeds.emplace_back("signet-seed.ocvcoin.rent.");
vSeeds.emplace_back("signet-seed.ocvcoin.vision.");
vSeeds.emplace_back("signet-seed.ocvcoin.mortgage.");
vSeeds.emplace_back("signet-seed.ocvcoin.shopping.");
vSeeds.emplace_back("signet-seed.ocvcoin.gratis.");
vSeeds.emplace_back("signet-seed.ocvcoin.apartments.");
vSeeds.emplace_back("signet-seed.ocvcoin.army.");
vSeeds.emplace_back("signet-seed.ocvcoin.fit.");
vSeeds.emplace_back("signet-seed.ocvcoin.college.");
vSeeds.emplace_back("signet-seed.ocvcoin.aaa.");
vSeeds.emplace_back("signet-seed.ocvcoin.builders.");
vSeeds.emplace_back("signet-seed.ocvcoin.beer.");
vSeeds.emplace_back("signet-seed.ocvcoin.irish.");
vSeeds.emplace_back("signet-seed.ocvcoin.clinic.");
vSeeds.emplace_back("signet-seed.ocvcoin.law.");
vSeeds.emplace_back("signet-seed.ocvcoin.gives.");
vSeeds.emplace_back("signet-seed.ocvcoin.ke.");
vSeeds.emplace_back("signet-seed.ocvcoin.fish.");
vSeeds.emplace_back("signet-seed.ocvcoin.sucks.");
vSeeds.emplace_back("signet-seed.ocvcoin.country.");
vSeeds.emplace_back("signet-seed.ocvcoin.porn.");
vSeeds.emplace_back("signet-seed.ocvcoin.camera.");
vSeeds.emplace_back("signet-seed.ocvcoin.vg.");
vSeeds.emplace_back("signet-seed.ocvcoin.sarl.");
vSeeds.emplace_back("signet-seed.ocvcoin.salon.");
vSeeds.emplace_back("signet-seed.ocvcoin.security.");
vSeeds.emplace_back("signet-seed.ocvcoin.attorney.");
vSeeds.emplace_back("signet-seed.ocvcoin.pictures.");
vSeeds.emplace_back("signet-seed.ocvcoin.hr.");
vSeeds.emplace_back("signet-seed.ocvcoin.solar.");
vSeeds.emplace_back("signet-seed.ocvcoin.mom.");
vSeeds.emplace_back("signet-seed.ocvcoin.equipment.");
vSeeds.emplace_back("signet-seed.ocvcoin.charity.");
vSeeds.emplace_back("signet-seed.ocvcoin.viajes.");
vSeeds.emplace_back("signet-seed.ocvcoin.christmas.");
vSeeds.emplace_back("signet-seed.ocvcoin.na.");
vSeeds.emplace_back("signet-seed.ocvcoin.discount.");
vSeeds.emplace_back("signet-seed.ocvcoin.boats.");
vSeeds.emplace_back("signet-seed.ocvcoin.ph.");
vSeeds.emplace_back("signet-seed.ocvcoin.feedback.");
vSeeds.emplace_back("signet-seed.ocvcoin.sk.");
vSeeds.emplace_back("signet-seed.ocvcoin.creditcard.");
vSeeds.emplace_back("signet-seed.ocvcoin.vegas.");
vSeeds.emplace_back("signet-seed.ocvcoin.baby.");
vSeeds.emplace_back("signet-seed.ocvcoin.clothing.");
vSeeds.emplace_back("signet-seed.ocvcoin.cab.");
vSeeds.emplace_back("signet-seed.ocvcoin.courses.");
vSeeds.emplace_back("signet-seed.ocvcoin.camp.");
vSeeds.emplace_back("signet-seed.ocvcoin.villas.");
vSeeds.emplace_back("signet-seed.ocvcoin.fail.");
vSeeds.emplace_back("signet-seed.ocvcoin.rodeo.");
vSeeds.emplace_back("signet-seed.ocvcoin.london.");
vSeeds.emplace_back("signet-seed.ocvcoin.earth.");
vSeeds.emplace_back("signet-seed.ocvcoin.lgbt.");
vSeeds.emplace_back("signet-seed.ocvcoin.ing.");
vSeeds.emplace_back("signet-seed.ocvcoin.jobs.");
vSeeds.emplace_back("signet-seed.ocvcoin.eco.");
vSeeds.emplace_back("signet-seed.ocvcoin.ht.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.uk.");
vSeeds.emplace_back("signet-seed.ocvcoin.aero.");
vSeeds.emplace_back("signet-seed.ocvcoin.cf.");
vSeeds.emplace_back("signet-seed.ocvcoin.management.");
vSeeds.emplace_back("signet-seed.ocvcoin.berlin.");
vSeeds.emplace_back("signet-seed.ocvcoin.voyage.");
vSeeds.emplace_back("signet-seed.ocvcoin.gr.");
vSeeds.emplace_back("signet-seed.ocvcoin.tienda.");
vSeeds.emplace_back("signet-seed.ocvcoin.sa.");
vSeeds.emplace_back("signet-seed.ocvcoin.reisen.");
vSeeds.emplace_back("signet-seed.ocvcoin.construction.");
vSeeds.emplace_back("signet-seed.ocvcoin.mn.");
vSeeds.emplace_back("signet-seed.ocvcoin.schule.");
vSeeds.emplace_back("signet-seed.ocvcoin.ceo.");
vSeeds.emplace_back("signet-seed.ocvcoin.tl.");
vSeeds.emplace_back("signet-seed.ocvcoin.by.");
vSeeds.emplace_back("signet-seed.ocvcoin.green.");
vSeeds.emplace_back("signet-seed.ocvcoin.car.");
vSeeds.emplace_back("signet-seed.ocvcoin.cleaning.");
vSeeds.emplace_back("signet-seed.ocvcoin.immo.");
vSeeds.emplace_back("signet-seed.ocvcoin.sx.");
vSeeds.emplace_back("signet-seed.ocvcoin.property.");
vSeeds.emplace_back("signet-seed.ocvcoin.sb.");
vSeeds.emplace_back("signet-seed.ocvcoin.gallery.");
vSeeds.emplace_back("signet-seed.ocvcoin.adult.");
vSeeds.emplace_back("signet-seed.ocvcoin.wf.");
vSeeds.emplace_back("signet-seed.ocvcoin.je.");
vSeeds.emplace_back("signet-seed.ocvcoin.lc.");
vSeeds.emplace_back("signet-seed.ocvcoin.build.");
vSeeds.emplace_back("signet-seed.ocvcoin.vacations.");
vSeeds.emplace_back("signet-seed.ocvcoin.furniture.");
vSeeds.emplace_back("signet-seed.ocvcoin.qa.");
vSeeds.emplace_back("signet-seed.ocvcoin.catering.");
vSeeds.emplace_back("signet-seed.ocvcoin.fashion.");
vSeeds.emplace_back("signet-seed.ocvcoin.supplies.");
vSeeds.emplace_back("signet-seed.ocvcoin.tt.");
vSeeds.emplace_back("signet-seed.ocvcoin.th.");
vSeeds.emplace_back("signet-seed.ocvcoin.lawyer.");
vSeeds.emplace_back("signet-seed.ocvcoin.cruises.");
vSeeds.emplace_back("signet-seed.ocvcoin.diamonds.");
vSeeds.emplace_back("signet-seed.ocvcoin.holiday.");
vSeeds.emplace_back("signet-seed.ocvcoin.holdings.");
vSeeds.emplace_back("signet-seed.ocvcoin.ba.");
vSeeds.emplace_back("signet-seed.ocvcoin.exposed.");
vSeeds.emplace_back("signet-seed.ocvcoin.tires.");
vSeeds.emplace_back("signet-seed.ocvcoin.ki.");
vSeeds.emplace_back("signet-seed.ocvcoin.bo.");
vSeeds.emplace_back("signet-seed.ocvcoin.place.");
vSeeds.emplace_back("signet-seed.ocvcoin.healthcare.");
vSeeds.emplace_back("signet-seed.ocvcoin.my.");
vSeeds.emplace_back("signet-seed.ocvcoin.homes.");
vSeeds.emplace_back("signet-seed.ocvcoin.tokyo.");
vSeeds.emplace_back("signet-seed.ocvcoin.cv.");
vSeeds.emplace_back("signet-seed.ocvcoin.airforce.");
vSeeds.emplace_back("signet-seed.ocvcoin.mu.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.za.");
vSeeds.emplace_back("signet-seed.ocvcoin.observer.");
vSeeds.emplace_back("signet-seed.ocvcoin.graphics.");
vSeeds.emplace_back("signet-seed.ocvcoin.archi.");
vSeeds.emplace_back("signet-seed.ocvcoin.mk.");
vSeeds.emplace_back("signet-seed.ocvcoin.gd.");
vSeeds.emplace_back("signet-seed.ocvcoin.gdn.");
vSeeds.emplace_back("signet-seed.ocvcoin.tc.");
vSeeds.emplace_back("signet-seed.ocvcoin.futbol.");
vSeeds.emplace_back("signet-seed.ocvcoin.poker.");
vSeeds.emplace_back("signet-seed.ocvcoin.mo.");
vSeeds.emplace_back("signet-seed.ocvcoin.mba.");
vSeeds.emplace_back("signet-seed.ocvcoin.cooking.");
vSeeds.emplace_back("signet-seed.ocvcoin.auto.");
vSeeds.emplace_back("signet-seed.ocvcoin.om.");
vSeeds.emplace_back("signet-seed.ocvcoin.onl.");
vSeeds.emplace_back("signet-seed.ocvcoin.sbs.");
vSeeds.emplace_back("signet-seed.ocvcoin.garden.");
vSeeds.emplace_back("signet-seed.ocvcoin.immobilien.");
vSeeds.emplace_back("signet-seed.ocvcoin.dentist.");
vSeeds.emplace_back("signet-seed.ocvcoin.surf.");
vSeeds.emplace_back("signet-seed.ocvcoin.au.");
vSeeds.emplace_back("signet-seed.ocvcoin.republican.");
vSeeds.emplace_back("signet-seed.ocvcoin.africa.");
vSeeds.emplace_back("signet-seed.ocvcoin.flights.");
vSeeds.emplace_back("signet-seed.ocvcoin.glass.");
vSeeds.emplace_back("signet-seed.ocvcoin.wang.");
vSeeds.emplace_back("signet-seed.ocvcoin.dental.");
vSeeds.emplace_back("signet-seed.ocvcoin.vodka.");
vSeeds.emplace_back("signet-seed.ocvcoin.jewelry.");
vSeeds.emplace_back("signet-seed.ocvcoin.contractors.");
vSeeds.emplace_back("signet-seed.ocvcoin.condos.");
vSeeds.emplace_back("signet-seed.ocvcoin.fo.");
vSeeds.emplace_back("signet-seed.ocvcoin.miami.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.ir.");
vSeeds.emplace_back("signet-seed.ocvcoin.kr.");
vSeeds.emplace_back("signet-seed.ocvcoin.dj.");
vSeeds.emplace_back("signet-seed.ocvcoin.football.");
vSeeds.emplace_back("signet-seed.ocvcoin.desi.");
vSeeds.emplace_back("signet-seed.ocvcoin.hn.");
vSeeds.emplace_back("signet-seed.ocvcoin.bg.");
vSeeds.emplace_back("signet-seed.ocvcoin.cr.");
vSeeds.emplace_back("signet-seed.ocvcoin.tattoo.");
vSeeds.emplace_back("signet-seed.ocvcoin.lease.");
vSeeds.emplace_back("signet-seed.ocvcoin.industries.");
vSeeds.emplace_back("signet-seed.ocvcoin.fans.");
vSeeds.emplace_back("signet-seed.ocvcoin.limited.");
vSeeds.emplace_back("signet-seed.ocvcoin.degree.");
vSeeds.emplace_back("signet-seed.ocvcoin.supply.");
vSeeds.emplace_back("signet-seed.ocvcoin.llc.");
vSeeds.emplace_back("signet-seed.ocvcoin.vote.");
vSeeds.emplace_back("signet-seed.ocvcoin.ec.");
vSeeds.emplace_back("signet-seed.ocvcoin.cars.");
vSeeds.emplace_back("signet-seed.ocvcoin.ist.");
vSeeds.emplace_back("signet-seed.ocvcoin.horse.");
vSeeds.emplace_back("signet-seed.ocvcoin.ps.");
vSeeds.emplace_back("signet-seed.ocvcoin.loans.");
vSeeds.emplace_back("signet-seed.ocvcoin.democrat.");
vSeeds.emplace_back("signet-seed.ocvcoin.how.");
vSeeds.emplace_back("signet-seed.ocvcoin.maison.");
vSeeds.emplace_back("signet-seed.ocvcoin.theater.");
vSeeds.emplace_back("signet-seed.ocvcoin.scot.");
vSeeds.emplace_back("signet-seed.ocvcoin.flowers.");
vSeeds.emplace_back("signet-seed.ocvcoin.soccer.");
vSeeds.emplace_back("signet-seed.ocvcoin.rehab.");
vSeeds.emplace_back("signet-seed.ocvcoin.pk.");
vSeeds.emplace_back("signet-seed.ocvcoin.vn.");
vSeeds.emplace_back("signet-seed.ocvcoin.limo.");
vSeeds.emplace_back("signet-seed.ocvcoin.health.");
vSeeds.emplace_back("signet-seed.ocvcoin.realty.");
vSeeds.emplace_back("signet-seed.ocvcoin.lv.");
vSeeds.emplace_back("signet-seed.ocvcoin.bb.");
vSeeds.emplace_back("signet-seed.ocvcoin.surgery.");
vSeeds.emplace_back("signet-seed.ocvcoin.film.");
vSeeds.emplace_back("signet-seed.ocvcoin.kz.");
vSeeds.emplace_back("signet-seed.ocvcoin.cd.");
vSeeds.emplace_back("signet-seed.ocvcoin.ky.");
vSeeds.emplace_back("signet-seed.ocvcoin.soy.");
vSeeds.emplace_back("signet-seed.ocvcoin.florist.");
vSeeds.emplace_back("signet-seed.ocvcoin.ngo.");
vSeeds.emplace_back("signet-seed.ocvcoin.quebec.");
vSeeds.emplace_back("signet-seed.ocvcoin.memorial.");
vSeeds.emplace_back("signet-seed.ocvcoin.shiksha.");
vSeeds.emplace_back("signet-seed.ocvcoin.protection.");
vSeeds.emplace_back("signet-seed.ocvcoin.diet.");
vSeeds.emplace_back("signet-seed.ocvcoin.fan.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.mx.");
vSeeds.emplace_back("signet-seed.ocvcoin.gripe.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.tr.");
vSeeds.emplace_back("signet-seed.ocvcoin.new.");
vSeeds.emplace_back("signet-seed.ocvcoin.gifts.");
vSeeds.emplace_back("signet-seed.ocvcoin.zip.");
vSeeds.emplace_back("signet-seed.ocvcoin.my.id.");
vSeeds.emplace_back("signet-seed.ocvcoin.compare.");
vSeeds.emplace_back("signet-seed.ocvcoin.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.plumbing.");
vSeeds.emplace_back("signet-seed.ocvcoin.cfd.");
vSeeds.emplace_back("signet-seed.ocvcoin.kiwi.");
vSeeds.emplace_back("signet-seed.ocvcoin.me.uk.");
vSeeds.emplace_back("signet-seed.ocvcoin.day.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.nz.");
vSeeds.emplace_back("signet-seed.ocvcoin.az.");
vSeeds.emplace_back("signet-seed.ocvcoin.bayern.");
vSeeds.emplace_back("signet-seed.ocvcoin.theatre.");
vSeeds.emplace_back("signet-seed.ocvcoin.voto.");
vSeeds.emplace_back("signet-seed.ocvcoin.ls.");
vSeeds.emplace_back("signet-seed.ocvcoin.hockey.");
vSeeds.emplace_back("signet-seed.ocvcoin.luxury.");
vSeeds.emplace_back("signet-seed.ocvcoin.lighting.");
vSeeds.emplace_back("signet-seed.ocvcoin.bs.");
vSeeds.emplace_back("signet-seed.ocvcoin.gmbh.");
vSeeds.emplace_back("signet-seed.ocvcoin.td.");
vSeeds.emplace_back("signet-seed.ocvcoin.fishing.");
vSeeds.emplace_back("signet-seed.ocvcoin.boston.");
vSeeds.emplace_back("signet-seed.ocvcoin.reise.");
vSeeds.emplace_back("signet-seed.ocvcoin.rich.");
vSeeds.emplace_back("signet-seed.ocvcoin.tm.");
vSeeds.emplace_back("signet-seed.ocvcoin.jetzt.");
vSeeds.emplace_back("signet-seed.ocvcoin.pr.");
vSeeds.emplace_back("signet-seed.ocvcoin.insure.");
vSeeds.emplace_back("signet-seed.ocvcoin.tg.");
vSeeds.emplace_back("signet-seed.ocvcoin.vu.");
vSeeds.emplace_back("signet-seed.ocvcoin.ug.");
vSeeds.emplace_back("signet-seed.ocvcoin.ni.");
vSeeds.emplace_back("signet-seed.ocvcoin.box.");
vSeeds.emplace_back("signet-seed.ocvcoin.organic.");
vSeeds.emplace_back("signet-seed.ocvcoin.hiphop.");
vSeeds.emplace_back("signet-seed.ocvcoin.cymru.");
vSeeds.emplace_back("signet-seed.ocvcoin.lk.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.de.");
vSeeds.emplace_back("signet-seed.ocvcoin.hospital.");
vSeeds.emplace_back("signet-seed.ocvcoin.ao.");
vSeeds.emplace_back("signet-seed.ocvcoin.sn.");
vSeeds.emplace_back("signet-seed.ocvcoin.lat.");
vSeeds.emplace_back("signet-seed.ocvcoin.bank.");
vSeeds.emplace_back("signet-seed.ocvcoin.yokohama.");
vSeeds.emplace_back("signet-seed.ocvcoin.guitars.");
vSeeds.emplace_back("signet-seed.ocvcoin.paris.");
vSeeds.emplace_back("signet-seed.ocvcoin.barcelona.");
vSeeds.emplace_back("signet-seed.ocvcoin.mg.");
vSeeds.emplace_back("signet-seed.ocvcoin.kaufen.");
vSeeds.emplace_back("signet-seed.ocvcoin.qpon.");
vSeeds.emplace_back("signet-seed.ocvcoin.sm.");
vSeeds.emplace_back("signet-seed.ocvcoin.storage.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.il.");
vSeeds.emplace_back("signet-seed.ocvcoin.mp.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ru.");
vSeeds.emplace_back("signet-seed.ocvcoin.edu.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.broker.");
vSeeds.emplace_back("signet-seed.ocvcoin.tickets.");
vSeeds.emplace_back("signet-seed.ocvcoin.bj.");
vSeeds.emplace_back("signet-seed.ocvcoin.autos.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.co.");
vSeeds.emplace_back("signet-seed.ocvcoin.career.");
vSeeds.emplace_back("signet-seed.ocvcoin.cy.");
vSeeds.emplace_back("signet-seed.ocvcoin.nf.");
vSeeds.emplace_back("signet-seed.ocvcoin.trading.");
vSeeds.emplace_back("signet-seed.ocvcoin.abogado.");
vSeeds.emplace_back("signet-seed.ocvcoin.aq.");
vSeeds.emplace_back("signet-seed.ocvcoin.amsterdam.");
vSeeds.emplace_back("signet-seed.ocvcoin.pa.");
vSeeds.emplace_back("signet-seed.ocvcoin.bond.");
vSeeds.emplace_back("signet-seed.ocvcoin.dz.");
vSeeds.emplace_back("signet-seed.ocvcoin.mt.");
vSeeds.emplace_back("signet-seed.ocvcoin.contact.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.in.");
vSeeds.emplace_back("signet-seed.ocvcoin.navy.");
vSeeds.emplace_back("signet-seed.ocvcoin.capetown.");
vSeeds.emplace_back("signet-seed.ocvcoin.channel.");
vSeeds.emplace_back("signet-seed.ocvcoin.markets.");
vSeeds.emplace_back("signet-seed.ocvcoin.forum.");
vSeeds.emplace_back("signet-seed.ocvcoin.bible.");
vSeeds.emplace_back("signet-seed.ocvcoin.bh.");
vSeeds.emplace_back("signet-seed.ocvcoin.sr.");
vSeeds.emplace_back("signet-seed.ocvcoin.voting.");
vSeeds.emplace_back("signet-seed.ocvcoin.juegos.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.iq.");
vSeeds.emplace_back("signet-seed.ocvcoin.wien.");
vSeeds.emplace_back("signet-seed.ocvcoin.wales.");
vSeeds.emplace_back("signet-seed.ocvcoin.srl.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.in.");
vSeeds.emplace_back("signet-seed.ocvcoin.coop.");
vSeeds.emplace_back("signet-seed.ocvcoin.kg.");
vSeeds.emplace_back("signet-seed.ocvcoin.nagoya.");
vSeeds.emplace_back("signet-seed.ocvcoin.cu.");
vSeeds.emplace_back("signet-seed.ocvcoin.mr.");
vSeeds.emplace_back("signet-seed.ocvcoin.radio.");
vSeeds.emplace_back("signet-seed.ocvcoin.aw.");
vSeeds.emplace_back("signet-seed.ocvcoin.melbourne.");
vSeeds.emplace_back("signet-seed.ocvcoin.brussels.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ar.");
vSeeds.emplace_back("signet-seed.ocvcoin.jo.");
vSeeds.emplace_back("signet-seed.ocvcoin.uz.");
vSeeds.emplace_back("signet-seed.ocvcoin.sl.");
vSeeds.emplace_back("signet-seed.ocvcoin.ve.");
vSeeds.emplace_back("signet-seed.ocvcoin.luxe.");
vSeeds.emplace_back("signet-seed.ocvcoin.dm.");
vSeeds.emplace_back("signet-seed.ocvcoin.gi.");
vSeeds.emplace_back("signet-seed.ocvcoin.gq.");
vSeeds.emplace_back("signet-seed.ocvcoin.tz.");
vSeeds.emplace_back("signet-seed.ocvcoin.sy.");
vSeeds.emplace_back("signet-seed.ocvcoin.nrw.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.tw.");
vSeeds.emplace_back("signet-seed.ocvcoin.bf.");
vSeeds.emplace_back("signet-seed.ocvcoin.realestate.");
vSeeds.emplace_back("signet-seed.ocvcoin.cologne.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ag.");
vSeeds.emplace_back("signet-seed.ocvcoin.uy.");
vSeeds.emplace_back("signet-seed.ocvcoin.gp.");
vSeeds.emplace_back("signet-seed.ocvcoin.sv.");
vSeeds.emplace_back("signet-seed.ocvcoin.boo.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.rw.");
vSeeds.emplace_back("signet-seed.ocvcoin.hamburg.");
vSeeds.emplace_back("signet-seed.ocvcoin.meme.");
vSeeds.emplace_back("signet-seed.ocvcoin.ong.");
vSeeds.emplace_back("signet-seed.ocvcoin.bzh.");
vSeeds.emplace_back("signet-seed.ocvcoin.gt.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.ag.");
vSeeds.emplace_back("signet-seed.ocvcoin.ss.");
vSeeds.emplace_back("signet-seed.ocvcoin.ltda.");
vSeeds.emplace_back("signet-seed.ocvcoin.quest.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.es.");
vSeeds.emplace_back("signet-seed.ocvcoin.physio.");
vSeeds.emplace_back("signet-seed.ocvcoin.gm.");
vSeeds.emplace_back("signet-seed.ocvcoin.eus.");
vSeeds.emplace_back("signet-seed.ocvcoin.vlaanderen.");
vSeeds.emplace_back("signet-seed.ocvcoin.sport.");
vSeeds.emplace_back("signet-seed.ocvcoin.durban.");
vSeeds.emplace_back("signet-seed.ocvcoin.hiv.");
vSeeds.emplace_back("signet-seed.ocvcoin.gf.");
vSeeds.emplace_back("signet-seed.ocvcoin.saarland.");
vSeeds.emplace_back("signet-seed.ocvcoin.eg.");
vSeeds.emplace_back("signet-seed.ocvcoin.ye.");
vSeeds.emplace_back("signet-seed.ocvcoin.ruhr.");
vSeeds.emplace_back("signet-seed.ocvcoin.ryukyu.");
vSeeds.emplace_back("signet-seed.ocvcoin.sydney.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.sg.");
vSeeds.emplace_back("signet-seed.ocvcoin.koeln.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.bo.");
vSeeds.emplace_back("signet-seed.ocvcoin.py.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.at.");
vSeeds.emplace_back("signet-seed.ocvcoin.osaka.");
vSeeds.emplace_back("signet-seed.ocvcoin.vi.");
vSeeds.emplace_back("signet-seed.ocvcoin.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.gal.");
vSeeds.emplace_back("signet-seed.ocvcoin.or.at.");
vSeeds.emplace_back("signet-seed.ocvcoin.now.");
vSeeds.emplace_back("signet-seed.ocvcoin.tj.");
vSeeds.emplace_back("signet-seed.ocvcoin.kn.");
vSeeds.emplace_back("signet-seed.ocvcoin.tr.");
vSeeds.emplace_back("signet-seed.ocvcoin.swiss.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.vc.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.id.");
vSeeds.emplace_back("signet-seed.ocvcoin.rio.");
vSeeds.emplace_back("signet-seed.ocvcoin.cg.");
vSeeds.emplace_back("signet-seed.ocvcoin.forex.");
vSeeds.emplace_back("signet-seed.ocvcoin.food.");
vSeeds.emplace_back("signet-seed.ocvcoin.ren.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.nz.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.pt.");
vSeeds.emplace_back("signet-seed.ocvcoin.il.");
vSeeds.emplace_back("signet-seed.ocvcoin.mw.");
vSeeds.emplace_back("signet-seed.ocvcoin.nc.");
vSeeds.emplace_back("signet-seed.ocvcoin.tirol.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.okinawa.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.nz.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.af.");
vSeeds.emplace_back("signet-seed.ocvcoin.info.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.foo.");
vSeeds.emplace_back("signet-seed.ocvcoin.museum.");
vSeeds.emplace_back("signet-seed.ocvcoin.gen.in.");
vSeeds.emplace_back("signet-seed.ocvcoin.alsace.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.hk.");
vSeeds.emplace_back("signet-seed.ocvcoin.mv.");
vSeeds.emplace_back("signet-seed.ocvcoin.sd.");
vSeeds.emplace_back("signet-seed.ocvcoin.trust.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.int.");
vSeeds.emplace_back("signet-seed.ocvcoin.joburg.");
vSeeds.emplace_back("signet-seed.ocvcoin.makeup.");
vSeeds.emplace_back("signet-seed.ocvcoin.hm.");
vSeeds.emplace_back("signet-seed.ocvcoin.firm.in.");
vSeeds.emplace_back("signet-seed.ocvcoin.music.");
vSeeds.emplace_back("signet-seed.ocvcoin.yachts.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.es.");
vSeeds.emplace_back("signet-seed.ocvcoin.gent.");
vSeeds.emplace_back("signet-seed.ocvcoin.data.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.py.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.ru.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ni.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.bz.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.sa.");
vSeeds.emplace_back("signet-seed.ocvcoin.ind.in.");
vSeeds.emplace_back("signet-seed.ocvcoin.beauty.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.th.");
vSeeds.emplace_back("signet-seed.ocvcoin.motorcycles.");
vSeeds.emplace_back("signet-seed.ocvcoin.gw.");
vSeeds.emplace_back("signet-seed.ocvcoin.bw.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.cr.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.insurance.");
vSeeds.emplace_back("signet-seed.ocvcoin.basketball.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.pn.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.pe.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.abc.");
vSeeds.emplace_back("signet-seed.ocvcoin.bm.");
vSeeds.emplace_back("signet-seed.ocvcoin.dad.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.sc.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ph.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ec.");
vSeeds.emplace_back("signet-seed.ocvcoin.istanbul.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.jp.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.cm.");
vSeeds.emplace_back("signet-seed.ocvcoin.frl.");
vSeeds.emplace_back("signet-seed.ocvcoin.nr.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.ke.");
vSeeds.emplace_back("signet-seed.ocvcoin.in.th.");
vSeeds.emplace_back("signet-seed.ocvcoin.whoswho.");
vSeeds.emplace_back("signet-seed.ocvcoin.free.");
vSeeds.emplace_back("signet-seed.ocvcoin.web.tr.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.ru.");
vSeeds.emplace_back("signet-seed.ocvcoin.versicherung.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.sb.");
vSeeds.emplace_back("signet-seed.ocvcoin.bt.");
vSeeds.emplace_back("signet-seed.ocvcoin.you.");
vSeeds.emplace_back("signet-seed.ocvcoin.taipei.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ng.");
vSeeds.emplace_back("signet-seed.ocvcoin.moscow.");
vSeeds.emplace_back("signet-seed.ocvcoin.biz.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.nl.");
vSeeds.emplace_back("signet-seed.ocvcoin.spa.");
vSeeds.emplace_back("signet-seed.ocvcoin.cw.");
vSeeds.emplace_back("signet-seed.ocvcoin.book.");
vSeeds.emplace_back("signet-seed.ocvcoin.gh.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.kr.");
vSeeds.emplace_back("signet-seed.ocvcoin.lotto.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.je.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.ph.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.za.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.pr.");
vSeeds.emplace_back("signet-seed.ocvcoin.nexus.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ve.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.hr.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.cy.");
vSeeds.emplace_back("signet-seed.ocvcoin.id.au.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.za.");
vSeeds.emplace_back("signet-seed.ocvcoin.kids.");
vSeeds.emplace_back("signet-seed.ocvcoin.gn.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.il.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.gg.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ro.");
vSeeds.emplace_back("signet-seed.ocvcoin.mq.");
vSeeds.emplace_back("signet-seed.ocvcoin.dealer.");
vSeeds.emplace_back("signet-seed.ocvcoin.skin.");
vSeeds.emplace_back("signet-seed.ocvcoin.pf.");
vSeeds.emplace_back("signet-seed.ocvcoin.kyoto.");
vSeeds.emplace_back("signet-seed.ocvcoin.google.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.gy.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.my.");
vSeeds.emplace_back("signet-seed.ocvcoin.idv.tw.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.do.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.gt.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.pa.");
vSeeds.emplace_back("signet-seed.ocvcoin.phd.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ai.");
vSeeds.emplace_back("signet-seed.ocvcoin.corsica.");
vSeeds.emplace_back("signet-seed.ocvcoin.meet.");
vSeeds.emplace_back("signet-seed.ocvcoin.gov.");
vSeeds.emplace_back("signet-seed.ocvcoin.reit.");
vSeeds.emplace_back("signet-seed.ocvcoin.krd.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.ve.");
vSeeds.emplace_back("signet-seed.ocvcoin.madrid.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.au.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.lc.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.mx.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.im.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.rs.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.ph.");
vSeeds.emplace_back("signet-seed.ocvcoin.mov.");
vSeeds.emplace_back("signet-seed.ocvcoin.ltd.uk.");
vSeeds.emplace_back("signet-seed.ocvcoin.case.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.pk.");
vSeeds.emplace_back("signet-seed.ocvcoin.rsvp.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.au.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.uy.");
vSeeds.emplace_back("signet-seed.ocvcoin.man.");
vSeeds.emplace_back("signet-seed.ocvcoin.play.");
vSeeds.emplace_back("signet-seed.ocvcoin.dot.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.pe.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ly.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.pe.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.gh.");
vSeeds.emplace_back("signet-seed.ocvcoin.pay.");
vSeeds.emplace_back("signet-seed.ocvcoin.hair.");
vSeeds.emplace_back("signet-seed.ocvcoin.moi.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.mt.");
vSeeds.emplace_back("signet-seed.ocvcoin.biz.id.");
vSeeds.emplace_back("signet-seed.ocvcoin.kiwi.nz.");
vSeeds.emplace_back("signet-seed.ocvcoin.aarp.");
vSeeds.emplace_back("signet-seed.ocvcoin.edu.rs.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.so.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.sv.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.gr.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ht.");
vSeeds.emplace_back("signet-seed.ocvcoin.xin.");
vSeeds.emplace_back("signet-seed.ocvcoin.med.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.hu.");
vSeeds.emplace_back("signet-seed.ocvcoin.zuerich.");
vSeeds.emplace_back("signet-seed.ocvcoin.giving.");
vSeeds.emplace_back("signet-seed.ocvcoin.gop.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.ao.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.bi.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ee.");
vSeeds.emplace_back("signet-seed.ocvcoin.apple.");
vSeeds.emplace_back("signet-seed.ocvcoin.gen.tr.");
vSeeds.emplace_back("signet-seed.ocvcoin.pharmacy.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.lv.");
vSeeds.emplace_back("signet-seed.ocvcoin.wow.");
vSeeds.emplace_back("signet-seed.ocvcoin.rugby.");
vSeeds.emplace_back("signet-seed.ocvcoin.msk.ru.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.tw.");
vSeeds.emplace_back("signet-seed.ocvcoin.zero.");
vSeeds.emplace_back("signet-seed.ocvcoin.biz.tr.");
vSeeds.emplace_back("signet-seed.ocvcoin.va.");
vSeeds.emplace_back("signet-seed.ocvcoin.select.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.vn.");
vSeeds.emplace_back("signet-seed.ocvcoin.prof.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.tt.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.so.");
vSeeds.emplace_back("signet-seed.ocvcoin.waw.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.pk.");
vSeeds.emplace_back("signet-seed.ocvcoin.mz.");
vSeeds.emplace_back("signet-seed.ocvcoin.here.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.my.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.vn.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.bh.");
vSeeds.emplace_back("signet-seed.ocvcoin.gu.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.ug.");
vSeeds.emplace_back("signet-seed.ocvcoin.web.id.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.tz.");
vSeeds.emplace_back("signet-seed.ocvcoin.or.kr.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.nf.");
vSeeds.emplace_back("signet-seed.ocvcoin.esq.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ms.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.by.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.tn.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.pk.");
vSeeds.emplace_back("signet-seed.ocvcoin.bn.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.my.");
vSeeds.emplace_back("signet-seed.ocvcoin.able.");
vSeeds.emplace_back("signet-seed.ocvcoin.ads.");
vSeeds.emplace_back("signet-seed.ocvcoin.eat.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.mk.");
vSeeds.emplace_back("signet-seed.ocvcoin.tv.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.info.tr.");
vSeeds.emplace_back("signet-seed.ocvcoin.in.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.ac.nz.");
vSeeds.emplace_back("signet-seed.ocvcoin.kiev.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.ice.");
vSeeds.emplace_back("signet-seed.ocvcoin.cpa.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.eg.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.ma.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.lb.");
vSeeds.emplace_back("signet-seed.ocvcoin.spb.ru.");
vSeeds.emplace_back("signet-seed.ocvcoin.map.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.kz.");
vSeeds.emplace_back("signet-seed.ocvcoin.amazon.");
vSeeds.emplace_back("signet-seed.ocvcoin.gen.nz.");
vSeeds.emplace_back("signet-seed.ocvcoin.geek.nz.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.fj.");
vSeeds.emplace_back("signet-seed.ocvcoin.watches.");
vSeeds.emplace_back("signet-seed.ocvcoin.km.");
vSeeds.emplace_back("signet-seed.ocvcoin.edu.vn.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.mz.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.kw.");
vSeeds.emplace_back("signet-seed.ocvcoin.abbott.");
vSeeds.emplace_back("signet-seed.ocvcoin.diy.");
vSeeds.emplace_back("signet-seed.ocvcoin.tatar.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.gp.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.na.");
vSeeds.emplace_back("signet-seed.ocvcoin.fox.");
vSeeds.emplace_back("signet-seed.ocvcoin.search.");
vSeeds.emplace_back("signet-seed.ocvcoin.ind.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.arpa.");
vSeeds.emplace_back("signet-seed.ocvcoin.open.");
vSeeds.emplace_back("signet-seed.ocvcoin.fly.");
vSeeds.emplace_back("signet-seed.ocvcoin.eco.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.living.");
vSeeds.emplace_back("signet-seed.ocvcoin.lifestyle.");
vSeeds.emplace_back("signet-seed.ocvcoin.secure.");
vSeeds.emplace_back("signet-seed.ocvcoin.abb.");
vSeeds.emplace_back("signet-seed.ocvcoin.zm.");
vSeeds.emplace_back("signet-seed.ocvcoin.sky.");
vSeeds.emplace_back("signet-seed.ocvcoin.docs.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.vi.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.ar.");
vSeeds.emplace_back("signet-seed.ocvcoin.idv.hk.");
vSeeds.emplace_back("signet-seed.ocvcoin.ne.jp.");
vSeeds.emplace_back("signet-seed.ocvcoin.youtube.");
vSeeds.emplace_back("signet-seed.ocvcoin.goo.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.jo.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.bw.");
vSeeds.emplace_back("signet-seed.ocvcoin.post.");
vSeeds.emplace_back("signet-seed.ocvcoin.star.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.ls.");
vSeeds.emplace_back("signet-seed.ocvcoin.school.nz.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ge.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.lk.");
vSeeds.emplace_back("signet-seed.ocvcoin.lr.");
vSeeds.emplace_back("signet-seed.ocvcoin.aws.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.gi.");
vSeeds.emplace_back("signet-seed.ocvcoin.name.tr.");
vSeeds.emplace_back("signet-seed.ocvcoin.zw.");
vSeeds.emplace_back("signet-seed.ocvcoin.creditunion.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.zw.");
vSeeds.emplace_back("signet-seed.ocvcoin.sh.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.next.");
vSeeds.emplace_back("signet-seed.ocvcoin.kp.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.tj.");
vSeeds.emplace_back("signet-seed.ocvcoin.fj.");
vSeeds.emplace_back("signet-seed.ocvcoin.ping.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ci.");
vSeeds.emplace_back("signet-seed.ocvcoin.analytics.");
vSeeds.emplace_back("signet-seed.ocvcoin.lb.");
vSeeds.emplace_back("signet-seed.ocvcoin.tvs.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.zm.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.sl.");
vSeeds.emplace_back("signet-seed.ocvcoin.buy.");
vSeeds.emplace_back("signet-seed.ocvcoin.vana.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.lk.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.hk.");
vSeeds.emplace_back("signet-seed.ocvcoin.abbvie.");
vSeeds.emplace_back("signet-seed.ocvcoin.drive.");
vSeeds.emplace_back("signet-seed.ocvcoin.hot.");
vSeeds.emplace_back("signet-seed.ocvcoin.hk.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.tw.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.talk.");
vSeeds.emplace_back("signet-seed.ocvcoin.or.jp.");
vSeeds.emplace_back("signet-seed.ocvcoin.mobile.");
vSeeds.emplace_back("signet-seed.ocvcoin.ks.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.room.");
vSeeds.emplace_back("signet-seed.ocvcoin.pro.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.fast.");
vSeeds.emplace_back("signet-seed.ocvcoin.wiki.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.sz.");
vSeeds.emplace_back("signet-seed.ocvcoin.java.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.tr.");
vSeeds.emplace_back("signet-seed.ocvcoin.office.");
vSeeds.emplace_back("signet-seed.ocvcoin.art.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.abudhabi.");
vSeeds.emplace_back("signet-seed.ocvcoin.bentley.");
vSeeds.emplace_back("signet-seed.ocvcoin.cal.");
vSeeds.emplace_back("signet-seed.ocvcoin.smart.");
vSeeds.emplace_back("signet-seed.ocvcoin.gmail.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.et.");
vSeeds.emplace_back("signet-seed.ocvcoin.bj.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.bbc.");
vSeeds.emplace_back("signet-seed.ocvcoin.hi.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.ott.");
vSeeds.emplace_back("signet-seed.ocvcoin.prod.");
vSeeds.emplace_back("signet-seed.ocvcoin.android.");
vSeeds.emplace_back("signet-seed.ocvcoin.gle.");
vSeeds.emplace_back("signet-seed.ocvcoin.ntt.");
vSeeds.emplace_back("signet-seed.ocvcoin.kw.");
vSeeds.emplace_back("signet-seed.ocvcoin.gb.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.qa.");
vSeeds.emplace_back("signet-seed.ocvcoin.joy.");
vSeeds.emplace_back("signet-seed.ocvcoin.re.kr.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.ng.");
vSeeds.emplace_back("signet-seed.ocvcoin.ally.");
vSeeds.emplace_back("signet-seed.ocvcoin.lds.");
vSeeds.emplace_back("signet-seed.ocvcoin.safe.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.ai.");
vSeeds.emplace_back("signet-seed.ocvcoin.fire.");
vSeeds.emplace_back("signet-seed.ocvcoin.org.gr.");
vSeeds.emplace_back("signet-seed.ocvcoin.mil.");
vSeeds.emplace_back("signet-seed.ocvcoin.azure.");
vSeeds.emplace_back("signet-seed.ocvcoin.catholic.");
vSeeds.emplace_back("signet-seed.ocvcoin.ups.");
vSeeds.emplace_back("signet-seed.ocvcoin.nieruchomosci.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.read.");
vSeeds.emplace_back("signet-seed.ocvcoin.netflix.");
vSeeds.emplace_back("signet-seed.ocvcoin.deal.");
vSeeds.emplace_back("signet-seed.ocvcoin.hotels.");
vSeeds.emplace_back("signet-seed.ocvcoin.anz.");
vSeeds.emplace_back("signet-seed.ocvcoin.pioneer.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.ai.");
vSeeds.emplace_back("signet-seed.ocvcoin.warszawa.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.if.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.spot.");
vSeeds.emplace_back("signet-seed.ocvcoin.accenture.");
vSeeds.emplace_back("signet-seed.ocvcoin.alibaba.");
vSeeds.emplace_back("signet-seed.ocvcoin.americanexpress.");
vSeeds.emplace_back("signet-seed.ocvcoin.circle.");
vSeeds.emplace_back("signet-seed.ocvcoin.cv.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.gmo.");
vSeeds.emplace_back("signet-seed.ocvcoin.pe.kr.");
vSeeds.emplace_back("signet-seed.ocvcoin.off.ai.");
vSeeds.emplace_back("signet-seed.ocvcoin.arte.");
vSeeds.emplace_back("signet-seed.ocvcoin.arab.");
vSeeds.emplace_back("signet-seed.ocvcoin.nowruz.");
vSeeds.emplace_back("signet-seed.ocvcoin.ieee.");
vSeeds.emplace_back("signet-seed.ocvcoin.sklep.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.realtor.");
vSeeds.emplace_back("signet-seed.ocvcoin.bv.");
vSeeds.emplace_back("signet-seed.ocvcoin.phone.");
vSeeds.emplace_back("signet-seed.ocvcoin.call.");
vSeeds.emplace_back("signet-seed.ocvcoin.wroclaw.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.mh.");
vSeeds.emplace_back("signet-seed.ocvcoin.shell.");
vSeeds.emplace_back("signet-seed.ocvcoin.sn.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.ps.");
vSeeds.emplace_back("signet-seed.ocvcoin.xbox.");
vSeeds.emplace_back("signet-seed.ocvcoin.barefoot.");
vSeeds.emplace_back("signet-seed.ocvcoin.fj.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.com.nr.");
vSeeds.emplace_back("signet-seed.ocvcoin.george.");
vSeeds.emplace_back("signet-seed.ocvcoin.goog.");
vSeeds.emplace_back("signet-seed.ocvcoin.audi.");
vSeeds.emplace_back("signet-seed.ocvcoin.virgin.");
vSeeds.emplace_back("signet-seed.ocvcoin.nm.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.microsoft.");
vSeeds.emplace_back("signet-seed.ocvcoin.gx.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.edu.lk.");
vSeeds.emplace_back("signet-seed.ocvcoin.booking.");
vSeeds.emplace_back("signet-seed.ocvcoin.airbus.");
vSeeds.emplace_back("signet-seed.ocvcoin.mango.");
vSeeds.emplace_back("signet-seed.ocvcoin.mo.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.aco.");
vSeeds.emplace_back("signet-seed.ocvcoin.bmw.");
vSeeds.emplace_back("signet-seed.ocvcoin.song.");
vSeeds.emplace_back("signet-seed.ocvcoin.sx.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.gz.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.aol.");
vSeeds.emplace_back("signet-seed.ocvcoin.rec.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.like.");
vSeeds.emplace_back("signet-seed.ocvcoin.redstone.");
vSeeds.emplace_back("signet-seed.ocvcoin.wed.");
vSeeds.emplace_back("signet-seed.ocvcoin.bestbuy.");
vSeeds.emplace_back("signet-seed.ocvcoin.yandex.");
vSeeds.emplace_back("signet-seed.ocvcoin.hn.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.gd.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.gs.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.js.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.yn.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.obi.");
vSeeds.emplace_back("signet-seed.ocvcoin.srv.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.dubai.");
vSeeds.emplace_back("signet-seed.ocvcoin.omega.");
vSeeds.emplace_back("signet-seed.ocvcoin.save.");
vSeeds.emplace_back("signet-seed.ocvcoin.hl.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.esp.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.author.");
vSeeds.emplace_back("signet-seed.ocvcoin.tab.");
vSeeds.emplace_back("signet-seed.ocvcoin.godaddy.");
vSeeds.emplace_back("signet-seed.ocvcoin.mini.");
vSeeds.emplace_back("signet-seed.ocvcoin.windows.");
vSeeds.emplace_back("signet-seed.ocvcoin.yun.");
vSeeds.emplace_back("signet-seed.ocvcoin.nico.");
vSeeds.emplace_back("signet-seed.ocvcoin.mint.");
vSeeds.emplace_back("signet-seed.ocvcoin.or.id.");
vSeeds.emplace_back("signet-seed.ocvcoin.dds.");
vSeeds.emplace_back("signet-seed.ocvcoin.beats.");
vSeeds.emplace_back("signet-seed.ocvcoin.ggee.");
vSeeds.emplace_back("signet-seed.ocvcoin.cq.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.gucci.");
vSeeds.emplace_back("signet-seed.ocvcoin.bofa.");
vSeeds.emplace_back("signet-seed.ocvcoin.smile.");
vSeeds.emplace_back("signet-seed.ocvcoin.sj.");
vSeeds.emplace_back("signet-seed.ocvcoin.prime.");
vSeeds.emplace_back("signet-seed.ocvcoin.amex.");
vSeeds.emplace_back("signet-seed.ocvcoin.imb.br.");
vSeeds.emplace_back("signet-seed.ocvcoin.ollo.");
vSeeds.emplace_back("signet-seed.ocvcoin.orange.");
vSeeds.emplace_back("signet-seed.ocvcoin.aetna.");
vSeeds.emplace_back("signet-seed.ocvcoin.got.");
vSeeds.emplace_back("signet-seed.ocvcoin.pid.");
vSeeds.emplace_back("signet-seed.ocvcoin.walmart.");
vSeeds.emplace_back("signet-seed.ocvcoin.ac.cn.");
vSeeds.emplace_back("signet-seed.ocvcoin.seven.");
vSeeds.emplace_back("signet-seed.ocvcoin.pin.");
vSeeds.emplace_back("signet-seed.ocvcoin.comsec.");
vSeeds.emplace_back("signet-seed.ocvcoin.farmers.");
vSeeds.emplace_back("signet-seed.ocvcoin.tokyo.jp.");
vSeeds.emplace_back("signet-seed.ocvcoin.afl.");
vSeeds.emplace_back("signet-seed.ocvcoin.calvinklein.");
vSeeds.emplace_back("signet-seed.ocvcoin.kosher.");
vSeeds.emplace_back("signet-seed.ocvcoin.asda.");
vSeeds.emplace_back("signet-seed.ocvcoin.eurovision.");
vSeeds.emplace_back("signet-seed.ocvcoin.bauhaus.");
vSeeds.emplace_back("signet-seed.ocvcoin.discover.");
vSeeds.emplace_back("signet-seed.ocvcoin.chrome.");
vSeeds.emplace_back("signet-seed.ocvcoin.wtc.");
vSeeds.emplace_back("signet-seed.ocvcoin.brother.");
vSeeds.emplace_back("signet-seed.ocvcoin.locker.");
vSeeds.emplace_back("signet-seed.ocvcoin.moto.");
vSeeds.emplace_back("signet-seed.ocvcoin.name.my.");
vSeeds.emplace_back("signet-seed.ocvcoin.bom.");
vSeeds.emplace_back("signet-seed.ocvcoin.lego.");
vSeeds.emplace_back("signet-seed.ocvcoin.axa.");
vSeeds.emplace_back("signet-seed.ocvcoin.cern.");
vSeeds.emplace_back("signet-seed.ocvcoin.ibm.");
vSeeds.emplace_back("signet-seed.ocvcoin.tui.");
vSeeds.emplace_back("signet-seed.ocvcoin.uol.");
vSeeds.emplace_back("signet-seed.ocvcoin.safety.");
vSeeds.emplace_back("signet-seed.ocvcoin.alipay.");
vSeeds.emplace_back("signet-seed.ocvcoin.kred.");
vSeeds.emplace_back("signet-seed.ocvcoin.williamhill.");
vSeeds.emplace_back("signet-seed.ocvcoin.tkmaxx.");
vSeeds.emplace_back("signet-seed.ocvcoin.lidl.");
vSeeds.emplace_back("signet-seed.ocvcoin.weather.");
vSeeds.emplace_back("signet-seed.ocvcoin.yahoo.");
vSeeds.emplace_back("signet-seed.ocvcoin.bradesco.");
vSeeds.emplace_back("signet-seed.ocvcoin.seek.");
vSeeds.emplace_back("signet-seed.ocvcoin.mit.");
vSeeds.emplace_back("signet-seed.ocvcoin.skype.");
vSeeds.emplace_back("signet-seed.ocvcoin.dhl.");
vSeeds.emplace_back("signet-seed.ocvcoin.agakhan.");
vSeeds.emplace_back("signet-seed.ocvcoin.zara.");
vSeeds.emplace_back("signet-seed.ocvcoin.americanfamily.");
vSeeds.emplace_back("signet-seed.ocvcoin.bloomberg.");
vSeeds.emplace_back("signet-seed.ocvcoin.playstation.");
vSeeds.emplace_back("signet-seed.ocvcoin.canon.");
vSeeds.emplace_back("signet-seed.ocvcoin.ftr.");
vSeeds.emplace_back("signet-seed.ocvcoin.jio.");
vSeeds.emplace_back("signet-seed.ocvcoin.viking.");
vSeeds.emplace_back("signet-seed.ocvcoin.baseball.");
vSeeds.emplace_back("signet-seed.ocvcoin.blockbuster.");
vSeeds.emplace_back("signet-seed.ocvcoin.bnpparibas.");
vSeeds.emplace_back("signet-seed.ocvcoin.aeg.");
vSeeds.emplace_back("signet-seed.ocvcoin.hangout.");
vSeeds.emplace_back("signet-seed.ocvcoin.sbi.");
vSeeds.emplace_back("signet-seed.ocvcoin.schwarz.");
vSeeds.emplace_back("signet-seed.ocvcoin.vanguard.");
vSeeds.emplace_back("signet-seed.ocvcoin.visa.");
vSeeds.emplace_back("signet-seed.ocvcoin.edeka.");
vSeeds.emplace_back("signet-seed.ocvcoin.lamborghini.");
vSeeds.emplace_back("signet-seed.ocvcoin.mma.");
vSeeds.emplace_back("signet-seed.ocvcoin.sakura.");
vSeeds.emplace_back("signet-seed.ocvcoin.globo.");
vSeeds.emplace_back("signet-seed.ocvcoin.sas.");
vSeeds.emplace_back("signet-seed.ocvcoin.aig.");
vSeeds.emplace_back("signet-seed.ocvcoin.llp.");
vSeeds.emplace_back("signet-seed.ocvcoin.airtel.");
vSeeds.emplace_back("signet-seed.ocvcoin.baidu.");
vSeeds.emplace_back("signet-seed.ocvcoin.crown.");
vSeeds.emplace_back("signet-seed.ocvcoin.ferrari.");
vSeeds.emplace_back("signet-seed.ocvcoin.travelersinsurance.");
vSeeds.emplace_back("signet-seed.ocvcoin.tunes.");
vSeeds.emplace_back("signet-seed.ocvcoin.extraspace.");
vSeeds.emplace_back("signet-seed.ocvcoin.hotmail.");
vSeeds.emplace_back("signet-seed.ocvcoin.coupon.");
vSeeds.emplace_back("signet-seed.ocvcoin.barclays.");
vSeeds.emplace_back("signet-seed.ocvcoin.delta.");
vSeeds.emplace_back("signet-seed.ocvcoin.bing.");
vSeeds.emplace_back("signet-seed.ocvcoin.cruise.");
vSeeds.emplace_back("signet-seed.ocvcoin.aquarelle.");
vSeeds.emplace_back("signet-seed.ocvcoin.gmx.");
vSeeds.emplace_back("signet-seed.ocvcoin.seat.");
vSeeds.emplace_back("signet-seed.ocvcoin.ubs.");
vSeeds.emplace_back("signet-seed.ocvcoin.sap.");
vSeeds.emplace_back("signet-seed.ocvcoin.latino.");
vSeeds.emplace_back("signet-seed.ocvcoin.viva.");
vSeeds.emplace_back("signet-seed.ocvcoin.bcn.");
vSeeds.emplace_back("signet-seed.ocvcoin.final.");
vSeeds.emplace_back("signet-seed.ocvcoin.unicom.");
vSeeds.emplace_back("signet-seed.ocvcoin.samsung.");
vSeeds.emplace_back("signet-seed.ocvcoin.origins.");
vSeeds.emplace_back("signet-seed.ocvcoin.erni.");
vSeeds.emplace_back("signet-seed.ocvcoin.pwc.");
vSeeds.emplace_back("signet-seed.ocvcoin.ericsson.");
vSeeds.emplace_back("signet-seed.ocvcoin.mormon.");
vSeeds.emplace_back("signet-seed.ocvcoin.weber.");
vSeeds.emplace_back("signet-seed.ocvcoin.citadel.");
vSeeds.emplace_back("signet-seed.ocvcoin.deloitte.");
vSeeds.emplace_back("signet-seed.ocvcoin.cba.");
vSeeds.emplace_back("signet-seed.ocvcoin.total.");
vSeeds.emplace_back("signet-seed.ocvcoin.imdb.");
vSeeds.emplace_back("signet-seed.ocvcoin.auspost.");
vSeeds.emplace_back("signet-seed.ocvcoin.sony.");
vSeeds.emplace_back("signet-seed.ocvcoin.chanel.");
vSeeds.emplace_back("signet-seed.ocvcoin.oracle.");
vSeeds.emplace_back("signet-seed.ocvcoin.hermes.");
vSeeds.emplace_back("signet-seed.ocvcoin.homedepot.");
vSeeds.emplace_back("signet-seed.ocvcoin.infiniti.");
vSeeds.emplace_back("signet-seed.ocvcoin.datsun.");
vSeeds.emplace_back("signet-seed.ocvcoin.gap.");
vSeeds.emplace_back("signet-seed.ocvcoin.itv.");
vSeeds.emplace_back("signet-seed.ocvcoin.natura.");
vSeeds.emplace_back("signet-seed.ocvcoin.toyota.");
vSeeds.emplace_back("signet-seed.ocvcoin.audible.");
vSeeds.emplace_back("signet-seed.ocvcoin.cisco.");
vSeeds.emplace_back("signet-seed.ocvcoin.sncf.");
vSeeds.emplace_back("signet-seed.ocvcoin.cbn.");
vSeeds.emplace_back("signet-seed.ocvcoin.fage.");
vSeeds.emplace_back("signet-seed.ocvcoin.leclerc.");
vSeeds.emplace_back("signet-seed.ocvcoin.pfizer.");
vSeeds.emplace_back("signet-seed.ocvcoin.stockholm.");
vSeeds.emplace_back("signet-seed.ocvcoin.winners.");
vSeeds.emplace_back("signet-seed.ocvcoin.jpmorgan.");
vSeeds.emplace_back("signet-seed.ocvcoin.kpmg.");
vSeeds.emplace_back("signet-seed.ocvcoin.schmidt.");
vSeeds.emplace_back("signet-seed.ocvcoin.akdn.");
vSeeds.emplace_back("signet-seed.ocvcoin.helsinki.");
vSeeds.emplace_back("signet-seed.ocvcoin.jmp.");
vSeeds.emplace_back("signet-seed.ocvcoin.crs.");
vSeeds.emplace_back("signet-seed.ocvcoin.weibo.");
vSeeds.emplace_back("signet-seed.ocvcoin.bharti.");
vSeeds.emplace_back("signet-seed.ocvcoin.sharp.");
vSeeds.emplace_back("signet-seed.ocvcoin.nike.");
vSeeds.emplace_back("signet-seed.ocvcoin.vivo.");
vSeeds.emplace_back("signet-seed.ocvcoin.walter.");
vSeeds.emplace_back("signet-seed.ocvcoin.weir.");
vSeeds.emplace_back("signet-seed.ocvcoin.itau.");
vSeeds.emplace_back("signet-seed.ocvcoin.allstate.");
vSeeds.emplace_back("signet-seed.ocvcoin.aramco.");
vSeeds.emplace_back("signet-seed.ocvcoin.broadway.");
vSeeds.emplace_back("signet-seed.ocvcoin.bridgestone.");
vSeeds.emplace_back("signet-seed.ocvcoin.hsbc.");
vSeeds.emplace_back("signet-seed.ocvcoin.travelers.");
vSeeds.emplace_back("signet-seed.ocvcoin.capitalone.");
vSeeds.emplace_back("signet-seed.ocvcoin.lamer.");
vSeeds.emplace_back("signet-seed.ocvcoin.ipiranga.");
vSeeds.emplace_back("signet-seed.ocvcoin.honda.");
vSeeds.emplace_back("signet-seed.ocvcoin.lilly.");
vSeeds.emplace_back("signet-seed.ocvcoin.ikano.");
vSeeds.emplace_back("signet-seed.ocvcoin.ismaili.");
vSeeds.emplace_back("signet-seed.ocvcoin.nokia.");
vSeeds.emplace_back("signet-seed.ocvcoin.yodobashi.");
vSeeds.emplace_back("signet-seed.ocvcoin.jaguar.");
vSeeds.emplace_back("signet-seed.ocvcoin.bosch.");
vSeeds.emplace_back("signet-seed.ocvcoin.dell.");
vSeeds.emplace_back("signet-seed.ocvcoin.target.");
vSeeds.emplace_back("signet-seed.ocvcoin.xerox.");
vSeeds.emplace_back("signet-seed.ocvcoin.barclaycard.");
vSeeds.emplace_back("signet-seed.ocvcoin.bms.");
vSeeds.emplace_back("signet-seed.ocvcoin.chase.");
vSeeds.emplace_back("signet-seed.ocvcoin.epson.");
vSeeds.emplace_back("signet-seed.ocvcoin.saxo.");
vSeeds.emplace_back("signet-seed.ocvcoin.shia.");
vSeeds.emplace_back("signet-seed.ocvcoin.nra.");
vSeeds.emplace_back("signet-seed.ocvcoin.dnp.");
vSeeds.emplace_back("signet-seed.ocvcoin.fido.");
vSeeds.emplace_back("signet-seed.ocvcoin.tdk.");
vSeeds.emplace_back("signet-seed.ocvcoin.dish.");
vSeeds.emplace_back("signet-seed.ocvcoin.frontier.");
vSeeds.emplace_back("signet-seed.ocvcoin.icbc.");
vSeeds.emplace_back("signet-seed.ocvcoin.kia.");
vSeeds.emplace_back("signet-seed.ocvcoin.politie.");
vSeeds.emplace_back("signet-seed.ocvcoin.sew.");
vSeeds.emplace_back("signet-seed.ocvcoin.jeep.");
vSeeds.emplace_back("signet-seed.ocvcoin.ril.");
vSeeds.emplace_back("signet-seed.ocvcoin.zappos.");
vSeeds.emplace_back("signet-seed.ocvcoin.cuisinella.");
vSeeds.emplace_back("signet-seed.ocvcoin.dvr.");
vSeeds.emplace_back("signet-seed.ocvcoin.sfr.");
vSeeds.emplace_back("signet-seed.ocvcoin.stc.");
vSeeds.emplace_back("signet-seed.ocvcoin.anquan.");
vSeeds.emplace_back("signet-seed.ocvcoin.caravan.");
vSeeds.emplace_back("signet-seed.ocvcoin.juniper.");
vSeeds.emplace_back("signet-seed.ocvcoin.lifeinsurance.");
vSeeds.emplace_back("signet-seed.ocvcoin.monash.");
vSeeds.emplace_back("signet-seed.ocvcoin.neustar.");
vSeeds.emplace_back("signet-seed.ocvcoin.clubmed.");
vSeeds.emplace_back("signet-seed.ocvcoin.dabur.");
vSeeds.emplace_back("signet-seed.ocvcoin.frogans.");
vSeeds.emplace_back("signet-seed.ocvcoin.kpn.");
vSeeds.emplace_back("signet-seed.ocvcoin.lotte.");
vSeeds.emplace_back("signet-seed.ocvcoin.netbank.");
vSeeds.emplace_back("signet-seed.ocvcoin.gea.");
vSeeds.emplace_back("signet-seed.ocvcoin.wme.");
vSeeds.emplace_back("signet-seed.ocvcoin.hitachi.");
vSeeds.emplace_back("signet-seed.ocvcoin.hyundai.");
vSeeds.emplace_back("signet-seed.ocvcoin.praxi.");
vSeeds.emplace_back("signet-seed.ocvcoin.weatherchannel.");
vSeeds.emplace_back("signet-seed.ocvcoin.amica.");
vSeeds.emplace_back("signet-seed.ocvcoin.commbank.");
vSeeds.emplace_back("signet-seed.ocvcoin.otsuka.");
vSeeds.emplace_back("signet-seed.ocvcoin.tjmaxx.");
vSeeds.emplace_back("signet-seed.ocvcoin.woodside.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.tw.");
vSeeds.emplace_back("signet-seed.ocvcoin.mls.");
vSeeds.emplace_back("signet-seed.ocvcoin.mtn.");
vSeeds.emplace_back("signet-seed.ocvcoin.nissan.");
vSeeds.emplace_back("signet-seed.ocvcoin.shangrila.");
vSeeds.emplace_back("signet-seed.ocvcoin.trv.");
vSeeds.emplace_back("signet-seed.ocvcoin.volvo.");
vSeeds.emplace_back("signet-seed.ocvcoin.hughes.");
vSeeds.emplace_back("signet-seed.ocvcoin.kerryhotels.");
vSeeds.emplace_back("signet-seed.ocvcoin.marriott.");
vSeeds.emplace_back("signet-seed.ocvcoin.suzuki.");
vSeeds.emplace_back("signet-seed.ocvcoin.cipriani.");
vSeeds.emplace_back("signet-seed.ocvcoin.dvag.");
vSeeds.emplace_back("signet-seed.ocvcoin.ferrero.");
vSeeds.emplace_back("signet-seed.ocvcoin.samsclub.");
vSeeds.emplace_back("signet-seed.ocvcoin.statefarm.");
vSeeds.emplace_back("signet-seed.ocvcoin.thd.");
vSeeds.emplace_back("signet-seed.ocvcoin.maif.");
vSeeds.emplace_back("signet-seed.ocvcoin.nec.");
vSeeds.emplace_back("signet-seed.ocvcoin.dclk.");
vSeeds.emplace_back("signet-seed.ocvcoin.locus.");
vSeeds.emplace_back("signet-seed.ocvcoin.progressive.");
vSeeds.emplace_back("signet-seed.ocvcoin.lexus.");
vSeeds.emplace_back("signet-seed.ocvcoin.flickr.");
vSeeds.emplace_back("signet-seed.ocvcoin.imamat.");
vSeeds.emplace_back("signet-seed.ocvcoin.silk.");
vSeeds.emplace_back("signet-seed.ocvcoin.wanggou.");
vSeeds.emplace_back("signet-seed.ocvcoin.citi.");
vSeeds.emplace_back("signet-seed.ocvcoin.grocery.");
vSeeds.emplace_back("signet-seed.ocvcoin.lpl.");
vSeeds.emplace_back("signet-seed.ocvcoin.sandvikcoromant.");
vSeeds.emplace_back("signet-seed.ocvcoin.jot.");
vSeeds.emplace_back("signet-seed.ocvcoin.jprs.");
vSeeds.emplace_back("signet-seed.ocvcoin.sina.");
vSeeds.emplace_back("signet-seed.ocvcoin.athleta.");
vSeeds.emplace_back("signet-seed.ocvcoin.banamex.");
vSeeds.emplace_back("signet-seed.ocvcoin.fujitsu.");
vSeeds.emplace_back("signet-seed.ocvcoin.scb.");
vSeeds.emplace_back("signet-seed.ocvcoin.sohu.");
vSeeds.emplace_back("signet-seed.ocvcoin.toray.");
vSeeds.emplace_back("signet-seed.ocvcoin.fedex.");
vSeeds.emplace_back("signet-seed.ocvcoin.komatsu.");
vSeeds.emplace_back("signet-seed.ocvcoin.redumbrella.");
vSeeds.emplace_back("signet-seed.ocvcoin.tmall.");
vSeeds.emplace_back("signet-seed.ocvcoin.verisign.");
vSeeds.emplace_back("signet-seed.ocvcoin.dtv.");
vSeeds.emplace_back("signet-seed.ocvcoin.hkt.");
vSeeds.emplace_back("signet-seed.ocvcoin.taobao.");
vSeeds.emplace_back("signet-seed.ocvcoin.wolterskluwer.");
vSeeds.emplace_back("signet-seed.ocvcoin.ifm.");
vSeeds.emplace_back("signet-seed.ocvcoin.kddi.");
vSeeds.emplace_back("signet-seed.ocvcoin.lacaixa.");
vSeeds.emplace_back("signet-seed.ocvcoin.nab.");
vSeeds.emplace_back("signet-seed.ocvcoin.richardli.");
vSeeds.emplace_back("signet-seed.ocvcoin.lancaster.");
vSeeds.emplace_back("signet-seed.ocvcoin.landrover.");
vSeeds.emplace_back("signet-seed.ocvcoin.sling.");
vSeeds.emplace_back("signet-seed.ocvcoin.alstom.");
vSeeds.emplace_back("signet-seed.ocvcoin.kuokgroup.");
vSeeds.emplace_back("signet-seed.ocvcoin.pars.");
vSeeds.emplace_back("signet-seed.ocvcoin.pohl.");
vSeeds.emplace_back("signet-seed.ocvcoin.xihuan.");
vSeeds.emplace_back("signet-seed.ocvcoin.allfinanz.");
vSeeds.emplace_back("signet-seed.ocvcoin.panasonic.");
vSeeds.emplace_back("signet-seed.ocvcoin.bbva.");
vSeeds.emplace_back("signet-seed.ocvcoin.ford.");
vSeeds.emplace_back("signet-seed.ocvcoin.schaeffler.");
vSeeds.emplace_back("signet-seed.ocvcoin.softbank.");
vSeeds.emplace_back("signet-seed.ocvcoin.yamaxun.");
vSeeds.emplace_back("signet-seed.ocvcoin.hisamitsu.");
vSeeds.emplace_back("signet-seed.ocvcoin.boehringer.");
vSeeds.emplace_back("signet-seed.ocvcoin.kerryproperties.");
vSeeds.emplace_back("signet-seed.ocvcoin.nhk.");
vSeeds.emplace_back("signet-seed.ocvcoin.genting.");
vSeeds.emplace_back("signet-seed.ocvcoin.rwe.");
vSeeds.emplace_back("signet-seed.ocvcoin.scholarships.");
vSeeds.emplace_back("signet-seed.ocvcoin.fidelity.");
vSeeds.emplace_back("signet-seed.ocvcoin.fresenius.");
vSeeds.emplace_back("signet-seed.ocvcoin.jcb.");
vSeeds.emplace_back("signet-seed.ocvcoin.gallo.");
vSeeds.emplace_back("signet-seed.ocvcoin.goodyear.");
vSeeds.emplace_back("signet-seed.ocvcoin.nfl.");
vSeeds.emplace_back("signet-seed.ocvcoin.pnc.");
vSeeds.emplace_back("signet-seed.ocvcoin.cfa.");
vSeeds.emplace_back("signet-seed.ocvcoin.mckinsey.");
vSeeds.emplace_back("signet-seed.ocvcoin.shaw.");
vSeeds.emplace_back("signet-seed.ocvcoin.citic.");
vSeeds.emplace_back("signet-seed.ocvcoin.dupont.");
vSeeds.emplace_back("signet-seed.ocvcoin.fairwinds.");
vSeeds.emplace_back("signet-seed.ocvcoin.kindle.");
vSeeds.emplace_back("signet-seed.ocvcoin.mlb.");
vSeeds.emplace_back("signet-seed.ocvcoin.chintai.");
vSeeds.emplace_back("signet-seed.ocvcoin.emerck.");
vSeeds.emplace_back("signet-seed.ocvcoin.hbo.");
vSeeds.emplace_back("signet-seed.ocvcoin.pru.");
vSeeds.emplace_back("signet-seed.ocvcoin.vig.");
vSeeds.emplace_back("signet-seed.ocvcoin.msd.");
vSeeds.emplace_back("signet-seed.ocvcoin.pccw.");
vSeeds.emplace_back("signet-seed.ocvcoin.philips.");
vSeeds.emplace_back("signet-seed.ocvcoin.goldpoint.");
vSeeds.emplace_back("signet-seed.ocvcoin.jll.");
vSeeds.emplace_back("signet-seed.ocvcoin.lasalle.");
vSeeds.emplace_back("signet-seed.ocvcoin.tci.");
vSeeds.emplace_back("signet-seed.ocvcoin.toshiba.");
vSeeds.emplace_back("signet-seed.ocvcoin.ubank.");
vSeeds.emplace_back("signet-seed.ocvcoin.gbiz.");
vSeeds.emplace_back("signet-seed.ocvcoin.rogers.");
vSeeds.emplace_back("signet-seed.ocvcoin.dunlop.");
vSeeds.emplace_back("signet-seed.ocvcoin.hdfc.");
vSeeds.emplace_back("signet-seed.ocvcoin.cbre.");
vSeeds.emplace_back("signet-seed.ocvcoin.nba.");
vSeeds.emplace_back("signet-seed.ocvcoin.sanofi.");
vSeeds.emplace_back("signet-seed.ocvcoin.mtr.");
vSeeds.emplace_back("signet-seed.ocvcoin.tatamotors.");
vSeeds.emplace_back("signet-seed.ocvcoin.lincoln.");
vSeeds.emplace_back("signet-seed.ocvcoin.firestone.");
vSeeds.emplace_back("signet-seed.ocvcoin.statebank.");
vSeeds.emplace_back("signet-seed.ocvcoin.amfam.");
vSeeds.emplace_back("signet-seed.ocvcoin.mitsubishi.");
vSeeds.emplace_back("signet-seed.ocvcoin.shouji.");
vSeeds.emplace_back("signet-seed.ocvcoin.stada.");
vSeeds.emplace_back("signet-seed.ocvcoin.temasek.");
vSeeds.emplace_back("signet-seed.ocvcoin.rexroth.");
vSeeds.emplace_back("signet-seed.ocvcoin.norton.");
vSeeds.emplace_back("signet-seed.ocvcoin.sandvik.");
vSeeds.emplace_back("signet-seed.ocvcoin.bcg.");
vSeeds.emplace_back("signet-seed.ocvcoin.hyatt.");
vSeeds.emplace_back("signet-seed.ocvcoin.kfh.");
vSeeds.emplace_back("signet-seed.ocvcoin.olayan.");
vSeeds.emplace_back("signet-seed.ocvcoin.web.lk.");
vSeeds.emplace_back("signet-seed.ocvcoin.kerrylogistics.");
vSeeds.emplace_back("signet-seed.ocvcoin.pramerica.");
vSeeds.emplace_back("signet-seed.ocvcoin.sener.");
vSeeds.emplace_back("signet-seed.ocvcoin.firmdale.");
vSeeds.emplace_back("signet-seed.ocvcoin.nowtv.");
vSeeds.emplace_back("signet-seed.ocvcoin.reliance.");
vSeeds.emplace_back("signet-seed.ocvcoin.bbt.");
vSeeds.emplace_back("signet-seed.ocvcoin.lipsy.");
vSeeds.emplace_back("signet-seed.ocvcoin.nikon.");
vSeeds.emplace_back("signet-seed.ocvcoin.nissay.");
vSeeds.emplace_back("signet-seed.ocvcoin.tjx.");
vSeeds.emplace_back("signet-seed.ocvcoin.grainger.");
vSeeds.emplace_back("signet-seed.ocvcoin.latrobe.");
vSeeds.emplace_back("signet-seed.ocvcoin.hotel.lk.");
vSeeds.emplace_back("signet-seed.ocvcoin.lundbeck.");
vSeeds.emplace_back("signet-seed.ocvcoin.intuit.");
vSeeds.emplace_back("signet-seed.ocvcoin.gallup.");
vSeeds.emplace_back("signet-seed.ocvcoin.guge.");
vSeeds.emplace_back("signet-seed.ocvcoin.nextdirect.");
vSeeds.emplace_back("signet-seed.ocvcoin.tushu.");
vSeeds.emplace_back("signet-seed.ocvcoin.clinique.");
vSeeds.emplace_back("signet-seed.ocvcoin.lefrak.");
vSeeds.emplace_back("signet-seed.ocvcoin.tiaa.");
vSeeds.emplace_back("signet-seed.ocvcoin.jnj.");
vSeeds.emplace_back("signet-seed.ocvcoin.swatch.");
vSeeds.emplace_back("signet-seed.ocvcoin.homesense.");
vSeeds.emplace_back("signet-seed.ocvcoin.teva.");
vSeeds.emplace_back("signet-seed.ocvcoin.hdfcbank.");
vSeeds.emplace_back("signet-seed.ocvcoin.prudential.");
vSeeds.emplace_back("signet-seed.ocvcoin.mattel.");
vSeeds.emplace_back("signet-seed.ocvcoin.staples.");
vSeeds.emplace_back("signet-seed.ocvcoin.pictet.");
vSeeds.emplace_back("signet-seed.ocvcoin.ricoh.");
vSeeds.emplace_back("signet-seed.ocvcoin.flir.");
vSeeds.emplace_back("signet-seed.ocvcoin.marshalls.");
vSeeds.emplace_back("signet-seed.ocvcoin.lanxess.");
vSeeds.emplace_back("signet-seed.ocvcoin.stcgroup.");
vSeeds.emplace_back("signet-seed.ocvcoin.bostik.");
vSeeds.emplace_back("signet-seed.ocvcoin.lplfinancial.");
vSeeds.emplace_back("signet-seed.ocvcoin.homegoods.");
vSeeds.emplace_back("signet-seed.ocvcoin.merckmsd.");
vSeeds.emplace_back("signet-seed.ocvcoin.olayangroup.");
vSeeds.emplace_back("signet-seed.ocvcoin.inf.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.cc.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--3ds443g.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--55qx5d.");
vSeeds.emplace_back("signet-seed.ocvcoin.ltd.ua.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--6qq986b3xl.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--fiqs8s.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--6frz82g.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--mk1bu44c.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--tckwe.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--vhquv.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--fiq228c5hs.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--q9jyb4c.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--t60b56a.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--5tzm5g.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--9dbq2a.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--czrs0t.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--rhqv96g.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--unup4y.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--fjq720a.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--vuq861b.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--ses554g.");
vSeeds.emplace_back("signet-seed.ocvcoin.sch.id.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--io0a7i.");
vSeeds.emplace_back("signet-seed.ocvcoin.net.id.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--czr694b.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--czru2d.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--hxt814e.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--3bst00m.");
vSeeds.emplace_back("signet-seed.ocvcoin.co.pl.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--qxam.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--xhq521b.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--1qqw23a.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--otu796d.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--45q11c.");
vSeeds.emplace_back("signet-seed.ocvcoin.xn--imr513n.");


            

            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 1;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                
                .nTime    = 0,
                .nTxCount = 0,
                .dTxRate  = 0,
            };
        } else {
            bin = *options.challenge;
            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                0,
                0,
                0,
            };
            LogPrintf("Signet with challenge %s\n", HexStr(bin));
        }

        if (options.seeds) {
            vSeeds = *options.seeds;
        }

        m_chain_type = ChainType::SIGNET;
        consensus.signet_blocks = true;
        consensus.signet_challenge.assign(bin.begin(), bin.end());
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.nPowTargetTimespan = 24 * 60 * 60; // 1 day
        consensus.nPowTargetSpacing = 5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1815; // 90% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Activation of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        // message start is defined as the first 4 bytes of the sha256d of the block script
        HashWriter h{};
        h << consensus.signet_challenge;
        uint256 hash = h.GetHash();
        std::copy_n(hash.begin(), 4, pchMessageStart.begin());

        nDefaultPort = 38333;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1630950526, 1, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x73cad474ba4d2bdff7f09d2e89574eb898b0f556f484639d5927fd83de6a37d1"));
        assert(genesis.hashMerkleRoot == uint256S("0xb2097d2bfc31e8937aaafe9fa7d634531274463f50e13469ba056cbddc594f1d"));

        vFixedSeeds.clear();

        m_assumeutxo_data = {
            
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tb";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
    }
};

/**
 * Regression test: intended for private networks only. Has minimal difficulty to ensure that
 * blocks can be found instantly.
 */
class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        m_chain_type = ChainType::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP34Height = 1; // Always active unless overridden
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1;  // Always active unless overridden
        consensus.BIP66Height = 1;  // Always active unless overridden
        consensus.CSVHeight = 1;    // Always active unless overridden
        consensus.SegwitHeight = 0; // Always active unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 24 * 60 * 60; // 1 day
        consensus.nPowTargetSpacing = 5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xd9;
        nDefaultPort = 18444;
        nPruneAfterHeight = opts.fastprune ? 100 : 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        for (const auto& [dep, height] : opts.activation_heights) {
            switch (dep) {
            case Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT:
                consensus.SegwitHeight = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB:
                consensus.BIP34Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_DERSIG:
                consensus.BIP66Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CLTV:
                consensus.BIP65Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CSV:
                consensus.CSVHeight = int{height};
                break;
            }
        }

        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height = version_bits_params.min_activation_height;
        }

        genesis = CreateGenesisBlock(1630950492, 1, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x12854eb9cb598ee986b980d2b298d94f651c11d7994ab6fcd142fc219bd4a77e"));
        assert(genesis.hashMerkleRoot == uint256S("0xb2097d2bfc31e8937aaafe9fa7d634531274463f50e13469ba056cbddc594f1d"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();
        vSeeds.emplace_back("dummySeed.invalid.");
		

        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
                {0, uint256S("12854eb9cb598ee986b980d2b298d94f651c11d7994ab6fcd142fc219bd4a77e")},
            }
        };

        m_assumeutxo_data = {
            
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "bcrt";
    }
};

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions& options)
{
    return std::make_unique<const SigNetParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}
