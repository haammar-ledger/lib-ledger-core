/*
 * AlgorandJsonParser
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

#ifndef LEDGER_CORE_ALGORANDJSONPARSER_H
#define LEDGER_CORE_ALGORANDJSONPARSER_H

#include <algorand/AlgorandAddress.hpp>
#include <algorand/AlgorandModel.hpp> // FIXME Deprecated
#include <algorand/model/transactions/AlgorandExplorerTransaction.hpp>

#include <core/math/BigInt.hpp>
#include <core/utils/Hex.hpp>

namespace ledger {
namespace core {
namespace algorand {
namespace constants {

    // Json objects keys
    static const std::string hash = "hash";
    static const std::string timestamp = "timestamp";
    static const std::string round = "round";

    static const std::string creator = "creator";
    static const std::string amount = "amount";
    static const std::string frozen = "frozen";
    static const std::string total = "total";
    static const std::string decimals = "decimals";
    static const std::string unitName = "unitname";
    static const std::string assetName = "assetname";
    static const std::string managerKey = "managerkey";
    static const std::string freezeAddr = "freezeaddr";
    static const std::string clawbackAddr = "clawbackaddr";
    static const std::string reserveAddr = "reserveaddr";
    static const std::string metadataHash = "metadatahash";
    static const std::string defaultFrozen = "defaultfrozen";
    static const std::string url = "url";

    static const std::string address = "address";
    static const std::string pendingRewards = "pendingrewards";
    static const std::string amountWithoutPendingRewards = "amountwithoutpendingrewards";
    static const std::string rewards = "rewards";
    static const std::string status = "status";
    static const std::string assets = "assets";
    static const std::string thisAssetTotal = "thisassettotal";
    static const std::string participation = "participation";

    static const std::string to = "to";
    static const std::string toRewards = "torewards";
    static const std::string close = "close";
    static const std::string closeAmount = "closeamount";
    static const std::string closeRewards = "closerewards";

    static const std::string selkey = "selkey";
    static const std::string votefst = "votefst";
    static const std::string votekd = "votekd";
    static const std::string votekey = "votekey";
    static const std::string votelst = "votelst";

    static const std::string id = "id";
    static const std::string params = "params";

    static const std::string amt = "amt";
    static const std::string rcv = "rcv";
    static const std::string snd = "snd";
    static const std::string closeTo = "closeto";

    static const std::string acct = "acct";
    static const std::string freeze = "freeze";

    static const std::string type = "type";
    static const std::string tx = "tx";
    static const std::string from = "from";
    static const std::string fee = "fee";
    static const std::string firstRound = "first-round";
    static const std::string lastRound = "last-round";
    static const std::string noteB64 = "noteb64";
    static const std::string fromRewards = "fromrewards";
    static const std::string genesisId = "genesisID";
    static const std::string genesisHashB64 = "genesishashb64";
    static const std::string group = "group";
    static const std::string lease = "lease";

    static const std::string payment = "payment";
    static const std::string keyreg = "keyreg";
    static const std::string curcfg = "curcfg";
    static const std::string curxfer = "curxfer";
    static const std::string curfrz = "curfrz";

    static const std::string transactions = "transactions";
    static const std::string txId = "txId";
    static const std::string minFee = "fee";
    static const std::string consensusVersion = "consensusVersion";

} // namespace constants

    class JsonParser {

    public:

        template <class T>
        static void parseBlock(const T& node, const std::string& currencyName, api::Block & out) {
            out.currencyName = currencyName;
            getMandatoryStringField(node, constants::hash, out.blockHash);

            // FIXME Test this!
            out.time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(node[constants::timestamp.c_str()].GetUint64()));
            //out.time = DateUtils::fromJSON(node[constants::timestampField].GetUint64());
            //getMandatoryUint64Field(node, constants::timestampField, out.time);

            // FIXME Test this!
            uint64_t blockHeight = 0;
            getMandatoryUint64Field(node, constants::round, blockHeight);
            if (blockHeight > std::numeric_limits<int64_t>::max()) {
                throw make_exception(api::ErrorCode::OUT_OF_RANGE, "Block height exceeds maximum value");
            }
            out.height = (int64_t) blockHeight;
            //out.height = BigInt::fromScalar<uint64_t>(node[constants::round].GetUint64()).toInt64();
        }

        template <class T>
        static void parseAssetAmount(const T& node, model::AssetAmount & out) {
            getMandatoryStringField(node, constants::creator, out.creatorAddress);
            getMandatoryUint64Field(node, constants::amount, out.amount);
            getMandatoryBoolField(node, constants::frozen, out.frozen);
        }

        template <class T>
        static void parseAssetsAmounts(const T& node, std::map<uint64_t, model::AssetAmount> & out) {
            for (rapidjson::Value::ConstMemberIterator child = node.MemberBegin(); child != node.MemberEnd(); ++child) {
                model::AssetAmount assetAmount;
                const auto coinId = child->name.GetUint64();
                parseAssetAmount(child->value, assetAmount);
                out[coinId] = assetAmount;
            }
        }

        template <typename T>
        static void parseAssetParams(const T& node, const uint64_t & assetId, model::AssetParams & out) {

            *out.assetId = assetId;

            getMandatoryStringField(node, constants::creator, out.creatorAddress);
            getMandatoryUint64Field(node, constants::total, out.total);
            getMandatoryUint64Field(node, constants::decimals, out.decimals);
            getMandatoryBoolField(node, constants::defaultFrozen, *out.defaultFrozen);
            getMandatoryStringField(node, constants::unitName, *out.unitName);
            getMandatoryStringField(node, constants::assetName, *out.assetName);

            getOptionalStringField(node, constants::managerKey, *out.managerAddress);
            getOptionalStringField(node, constants::freezeAddr, *out.freezeAddress);
            getOptionalStringField(node, constants::clawbackAddr, *out.clawbackAddress);
            getOptionalStringField(node, constants::reserveAddr, *out.reserveAddress);
            getOptionalStringField(node, constants::metadataHash, *out.metadataHash);
            getOptionalStringField(node, constants::url, *out.url);
        }

        template <class T>
        static void parseAssetsParams(const T& node, std::map<uint64_t, model::AssetParams> & out) {
            for (rapidjson::Value::ConstMemberIterator child = node.MemberBegin(); child != node.MemberEnd(); ++child) {
                model::AssetParams assetParams;
                const auto assetId = child->name.GetUint64();
                parseAssetParams(child->value, assetId, assetParams);
                out[assetId] = assetParams;
            }
        }

        template <class T>
        static void parsePaymentInfo(const T& node, model::PaymentInfo & out) {
            getMandatoryStringField(node, constants::to, out.recipientAddress);
            getMandatoryUint64Field(node, constants::amount, out.amount);

            getOptionalStringField(node, constants::close, *out.closeAddress);
            getOptionalUint64Field(node, constants::closeAmount, *out.closeAmount);
            getOptionalUint64Field(node, constants::closeRewards, *out.closeRewards);
            getOptionalUint64Field(node, constants::toRewards, *out.recipientRewards);
        }

        // WARNING This has not been tested
        template <class T>
        static void parseParticipationInfo(const T& node, model::ParticipationInfo & out) {
            getMandatoryStringField(node, constants::votekey , out.rootPublicKey);
            getMandatoryStringField(node, constants::selkey, out.vrfPublicKey);
            getMandatoryStringField(node, constants::votekd, out.voteKeyDilution);
            getMandatoryStringField(node, constants::votefst, out.voteFirstRound);
            getMandatoryStringField(node, constants::votelst, out.voteLastRound);
        }

        template <class T>
        static void parseAssetConfigurationInfo(const T& node, model::AssetConfigurationInfo & out) {
            getMandatoryUint64Field(node, constants::id, *out.assetId);

            assert(node.HasMember(constants::params.c_str()));
            parseAssetParams(node[constants::params.c_str()].GetObject(), *out.assetId, *out.assetParams);
        }

        template <class T>
        static void parseAssetTransferInfo(const T& node, model::AssetTransferInfo & out) {
            getMandatoryUint64Field(node, constants::id, out.assetId);
            getMandatoryStringField(node, constants::rcv, out.recipientAddress);
            getMandatoryUint64Field(node, constants::amt, out.amount);

            getOptionalStringField(node, constants::closeTo, *out.closeAddress);
            getOptionalStringField(node, constants::snd, *out.clawedBackAddress);
        }

        // WARNING This has not been tested
        template <class T>
        static void parseAssetFreezeInfo(const T& node, model::AssetFreezeInfo & out) {
            getMandatoryUint64Field(node, constants::id, out.assetId);
            getMandatoryStringField(node, constants::acct, out.frozenAddress);
            getMandatoryBoolField(node, constants::freeze, out.frozen);
        }

        template <class T>
        static void parseTransaction(const T& node, model::ExplorerTransaction & out) {

            getMandatoryStringField(node, constants::type, out.baseTransaction.type);
            getMandatoryStringField(node, constants::tx, out.id);
            getMandatoryStringField(node, constants::from, out.baseTransaction.senderAddress);
            getMandatoryUint64Field(node, constants::firstRound, out.baseTransaction.firstRound);
            getMandatoryUint64Field(node, constants::lastRound, out.baseTransaction.lastRound);
            getMandatoryUint64Field(node, constants::round, out.round);
            getMandatoryUint64Field(node, constants::fee, out.baseTransaction.fee);
            getMandatoryStringField(node, constants::genesisHashB64, out.baseTransaction.genesisHash);

            getOptionalStringField(node, constants::genesisId, *out.baseTransaction.genesisId);
            getOptionalStringField(node, constants::noteB64, *out.baseTransaction.note);
            getOptionalUint64Field(node, constants::fromRewards, out.fromRewards);
            getOptionalStringField(node, constants::group, *out.baseTransaction.group);
            getOptionalStringField(node, constants::lease, *out.baseTransaction.lease);

            if (out.baseTransaction.type == constants::pay) {
                    assert((node.HasMember(constants::payment.c_str())));
                    out.baseTransaction.details = model::PaymentInfo();
                    parsePaymentInfo(node[constants::payment.c_str()].GetObject(),
                                    boost::get<model::PaymentInfo>(out.baseTransaction.details));
            } else if (out.baseTransaction.type == constants::keyreg) {
                    assert((node.HasMember(constants::keyreg.c_str())));
                    out.baseTransaction.details = model::ParticipationInfo();
                    parseParticipationInfo(node[constants::keyreg.c_str()].GetObject(),
                                        boost::get<model::ParticipationInfo>(out.baseTransaction.details));
            } else if (out.baseTransaction.type == constants::acfg) {
                    assert((node.HasMember(constants::curcfg.c_str())));
                    out.baseTransaction.details = model::AssetConfigurationInfo();
                    parseAssetConfigurationInfo(node[constants::curcfg.c_str()].GetObject(),
                                                boost::get<model::AssetConfigurationInfo>(out.baseTransaction.details));
            } else if (out.baseTransaction.type == constants::axfer) {
                    assert((node.HasMember(constants::curxfer.c_str())));
                    out.baseTransaction.details = model::AssetTransferInfo();
                    parseAssetTransferInfo(node[constants::curxfer.c_str()].GetObject(),
                                        boost::get<model::AssetTransferInfo>(out.baseTransaction.details));
            } else if (out.baseTransaction.type == constants::afrz) {
                    assert((node.HasMember(constants::curfrz.c_str())));
                    out.baseTransaction.details = model::AssetFreezeInfo();
                    parseAssetFreezeInfo(node[constants::curfrz.c_str()].GetObject(),
                                        boost::get<model::AssetFreezeInfo>(out.baseTransaction.details));
            }
        }

        template <class T>
        static void parseTransactions(const T& array, std::vector<model::ExplorerTransaction> & out) {
            out.assign((std::size_t) array.Size(), model::ExplorerTransaction());
            auto index = 0;
            for (const auto& node : array) {
                parseTransaction(node.GetObject(), out[index]);
                index++;
            }
        }

        template <class T>
        static void parseTransactionParams(const T& node, model::TransactionParams & out) {
            getMandatoryStringField(node, constants::genesisId, out.genesisID);
            getMandatoryStringField(node, constants::genesisHashB64, out.genesisHash);
            getMandatoryUint64Field(node, constants::lastRound, out.lastRound);
            getMandatoryUint64Field(node, constants::fee, out.suggestedFeePerByte);
            getMandatoryUint64Field(node, constants::minFee, out.minFee);
            getMandatoryUint64Field(node, constants::consensusVersion, out.consensusVersion);
        }

        template <class T>
        static void parseAccount(const T& node, model::Account & out) {
            getMandatoryUint64Field(node, constants::round, out.round);
            getMandatoryStringField(node, constants::address, out.address);
            out.pubKeyHex = hex::toString(Address::toPublicKey(out.address));
            getMandatoryUint64Field(node, constants::amount, out.amount);
            getMandatoryUint64Field(node, constants::pendingRewards, out.pendingRewards);
            getMandatoryUint64Field(node, constants::amountWithoutPendingRewards, out.amountWithoutPendingRewards);
            getMandatoryUint64Field(node, constants::rewards, out.rewards);

            getOptionalStringField(node, constants::status, out.status);

            if (node.HasMember(constants::assets.c_str())) {
                parseAssetsAmounts(node[constants::assets.c_str()].GetObject(), out.assetsAmounts);
            }

            if (node.HasMember(constants::thisAssetTotal.c_str())) {
                parseAssetsParams(node[constants::thisAssetTotal.c_str()].GetObject(), out.createdAssets);
            }

            if (node.HasMember(constants::participation.c_str())) {
                parseParticipationInfo(node[constants::participation.c_str()].GetObject(), *out.participation);
            }
        }

    private:

        template <class T>
        static void getMandatoryStringField(const T & node, const std::string & fieldName, std::string & field) {
            assert(node.HasMember(fieldName.c_str()));
            field = node[fieldName.c_str()].GetString();
        }

        template <class T>
        static void getMandatoryUint64Field(const T & node, const std::string & fieldName, uint64_t & field) {
            assert(node.HasMember(fieldName));
            field = node[fieldName.c_str()].GetUint64();
        }

        template <class T>
        static void getMandatoryBoolField(const T & node, const std::string & fieldName, bool & field) {
            assert(node.HasMember(fieldName.c_str()));
            field = node[fieldName.c_str()].GetBool();
        }

        template <class T>
        static void getOptionalStringField(const T & node, const std::string & fieldName, std::string & field) {
            if (node.HasMember(fieldName.c_str())) {
                field = node[fieldName.c_str()].GetString();
            }
        }

        template <class T>
        static void getOptionalUint64Field(const T & node, const std::string & fieldName, uint64_t & field) {
            if (node.HasMember(fieldName.c_str())) {
                field = node[fieldName.c_str()].GetUint64();
            }
        }

        template <class T>
        static void getOptionalBoolField(const T & node, const std::string & fieldName, bool & field) {
            if (node.HasMember(fieldName.c_str())) {
                field = node[fieldName.c_str()].GetBool();
            }
        }
    };

} // namespace algorand
} // namespace core
} // namespace ledger

#endif // LEDGER_CORE_ALGORANDJSONPARSER_H
