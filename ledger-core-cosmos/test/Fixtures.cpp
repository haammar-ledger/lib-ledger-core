#include "Fixtures.hpp"

namespace ledger {
        namespace testing {
                namespace cosmos {

                        std::shared_ptr<core::CosmosLikeAccount>
                        createCosmosLikeAccount(const std::shared_ptr<core::AbstractWallet>& wallet,
                                                int32_t index,
                                                const api::AccountCreationInfo &info) {
                                auto i = info;
                                i.index = index;
                                return std::dynamic_pointer_cast<core::CosmosLikeAccount>(::wait(wallet->newAccountWithInfo(i)));
                        }

                        std::shared_ptr<core::CosmosLikeAccount>
                        createCosmosLikeAccount(const std::shared_ptr<core::AbstractWallet>& wallet,
                                                int32_t index,
                                                const api::ExtendedKeyAccountCreationInfo &info) {
                                auto i = info;
                                i.index = index;
                                return std::dynamic_pointer_cast<CosmosLikeAccount>(::wait(wallet->newAccountWithExtendedKeyInfo(i)));
                        }

                }
        }
}
