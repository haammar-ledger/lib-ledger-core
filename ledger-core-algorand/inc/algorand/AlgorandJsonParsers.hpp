/*
 * AlgorandJsonParsers
 *
 * Created by Hakim Aammar on 11/05/2020.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Ledger
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

// TODO Tests

#ifndef LEDGER_CORE_ALGORANDJSONPARSERS_H
#define LEDGER_CORE_ALGORANDJSONPARSERS_H

#include <algorand/AlgorandAddress.hpp>
#include <algorand/AlgorandModel.hpp>
#include <algorand/api/AlgorandTransactionType.hpp>

#include <core/math/BigInt.hpp>
#include <core/utils/Hex.hpp>

#define GET_MANDATORY_STRING_FIELD(FieldType, Field) assert(node.HasMember(FieldType)); Field = node[FieldType].GetString();
#define GET_MANDATORY_UINT64_FIELD(FieldType, Field) assert(node.HasMember(FieldType)); Field = node[FieldType].GetUint64();
#define GET_MANDATORY_BOOL_FIELD(FieldType, Field) assert(node.HasMember(FieldType)); Field = node[FieldType].GetBool();
#define GET_OPTIONAL_STRING_FIELD(FieldType, Field) if (node.HasMember(FieldType)) Field = node[FieldType].GetString();
#define GET_OPTIONAL_UINT64_FIELD(FieldType, Field) if (node.HasMember(FieldType)) Field = node[FieldType].GetUint64();
#define GET_OPTIONAL_BOOL_FIELD(FieldType, Field) if (node.HasMember(FieldType)) Field = node[FieldType].GetBool();

namespace ledger {
namespace core {
namespace algorand {
namespace json {

namespace constants {

    // Explorer endpoints
    static constexpr char purestakeBlockEndpoint[] = "ps1/v1/block/{}";
    static constexpr char purestakeAccountEndpoint[] = "ps1/v1/account/{}";
    static constexpr char purestakeAccountTransactionsEndpoint[] = "ps1/v1/account/{}/transactions";
    static constexpr char purestakeTransactionEndpoint[] = "ps1/v1/transaction/{}";
    static constexpr char purestakeTransactionsEndpoint[] = "ps1/v1/transactions";
    static constexpr char purestakeTransactionsParamsEndpoint[] = "ps1/v1/transactions/params";

    // Json objects keys
    static constexpr char hash[] = "hash";
    static constexpr char timestamp[] = "timestamp";
    static constexpr char round[] = "round";

    static constexpr char creator[] = "creator";
    static constexpr char amount[] = "amount";
    static constexpr char frozen[] = "frozen";
    static constexpr char total[] = "total";
    static constexpr char decimals[] = "decimals";
    static constexpr char unitName[] = "unitname";
    static constexpr char assetName[] = "assetname";
    static constexpr char managerKey[] = "managerkey";
    static constexpr char freezeAddr[] = "freezeaddr";
    static constexpr char clawbackAddr[] = "clawbackaddr";
    static constexpr char reserveAddr[] = "reserveaddr";
    static constexpr char metadataHash[] = "metadatahash";
    static constexpr char defaultFrozen[] = "defaultfrozen";
    static constexpr char url[] = "url";

    static constexpr char address[] = "address";
    static constexpr char pendingRewards[] = "pendingrewards";
    static constexpr char amountWithoutPendingRewards[] = "amountwithoutpendingrewards";
    static constexpr char rewards[] = "rewards";
    static constexpr char status[] = "status";
    static constexpr char assets[] = "assets";
    static constexpr char thisAssetTotal[] = "thisassettotal";
    static constexpr char participation[] = "participation";

    static constexpr char to[] = "to";
    static constexpr char toRewards[] = "torewards";
    static constexpr char close[] = "close";
    static constexpr char closeAmount[] = "closeamount";
    static constexpr char closeRewards[] = "closerewards";

    static constexpr char selkey[] = "selkey";
    static constexpr char votefst[] = "votefst";
    static constexpr char votekd[] = "votekd";
    static constexpr char votekey[] = "votekey";
    static constexpr char votelst[] = "votelst";

    static constexpr char id[] = "id";
    static constexpr char params[] = "params";

    static constexpr char amt[] = "amt";
    static constexpr char rcv[] = "rcv";
    static constexpr char snd[] = "snd";
    static constexpr char closeTo[] = "closeto";

    static constexpr char acct[] = "acct";
    static constexpr char freeze[] = "freeze";

    static constexpr char type[] = "type";
    static constexpr char tx[] = "tx";
    static constexpr char from[] = "from";
    static constexpr char fee[] = "fee";
    static constexpr char firstRound[] = "first-round";
    static constexpr char lastRound[] = "last-round";
    static constexpr char noteB64[] = "noteb64";
    static constexpr char fromRewards[] = "fromrewards";
    static constexpr char genesisId[] = "genesisID";
    static constexpr char genesisHashB64[] = "genesishashb64";
    static constexpr char group[] = "group";
    static constexpr char lease[] = "lease";

    static constexpr char payment[] = "payment";
    static constexpr char keyreg[] = "keyreg";
    static constexpr char curcfg[] = "curcfg";
    static constexpr char curxfer[] = "curxfer";
    static constexpr char curfrz[] = "curfrz";

    static constexpr char transactions[] = "transactions";
    static constexpr char txId[] = "txId";
    static constexpr char minFee[] = "fee";
    static constexpr char consensusVersion[] = "consensusVersion";

} // namespace constants

    using TxType = ledger::core::api::AlgorandTransactionType;

    constexpr const char* txTypeToChars(const TxType type) {
        switch (type) {
            case TxType::PAYMENT: return constants::payment;
            case TxType::KEY_REGISTRATION: return constants::keyreg;
            case TxType::ASSET_CONFIGURATION: return constants::curcfg;
            case TxType::ASSET_TRANSFER: return constants::curxfer;
            case TxType::ASSET_FREEZE: return constants::curfrz;
            case TxType::UNSUPPORTED:
            default:
                return "";
        }
    }

    const TxType charsToTxType(const std::string string) {
        if (string == constants::payment) return TxType::PAYMENT;
        if (string == constants::keyreg) return TxType::KEY_REGISTRATION;
        if (string == constants::curcfg) return TxType::ASSET_CONFIGURATION;
        if (string == constants::curxfer) return TxType::ASSET_TRANSFER;
        if (string == constants::curfrz) return TxType::ASSET_FREEZE;
        return TxType::UNSUPPORTED;
    }

    template <class T>
    void parseBlock(const T& node, const std::string& currencyName, api::Block & out) {
        out.currencyName = currencyName;
        GET_MANDATORY_STRING_FIELD(constants::hash, out.blockHash);

        // FIXME Test this!
        out.time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(node[constants::timestamp].GetUint64()));
        //out.time = DateUtils::fromJSON(node[constants::timestampField].GetUint64());
        //GET_MANDATORY_UINT64_FIELD(constants::timestampField, out.time);

        // FIXME Test this!
        uint64_t blockHeight = 0;
        GET_MANDATORY_UINT64_FIELD(constants::round, blockHeight);
        if (blockHeight > std::numeric_limits<int64_t>::max()) {
            throw make_exception(api::ErrorCode::OUT_OF_RANGE, "Block height exceeds maximum value");
        }
        out.height = (int64_t) blockHeight;
        //out.height = BigInt::fromScalar<uint64_t>(node[constants::round].GetUint64()).toInt64();
    }

    template <class T>
    void parseAssetAmount(const T& node, model::AssetAmount & out) {
        GET_MANDATORY_STRING_FIELD(constants::creator, out.creatorAddress);
        GET_MANDATORY_UINT64_FIELD(constants::amount, out.amount);
        GET_MANDATORY_BOOL_FIELD(constants::frozen, out.frozen);
    }

    template <class T>
    void parseAssetsAmounts(const T& node, std::map<uint64_t, model::AssetAmount> & out) {
        for (rapidjson::Value::ConstMemberIterator child = node.MemberBegin(); child != node.MemberEnd(); ++child) {
            model::AssetAmount assetAmount;
            const auto coinId = child->name.GetUint64();
            parseAssetAmount(child->value, assetAmount);
            out[coinId] = assetAmount;
        }
    }

    template <typename T>
    void parseAssetParams(const T& node, const uint64_t & assetId, model::AssetParams & out) {

        *out.assetId = assetId;

        GET_MANDATORY_STRING_FIELD(constants::creator, out.creatorAddress);
        GET_MANDATORY_UINT64_FIELD(constants::total, out.total);
        GET_MANDATORY_UINT64_FIELD(constants::decimals, out.decimals);
        GET_MANDATORY_BOOL_FIELD(constants::defaultFrozen, *out.defaultFrozen);
        GET_MANDATORY_STRING_FIELD(constants::unitName, *out.unitName);
        GET_MANDATORY_STRING_FIELD(constants::assetName, *out.assetName);

        GET_OPTIONAL_STRING_FIELD(constants::managerKey, *out.managerAddress);
        GET_OPTIONAL_STRING_FIELD(constants::freezeAddr, *out.freezeAddress);
        GET_OPTIONAL_STRING_FIELD(constants::clawbackAddr, *out.clawbackAddress);
        GET_OPTIONAL_STRING_FIELD(constants::reserveAddr, *out.reserveAddress);
        GET_OPTIONAL_STRING_FIELD(constants::metadataHash, *out.metadataHash);
        GET_OPTIONAL_STRING_FIELD(constants::url, *out.url);
    }

    template <class T>
    void parseAssetsParams(const T& node, std::map<uint64_t, model::AssetParams> & out) {
        for (rapidjson::Value::ConstMemberIterator child = node.MemberBegin(); child != node.MemberEnd(); ++child) {
            model::AssetParams assetParams;
            const auto assetId = child->name.GetUint64();
            parseAssetParams(child->value, assetId, assetParams);
            out[assetId] = assetParams;
        }
    }

    template <class T>
    void parsePaymentInfo(const T& node, model::PaymentInfo & out) {
        GET_MANDATORY_STRING_FIELD(constants::to, out.recipientAddress);
        GET_MANDATORY_UINT64_FIELD(constants::amount, out.amount);

        GET_OPTIONAL_STRING_FIELD(constants::close, *out.closeAddress);
        GET_OPTIONAL_UINT64_FIELD(constants::closeAmount, *out.closeAmount);
        GET_OPTIONAL_UINT64_FIELD(constants::closeRewards, *out.closeRewards);
        GET_OPTIONAL_UINT64_FIELD(constants::toRewards, *out.recipientRewards);
    }

    // WARNING This has not been tested
    template <class T>
    void parseParticipationInfo(const T& node, model::ParticipationInfo & out) {
        GET_MANDATORY_STRING_FIELD(constants::votekey , out.rootPublicKey);
        GET_MANDATORY_STRING_FIELD(constants::selkey, out.vrfPublicKey);
        GET_MANDATORY_STRING_FIELD(constants::votekd, out.voteKeyDilution);
        GET_MANDATORY_STRING_FIELD(constants::votefst, out.voteFirstRound);
        GET_MANDATORY_STRING_FIELD(constants::votelst, out.voteLastRound);
    }

    template <class T>
    void parseAssetConfigurationInfo(const T& node, model::AssetConfigurationInfo & out) {
        GET_MANDATORY_UINT64_FIELD(constants::id, *out.assetId);

        assert(node.HasMember(constants::params));
        // FIXME Temporary
        uint64_t assetId;
        std::istringstream iss(*out.assetId);
        iss >> assetId;
        parseAssetParams(node[constants::params].GetObject(), assetId, *out.assetParams);
    }

    template <class T>
    void parseAssetTransferInfo(const T& node, model::AssetTransferInfo & out) {
        GET_MANDATORY_STRING_FIELD(constants::id, out.assetId);
        GET_MANDATORY_STRING_FIELD(constants::rcv, out.recipientAddress);
        GET_MANDATORY_UINT64_FIELD(constants::amt, out.amount);

        GET_OPTIONAL_STRING_FIELD(constants::closeTo, *out.closeAddress);
        GET_OPTIONAL_STRING_FIELD(constants::snd, *out.clawedBackAddress);
    }

    // WARNING This has not been tested
    template <class T>
    void parseAssetFreezeInfo(const T& node, model::AssetFreezeInfo & out) {
        GET_MANDATORY_BOOL_FIELD(constants::id, out.assetId);
        GET_MANDATORY_STRING_FIELD(constants::acct, out.frozenAddress);
        GET_MANDATORY_UINT64_FIELD(constants::freeze, out.frozen);
    }

    template <class T>
    void parseTransaction(const T& node, model::Transaction & out) {

        assert(node.HasMember(constants::type));
        out.type = charsToTxType(node[constants::type].GetString());

        GET_MANDATORY_STRING_FIELD(constants::tx, out.id);
        GET_MANDATORY_STRING_FIELD(constants::from, out.senderAddress);
        GET_MANDATORY_UINT64_FIELD(constants::firstRound, out.firstRound);
        GET_MANDATORY_UINT64_FIELD(constants::lastRound, out.lastRound);
        GET_MANDATORY_UINT64_FIELD(constants::round, out.round);
        GET_MANDATORY_UINT64_FIELD(constants::fee, out.fee);
        GET_MANDATORY_STRING_FIELD(constants::genesisHashB64, out.genesisHash);

        GET_OPTIONAL_STRING_FIELD(constants::genesisId, out.genesisId);
        GET_OPTIONAL_STRING_FIELD(constants::noteB64, out.note);
        GET_OPTIONAL_UINT64_FIELD(constants::fromRewards, out.fromRewards);
        GET_OPTIONAL_STRING_FIELD(constants::group, out.group);
        GET_OPTIONAL_STRING_FIELD(constants::lease, out.lease);

        switch (out.type) {
            case TxType::PAYMENT:
                assert((node.HasMember(constants::payment)));
                out.details = model::PaymentInfo();
                parsePaymentInfo(node[constants::payment].GetObject(),
                                 boost::get<model::PaymentInfo>(out.details));
                break;

            case TxType::KEY_REGISTRATION:
                assert((node.HasMember(constants::keyreg)));
                out.details = model::ParticipationInfo();
                parseParticipationInfo(node[constants::keyreg].GetObject(),
                                       boost::get<model::ParticipationInfo>(out.details));
                break;

            case TxType::ASSET_CONFIGURATION:
                assert((node.HasMember(constants::curcfg)));
                out.details = model::AssetConfigurationInfo();
                parseAssetConfigurationInfo(node[constants::curcfg].GetObject(),
                                            boost::get<model::AssetConfigurationInfo>(out.details));
                break;

            case TxType::ASSET_TRANSFER:
                assert((node.HasMember(constants::curxfer)));
                out.details = model::AssetTransferInfo();
                parseAssetTransferInfo(node[constants::curxfer].GetObject(),
                                       boost::get<model::AssetTransferInfo>(out.details));
                break;

            case TxType::ASSET_FREEZE:
                assert((node.HasMember(constants::curfrz)));
                out.details = model::AssetFreezeInfo();
                parseAssetFreezeInfo(node[constants::curfrz].GetObject(),
                                     boost::get<model::AssetFreezeInfo>(out.details));
                break;

            case TxType::UNSUPPORTED:
            default:
                break;
        }
    }

    template <class T>
    void parseTransactions(const T& array, std::vector<model::Transaction> & out) {
        out.assign((std::size_t) array.Size(), model::Transaction());
        auto index = 0;
        for (const auto& node : array) {
            parseTransaction(node.GetObject(), out[index]);
            index++;
        }
    }

    template <class T>
    void parseTransactionParams(const T& node, model::TransactionParams & out) {
        GET_MANDATORY_STRING_FIELD(constants::genesisId, out.genesisID);
        GET_MANDATORY_UINT64_FIELD(constants::genesisHashB64, out.genesisHash);
        GET_MANDATORY_UINT64_FIELD(constants::lastRound, out.lastRound);
        GET_MANDATORY_UINT64_FIELD(constants::fee, out.suggestedFeePerByte);
        GET_MANDATORY_UINT64_FIELD(constants::minFee, out.minFee);
        GET_MANDATORY_UINT64_FIELD(constants::consensusVersion, out.consensusVersion);
    }

    template <class T>
    void parseAccount(const T& node, model::Account & out) {
        GET_MANDATORY_UINT64_FIELD(constants::round, out.round);
        GET_MANDATORY_STRING_FIELD(constants::address, out.address);
        out.pubKeyHex = hex::toString(Address::toPublicKey(out.address));
        GET_MANDATORY_UINT64_FIELD(constants::amount, out.amount);
        GET_MANDATORY_UINT64_FIELD(constants::pendingRewards, out.pendingRewards);
        GET_MANDATORY_UINT64_FIELD(constants::amountWithoutPendingRewards, out.amountWithoutPendingRewards);
        GET_MANDATORY_UINT64_FIELD(constants::rewards, out.rewards);

        GET_OPTIONAL_STRING_FIELD(constants::status, out.status);

        if (node.HasMember(constants::assets)) {
            parseAssetsAmounts(node[constants::assets].GetObject(), out.assetsAmounts);
        }

        if (node.HasMember(constants::thisAssetTotal)) {
            parseAssetsParams(node[constants::thisAssetTotal].GetObject(), out.createdAssets);
        }

        if (node.HasMember(constants::participation)) {
            parseParticipationInfo(node[constants::participation].GetObject(), *out.participation);
        }
    }

} // namespace json
} // namespace algorand
} // namespace core
} // namespace ledger

#undef GET_MANDATORY_STRING_FIELD
#undef GET_MANDATORY_UINT64_FIELD
#undef GET_MANDATORY_BOOL_FIELD
#undef GET_OPTIONAL_STRING_FIELD
#undef GET_OPTIONAL_UINT64_FIELD
#undef GET_OPTIONAL_BOOL_FIELD

#endif // LEDGER_CORE_ALGORANDJSONPARSERS_H
