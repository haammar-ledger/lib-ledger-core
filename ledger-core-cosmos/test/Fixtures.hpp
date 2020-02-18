#ifndef __FIXTURES_H_
#define __FIXTURES_H_

#include <integration/BaseFixture.hpp>

#include <cosmos/CosmosLikeAccount.hpp>
#include <cosmos/cosmos.hpp>


using namespace ledger::core::cosmos;

namespace ledger {
        namespace testing {
                namespace cosmos {

                        const std::string DEFAULT_ADDRESS = "cosmos1sd4tl9aljmmezzudugs7zlaya7pg2895tyn79r";
                        const std::string DEFAULT_HEX_PUB_KEY = "03d672c1b90c84d9d97522e9a73252a432b77d90a78bf81cdbe35270d9d3dc1c34";

                        std::shared_ptr<core::CosmosLikeAccount> createCosmosLikeAccount(
                                const std::shared_ptr<core::AbstractWallet>& wallet,
                                int32_t index,
                                const core::api::AccountCreationInfo &info);
                        std::shared_ptr<core::CosmosLikeAccount> createCosmosLikeAccount(
                                const std::shared_ptr<core::AbstractWallet>& wallet,
                                int32_t index,
                                const core::api::ExtendedKeyAccountCreationInfo &info);

                        void setupSendMessage(Message& msg, const std::chrono::system_clock::time_point& timeRef);

                        void setupTransactionWithSingleMessage(Transaction& tx, const Message& msg, const std::chrono::system_clock::time_point& timeRef);

                        void assertSameSendMessage(const Message& msgRef, const Message& msgResult);

                        void assertSameTransactionWithSingleMessage(const Transaction& txRef, const Transaction& txResult);
                }
        }
}

#endif // __FIXTURES_H_
