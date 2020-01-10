#ifndef __FIXTURES_H_
#define __FIXTURES_H_

#include <cosmos/CosmosLikeAccount.hpp>


namespace ledger {
        namespace testing {
                namespace cosmos {

                        std::shared_ptr<CosmosLikeAccount> createCosmosLikeAccount(
                                const std::shared_ptr<core::AbstractWallet>& wallet,
                                int32_t index,
                                const api::AccountCreationInfo &info);
                        std::shared_ptr<CosmosLikeAccount> createCosmosLikeAccount(
                                const std::shared_ptr<core::AbstractWallet>& wallet,
                                int32_t index,
                                const api::ExtendedKeyAccountCreationInfo &info);
                }
        }
}

#endif // __FIXTURES_H_
