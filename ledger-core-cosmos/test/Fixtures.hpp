#ifndef __FIXTURES_H_
#define __FIXTURES_H_

#include <integration/BaseFixture.hpp>

#include <cosmos/CosmosLikeAccount.hpp>


namespace ledger {
        namespace testing {
                namespace cosmos {

                        std::shared_ptr<core::CosmosLikeAccount> createCosmosLikeAccount(
                                const std::shared_ptr<core::AbstractWallet>& wallet,
                                int32_t index,
                                const core::api::AccountCreationInfo &info);
                        std::shared_ptr<core::CosmosLikeAccount> createCosmosLikeAccount(
                                const std::shared_ptr<core::AbstractWallet>& wallet,
                                int32_t index,
                                const core::api::ExtendedKeyAccountCreationInfo &info);
                }
        }
}

#endif // __FIXTURES_H_
