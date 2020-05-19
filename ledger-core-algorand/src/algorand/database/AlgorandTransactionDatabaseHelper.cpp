/*
 * AlgorandTransactionDatabaseHelper
 *
 * Created by Hakim Aammar on 18/05/2020.
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

#include <algorand/database/AlgorandTransactionDatabaseHelper.hpp>
#include <algorand/AlgorandExplorerConstants.hpp>
#include <algorand/model/transactions/AlgorandPayment.hpp>
#include <algorand/model/transactions/AlgorandKeyreg.hpp>
#include <algorand/model/transactions/AlgorandAsset.hpp>
#include <algorand/utils/B64String.hpp>

#include <core/crypto/SHA256.hpp>
#include <core/database/SociNumber.hpp>
#include <core/math/BaseConverter.hpp>
#include <core/wallet/BlockDatabaseHelper.hpp>

#include <fmt/format.h>

/*
#include <algorand/api/enum_from_string.hpp>

#include <core/database/SociOption.hpp>
#include <core/database/SociDate.hpp>
#include <core/database/SociNumber.hpp>
#include <core/utils/Option.hpp>
*/

//using namespace soci;

namespace ledger {
namespace core {
namespace algorand {

    constexpr auto COL_TX_HASH = 1;
    constexpr auto COL_TX_TYPE = 2;
    constexpr auto COL_TX_ROUND = 3;
    constexpr auto COL_TX_TIMESTAMP = 4;
    constexpr auto COL_TX_FIRST_VALID = 5;
    constexpr auto COL_TX_LAST_VALID = 6;
    constexpr auto COL_TX_GENESIS_ID = 7;
    constexpr auto COL_TX_GENESIS_HASH = 8;
    constexpr auto COL_TX_SENDER = 9;
    constexpr auto COL_TX_FEE = 10;
    constexpr auto COL_TX_FROM_REWARDS = 11;
    constexpr auto COL_TX_NOTE = 12;
    constexpr auto COL_TX_GROUP = 13;
    constexpr auto COL_TX_LEASE = 14;

    constexpr auto COL_TX_PAY_AMOUNT = 16;
    constexpr auto COL_TX_PAY_RECEIVER_ADDRESS = 17;
    constexpr auto COL_TX_PAY_RECEIVER_REWARDS = 18;
    constexpr auto COL_TX_PAY_CLOSE_ADDRESS = 19;
    constexpr auto COL_TX_PAY_CLOSE_AMOUNT = 20;
    constexpr auto COL_TX_PAY_CLOSE_REWARDS = 21;

    constexpr auto COL_TX_KEYREG_NONPART = 22;
    constexpr auto COL_TX_KEYREG_SELECTION_PK = 23;
    constexpr auto COL_TX_KEYREG_VOTE_PK = 24;
    constexpr auto COL_TX_KEYREG_VOTE_KEY_DILUTION = 25;
    constexpr auto COL_TX_KEYREG_VOTE_FIRST = 26;
    constexpr auto COL_TX_KEYREG_VOTE_LAST = 27;

    constexpr auto COL_TX_ACFG_ASSET_ID = 29;
    constexpr auto COL_TX_ACFG_ASSET_NAME = 30;
    constexpr auto COL_TX_ACFG_UNIT_NAME = 31;
    constexpr auto COL_TX_ACFG_TOTAL = 32;
    constexpr auto COL_TX_ACFG_DECIMALS = 33;
    constexpr auto COL_TX_ACFG_DEFAULT_FROZEN = 34;
    constexpr auto COL_TX_ACFG_CREATOR_ADDRESS = 35;
    constexpr auto COL_TX_ACFG_MANAGER_ADDRESS = 36;
    constexpr auto COL_TX_ACFG_RESERVE_ADDRESS = 37;
    constexpr auto COL_TX_ACFG_FREEZE_ADDRESS = 38;
    constexpr auto COL_TX_ACFG_CLAWBACK_ADDRESS = 39;
    constexpr auto COL_TX_ACFG_METADATA_HASH = 40;
    constexpr auto COL_TX_ACFG_URL = 41;

    constexpr auto COL_TX_AXFER_ASSET_ID = 42;
    constexpr auto COL_TX_AXFER_ASSET_AMOUNT = 43;
    constexpr auto COL_TX_AXFER_RECEIVER_ADDRESS = 44;
    constexpr auto COL_TX_AXFER_CLOSE_ADDRESS = 45;
    constexpr auto COL_TX_AXFER_SENDER_ADDRESS = 46;

    constexpr auto COL_TX_AFRZ_ASSET_ID = 47;
    constexpr auto COL_TX_AFRZ_FROZEN = 48;
    constexpr auto COL_TX_AFRZ_FROZEN_ADDRESS = 49;

    // FIXME Manage Option<>?
    // FIXME What to do with operation_type?...

    static void putPaymentTransaction(soci::session & sql, const std::string & txUid, const model::Transaction & tx) {
        auto& payment = boost::get<model::PaymentTxnFields>(tx.details);

        sql << "INSERT INTO algorand_transactions "
        // Header fields
        "(uid, hash, type, round, "
        //"timestamp, " // TODO add this
        "first_valid, last_valid, genesis_id, genesis_hash, "
        "sender, fee, from_rewards, note, group, lease, "
        // Details fields
        "pay_amount, pay_receiver_address, pay_receiver_rewards, pay_close_address, pay_close_amount, pay_close_rewards) "
        "VALUES(:tx_uid, :hash, :tx_type, :round, "
        //":timestamp, "" // TODO add this
        ":first_valid, :last_valid, :genesis_id, :genesis_hash, :sender, :fee, :from_rewards, :note, :group, :lease, "
        ":amount, :receiver_addr, :receiver_rewards, :close_addr, :close_amount, :close_rewards)",
            soci::use(txUid),
            soci::use(*tx.header.id),
            soci::use(tx.header.type),
            soci::use(*tx.header.round),
            //soci::use(*tx.header.timestamp), // TODO add this
            soci::use(tx.header.firstValid),
            soci::use(tx.header.lastValid),
            soci::use(*tx.header.genesisId),
            soci::use(tx.header.genesisHash),
            soci::use(tx.header.sender),
            soci::use(tx.header.fee),
            soci::use(*tx.header.fromRewards),
            soci::use(*tx.header.note),
            soci::use(*tx.header.group),
            soci::use(*tx.header.lease),
            soci::use(payment.amount),
            soci::use(payment.receiverAddr),
            soci::use(*payment.receiverRewards),
            soci::use(*payment.closeAddr),
            soci::use(*payment.closeAmount),
            soci::use(*payment.closeRewards);
    }

    static void putKeyRegTransaction(soci::session & sql, const std::string & txUid, const model::Transaction & tx) {
        auto& keyreg = boost::get<model::KeyRegTxnFields>(tx.details);

        sql << "INSERT INTO algorand_transactions "
        // Header fields
        "(uid, hash, type, round, "
        //"timestamp, " // TODO add this
        "first_valid, last_valid, genesis_id, genesis_hash, sender, fee, from_rewards, note, group, lease, "
        // Details fields
        "keyreg_non_participation, keyreg_selection_pk, keyreg_vote_pk, keyreg_vote_key_dilution, keyreg_vote_first, keyreg_vote_last) "
        "VALUES(:tx_uid, :hash, :tx_type, :round, "
        //":timestamp, "" // TODO add this
        ":first_valid, :last_valid, :genesis_id, :genesis_hash, :sender, :fee, :from_rewards, :note, :group, :lease, "
        ":non_part, :selection_pk, :vote_pk, :vote_key_dilution, :vote_first, :vote_last)",
            soci::use(txUid),
            soci::use(*tx.header.id),
            soci::use(tx.header.type),
            soci::use(*tx.header.round),
            //soci::use(*tx.header.timestamp), // TODO add this
            soci::use(tx.header.firstValid),
            soci::use(tx.header.lastValid),
            soci::use(*tx.header.genesisId),
            soci::use(tx.header.genesisHash),
            soci::use(tx.header.sender),
            soci::use(tx.header.fee),
            soci::use(*tx.header.fromRewards),
            soci::use(*tx.header.note),
            soci::use(*tx.header.group),
            soci::use(*tx.header.lease),
            soci::use(*keyreg.nonParticipation),
            soci::use(keyreg.selectionPk),
            soci::use(keyreg.votePk),
            soci::use(keyreg.voteKeyDilution),
            soci::use(keyreg.voteFirst),
            soci::use(keyreg.voteLast);
    }

    static void putAssetConfigTransaction(soci::session & sql, const std::string & txUid, const model::Transaction & tx) {
        auto& assetConfig = boost::get<model::AssetConfigTxnFields>(tx.details);
        auto& assetParams = *assetConfig.assetParams;

        sql << "INSERT INTO algorand_transactions "
        // Header fields
        "(uid, hash, type, round, "
        //"timestamp, " // TODO add this
        "first_valid, last_valid, genesis_id, genesis_hash, "
        "sender, fee, from_rewards, note, group, lease, "
        // Details fields
        "acfg_asset_id, acfg_asset_name, acfg_unit_name, acfg_total, acfg_decimals, acfg_default_frozen, "
        "acfg_creator_address, acfg_manager_address, acfg_reserve_address, acfg_freeze_address, acfg_clawback_address, "
        "acfg_metadata_hash, acfg_url) "
        "VALUES(:tx_uid, :hash, :tx_type, :round, "
        //":timestamp, "" // TODO add this
        ":first_valid, :last_valid, :genesis_id, :genesis_hash, :sender, :fee, :from_rewards, :note, :group, :lease, "
        ":asset_id, :asset_name, :unit_name, :total, :decimals, :default_frozen, "
        ":creator_addr, :manager_addr, :reserve_addr, :freeze_addr, :clawback_addr, "
        ":metadata_hash, :url)",
            soci::use(txUid),
            soci::use(*tx.header.id),
            soci::use(tx.header.type),
            soci::use(*tx.header.round),
            //soci::use(*tx.header.timestamp), // TODO add this
            soci::use(tx.header.firstValid),
            soci::use(tx.header.lastValid),
            soci::use(*tx.header.genesisId),
            soci::use(tx.header.genesisHash),
            soci::use(tx.header.sender),
            soci::use(tx.header.fee),
            soci::use(*tx.header.fromRewards),
            soci::use(*tx.header.note),
            soci::use(*tx.header.group),
            soci::use(*tx.header.lease),
            soci::use(*assetConfig.assetId),
            soci::use(*assetParams.assetName),
            soci::use(*assetParams.unitName),
            soci::use(*assetParams.total),
            soci::use(*assetParams.decimals),
            soci::use(*assetParams.defaultFrozen),
            soci::use(*assetParams.creatorAddr),
            soci::use(*assetParams.managerAddr),
            soci::use(*assetParams.reserveAddr),
            soci::use(*assetParams.freezeAddr),
            soci::use(*assetParams.clawbackAddr),
            soci::use(*assetParams.metaDataHash),
            soci::use(*assetParams.url);
    }

    static void putAssetTransferTransaction(soci::session & sql, const std::string & txUid, const model::Transaction & tx) {
        auto& assetTransfer = boost::get<model::AssetTransferTxnFields>(tx.details);

        sql << "INSERT INTO algorand_transactions "
        // Header fields
        "(uid, hash, type, round, "
        //"timestamp, " // TODO add this
        "first_valid, last_valid, genesis_id, genesis_hash, "
        "sender, fee, from_rewards, note, group, lease, "
        // Details fields
        "axfer_asset_id, axfer_asset_amount, axfer_receiver_address, axfer_close_address, axfer_sender_address) "
        "VALUES(:tx_uid, :hash, :tx_type, :round, "
        //":timestamp, "" // TODO add this
        ":first_valid, :last_valid, :genesis_id, :genesis_hash, :sender, :fee, :from_rewards, :note, :group, :lease, "
        ":asset_id, :amount, :receiver_addr, :close_addr, :sender_addr)",
            soci::use(txUid),
            soci::use(*tx.header.id),
            soci::use(tx.header.type),
            soci::use(*tx.header.round),
            //soci::use(*tx.header.timestamp), // TODO add this
            soci::use(tx.header.firstValid),
            soci::use(tx.header.lastValid),
            soci::use(*tx.header.genesisId),
            soci::use(tx.header.genesisHash),
            soci::use(tx.header.sender),
            soci::use(tx.header.fee),
            soci::use(*tx.header.fromRewards),
            soci::use(*tx.header.note),
            soci::use(*tx.header.group),
            soci::use(*tx.header.lease),
            soci::use(assetTransfer.assetId),
            soci::use(*assetTransfer.assetAmount),
            soci::use(assetTransfer.assetReceiver),
            soci::use(*assetTransfer.assetCloseTo),
            soci::use(*assetTransfer.assetSender);
    }

    static void putAssetFreezeTransaction(soci::session & sql, const std::string & txUid, const model::Transaction & tx) {
        auto& assetFreeze = boost::get<model::AssetFreezeTxnFields>(tx.details);

        sql << "INSERT INTO algorand_transactions "
        // Header fields
        "(uid, hash, type, round, "
        //"timestamp, " // TODO add this
        "first_valid, last_valid, genesis_id, genesis_hash, "
        "sender, fee, from_rewards, note, group, lease, "
        // Details fields
        "afrz_asset_id, afrz_frozen, afrz_frozen_address) "
        "VALUES(:tx_uid, :hash, :tx_type, :round, "
        //":timestamp, "" // TODO add this
        ":first_valid, :last_valid, :genesis_id, :genesis_hash, :sender, :fee, :from_rewards, :note, :group, :lease, "
        ":asset_id, :frozen, :frozen_addr)",
            soci::use(txUid),
            soci::use(*tx.header.id),
            soci::use(tx.header.type),
            soci::use(*tx.header.round),
            //soci::use(*tx.header.timestamp), // TODO add this
            soci::use(tx.header.firstValid),
            soci::use(tx.header.lastValid),
            soci::use(*tx.header.genesisId),
            soci::use(tx.header.genesisHash),
            soci::use(tx.header.sender),
            soci::use(tx.header.fee),
            soci::use(*tx.header.fromRewards),
            soci::use(*tx.header.note),
            soci::use(*tx.header.group),
            soci::use(*tx.header.lease),
            soci::use(assetFreeze.assetId),
            soci::use(assetFreeze.assetFrozen),
            soci::use(assetFreeze.frozenAddress);
    }

    static void inflateTransaction(soci::session & sql, const soci::row & row, model::Transaction & tx) {

        // FIXME Make sure columns numbers are correct
        // FIXME Manage all Option<> specifically?...

        tx.header.id = row.get<std::string>(COL_TX_HASH);
        tx.header.type = row.get<std::string>(COL_TX_TYPE);
        tx.header.round = soci::get_number<uint64_t>(row, COL_TX_ROUND);
        //tx.header.timestamp = soci::get_number<uint64_t>(row, COL_TX_TIMESTAMP); // TODO Add this
        tx.header.firstValid = soci::get_number<uint64_t>(row, COL_TX_FIRST_VALID);
        tx.header.lastValid = soci::get_number<uint64_t>(row, COL_TX_LAST_VALID);
        tx.header.genesisId = row.get<std::string>(COL_TX_GENESIS_ID);
        tx.header.genesisHash = B64String(row.get<std::string>(COL_TX_GENESIS_HASH));
        tx.header.sender = Address(row.get<std::string>(COL_TX_SENDER)); // FIXME Currency not specified: defaults to ALGO
        tx.header.fee = soci::get_number<uint64_t>(row, COL_TX_FEE);
        tx.header.fromRewards = soci::get_number<uint64_t>(row, COL_TX_FROM_REWARDS);

        // FIXME Test the following & chose

        //*
        if (row.get_indicator(COL_TX_NOTE) != soci::i_null) {
            tx.header.note = std::vector<uint8_t>();
            BaseConverter::decode(row.get<std::string>(COL_TX_NOTE), BaseConverter::BASE64_RFC4648, *tx.header.note);
        }
        /*/
        auto note = row.get<std::string>(COL_TX_NOTE);
        if (!note.empty()) {
            tx.header.note = std::vector<uint8_t>();
            BaseConverter::decode(note, BaseConverter::BASE64_RFC4648, *tx.header.note);
        }
        /*/
        if (row.get_indicator(COL_TX_GROUP) != soci::i_null) {
            tx.header.group = std::vector<uint8_t>();
            BaseConverter::decode(row.get<std::string>(COL_TX_GROUP), BaseConverter::BASE64_RFC4648, *tx.header.group);
        }
        /*/
        auto group = row.get<std::string>(COL_TX_GROUP);
        if (!group.empty()) {
            tx.header.group = std::vector<uint8_t>();
            BaseConverter::decode(group, BaseConverter::BASE64_RFC4648, *tx.header.group);
        }
        /*/
        if (row.get_indicator(COL_TX_LEASE) != soci::i_null) {
            tx.header.lease = std::vector<uint8_t>();
            BaseConverter::decode(row.get<std::string>(COL_TX_LEASE), BaseConverter::BASE64_RFC4648, *tx.header.lease);
        }
        /*/
        auto lease = row.get<std::string>(COL_TX_LEASE);
        if (!lease.empty()) {
            tx.header.lease = std::vector<uint8_t>();
            BaseConverter::decode(lease, BaseConverter::BASE64_RFC4648, *tx.header.lease);
        }
        //*/


        if (tx.header.type == constants::xPay) {

            tx.details = model::PaymentTxnFields();
            auto& payment = boost::get<model::PaymentTxnFields>(tx.details);
            payment.amount = soci::get_number<uint64_t>(row, COL_TX_PAY_AMOUNT);
            payment.receiverAddr = row.get<std::string>(COL_TX_PAY_RECEIVER_ADDRESS);
            payment.receiverRewards = soci::get_number<uint64_t>(row, COL_TX_PAY_RECEIVER_REWARDS);
            payment.closeAddr = row.get<std::string>(COL_TX_PAY_CLOSE_ADDRESS);
            payment.closeAmount = soci::get_number<uint64_t>(row, COL_TX_PAY_CLOSE_AMOUNT);
            payment.closeRewards = soci::get_number<uint64_t>(row, COL_TX_PAY_CLOSE_REWARDS);

        } else if (tx.header.type == constants::xKeyregs) {

            tx.details = model::KeyRegTxnFields();
            auto& keyreg = boost::get<model::KeyRegTxnFields>(tx.details);
            keyreg.nonParticipation = !! soci::get_number<int>(row, COL_TX_KEYREG_NONPART);
            keyreg.selectionPk = row.get<std::string>(COL_TX_KEYREG_SELECTION_PK);
            keyreg.votePk = row.get<std::string>(COL_TX_KEYREG_VOTE_PK);
            keyreg.voteKeyDilution = soci::get_number<uint64_t>(row, COL_TX_KEYREG_VOTE_KEY_DILUTION);
            keyreg.voteFirst = soci::get_number<uint64_t>(row, COL_TX_KEYREG_VOTE_FIRST);
            keyreg.voteLast = soci::get_number<uint64_t>(row, COL_TX_KEYREG_VOTE_LAST);

        } else if (tx.header.type == constants::xAcfg) {

            tx.details = model::AssetConfigTxnFields();
            auto& assetConfig = boost::get<model::AssetConfigTxnFields>(tx.details);
            if (row.get_indicator(COL_TX_ACFG_ASSET_ID) != soci::i_null) {
                assetConfig.assetId = soci::get_number<uint64_t>(row, COL_TX_ACFG_ASSET_ID);
            }
            if (row.get_indicator(COL_TX_ACFG_TOTAL) != soci::i_null) {
                assetConfig.assetParams = model::AssetParams();
                auto& assetParams = *assetConfig.assetParams;

                assetParams.assetName = row.get<std::string>(COL_TX_ACFG_ASSET_NAME);
                assetParams.unitName = row.get<std::string>(COL_TX_ACFG_UNIT_NAME);
                assetParams.total = soci::get_number<uint64_t>(row, COL_TX_ACFG_TOTAL);
                assetParams.decimals = soci::get_number<uint64_t>(row, COL_TX_ACFG_DECIMALS);
                assetParams.defaultFrozen = !! soci::get_number<int>(row, COL_TX_ACFG_DEFAULT_FROZEN);
                assetParams.creatorAddr = row.get<std::string>(COL_TX_ACFG_CREATOR_ADDRESS);
                assetParams.managerAddr = row.get<std::string>(COL_TX_ACFG_MANAGER_ADDRESS);
                assetParams.reserveAddr = row.get<std::string>(COL_TX_ACFG_RESERVE_ADDRESS);
                assetParams.freezeAddr = row.get<std::string>(COL_TX_ACFG_FREEZE_ADDRESS);
                assetParams.clawbackAddr = row.get<std::string>(COL_TX_ACFG_CLAWBACK_ADDRESS);
                assetParams.metaDataHash = row.get<std::string>(COL_TX_ACFG_METADATA_HASH);
                assetParams.url = row.get<std::string>(COL_TX_ACFG_URL);
            }

        } else if (tx.header.type == constants::xAxfer) {

            tx.details = model::AssetTransferTxnFields();
            auto& assetTransfer = boost::get<model::AssetTransferTxnFields>(tx.details);
            assetTransfer.assetId = soci::get_number<uint64_t>(row, COL_TX_AXFER_ASSET_ID);
            assetTransfer.assetAmount = soci::get_number<uint64_t>(row, COL_TX_AXFER_ASSET_AMOUNT);
            assetTransfer.assetReceiver = row.get<std::string>(COL_TX_AXFER_RECEIVER_ADDRESS);
            assetTransfer.assetCloseTo = row.get<std::string>(COL_TX_AXFER_CLOSE_ADDRESS);
            assetTransfer.assetSender = row.get<std::string>(COL_TX_AXFER_SENDER_ADDRESS);

        } else if (tx.header.type == constants::xAfreeze) {

            tx.details = model::AssetFreezeTxnFields();
            auto& assetFreeze = boost::get<model::AssetFreezeTxnFields>(tx.details);
            assetFreeze.assetId = soci::get_number<uint64_t>(row, COL_TX_AFRZ_ASSET_ID);
            assetFreeze.assetFrozen = !! soci::get_number<int>(row, COL_TX_AFRZ_FROZEN);
            assetFreeze.frozenAddress = row.get<std::string>(COL_TX_AFRZ_FROZEN_ADDRESS);

        }
    }

    bool TransactionDatabaseHelper::transactionExists(soci::session & sql, const std::string & txUid) {
        int32_t count = 0;
        sql << "SELECT COUNT(*) FROM algorand_transactions WHERE transaction_uid = :txUid", soci::use(txUid), soci::into(count);
        return count == 1;
    }

    bool TransactionDatabaseHelper::getTransactionByHash(soci::session & sql,
                                                         const std::string & hash,
                                                         model::Transaction & tx) {

        soci::rowset<soci::row> rows = (sql.prepare <<
                "SELECT *"
                "FROM algorand_transactions AS tx "
                "WHERE tx.hash = :hash", soci::use(hash));

        for (auto &row : rows) {
            inflateTransaction(sql, row, tx);
            return true;
        }

        return false;
    }

    std::string TransactionDatabaseHelper::createTransactionUid(const std::string & accountUid, const std::string & txHash) {
        return SHA256::stringToHexHash(fmt::format("uid:{}+{}", accountUid, txHash));
    }

    std::string TransactionDatabaseHelper::putTransaction(soci::session & sql,
                                                          const std::string & accountUid,
                                                          const model::Transaction & tx) {
        auto txUid = createTransactionUid(accountUid, *tx.header.id);

        if (tx.header.type == constants::xPay) {
            putPaymentTransaction(sql, txUid, tx);
        } else if (tx.header.type == constants::xKeyregs) {
            putKeyRegTransaction(sql, txUid, tx);
        } else if (tx.header.type == constants::xAcfg) {
            putAssetConfigTransaction(sql, txUid, tx);
        } else if (tx.header.type == constants::xAxfer) {
            putAssetTransferTransaction(sql, txUid, tx);
        } else if (tx.header.type == constants::xAfreeze) {
            putAssetFreezeTransaction(sql, txUid, tx);
        }

        return txUid;
    }

}
}
}
