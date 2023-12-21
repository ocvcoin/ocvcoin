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
        
        vSeeds.emplace_back("dnsseed.ocvcoin.dashjr.org."); // Luke Dashjr
        
        vSeeds.emplace_back("seed.ocvcoin.jonasschnelli.ch."); // Jonas Schnelli, only supports x1, x5, x9, and xd
        vSeeds.emplace_back("seed.ocv.petertodd.org."); // Peter Todd, only supports x1, x5, x9, and xd
        vSeeds.emplace_back("seed.ocvcoin.sprovoost.nl."); // Sjors Provoost
        
        vSeeds.emplace_back("seed.ocvcoin.wiz.biz."); // Jason Maurice

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
        vSeeds.emplace_back("testnet-seed.ocvcoin.jonasschnelli.ch.");
        vSeeds.emplace_back("seed.tocv.petertodd.org.");
        vSeeds.emplace_back("seed.testnet.ocvcoin.sprovoost.nl.");
        

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
            vSeeds.emplace_back("seed.signet.ocvcoin.sprovoost.nl.");

            

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
