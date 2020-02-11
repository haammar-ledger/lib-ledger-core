// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from database.djinni

#include "DatabaseBackendType.hpp"  // my header
#include "enum_from_string.hpp"

namespace ledger { namespace core { namespace api {

std::string to_string(const DatabaseBackendType& databaseBackendType) {
    switch (databaseBackendType) {
        case DatabaseBackendType::SQLITE3: return "SQLITE3";
        case DatabaseBackendType::POSTGRESQL: return "POSTGRESQL";
    };
};
template <>
DatabaseBackendType from_string(const std::string& databaseBackendType) {
    if (databaseBackendType == "SQLITE3") return DatabaseBackendType::SQLITE3;
    else return DatabaseBackendType::POSTGRESQL;
};

std::ostream &operator<<(std::ostream &os, const DatabaseBackendType &o)
{
    switch (o) {
        case DatabaseBackendType::SQLITE3:  return os << "SQLITE3";
        case DatabaseBackendType::POSTGRESQL:  return os << "POSTGRESQL";
    }
}

} } }  // namespace ledger::core::api
