#include <cosmos/database/Migrations.hpp>

namespace ledger {
    namespace core {
        int constexpr CosmosMigration::COIN_ID;
        uint32_t constexpr CosmosMigration::CURRENT_VERSION;

        template <> void migrate<1, CosmosMigration>(soci::session& sql) {
        }

        template <> void rollback<1, CosmosMigration>(soci::session& sql) {
        }
    }
}
