//
// Created by erik on 2022-07-20.
//

#include "DatabaseCreation.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/DatabaseSQLite.h"

#ifdef POSTGRES_FOUND
#include "common/CommPSQLSocket.h"
#include "common/DatabasePostgres.h"
#endif

std::unique_ptr<Database> createDatabase()
{
    std::string databaseBackend;
    readConfigItem(instance, "database", databaseBackend);

    if (databaseBackend == "postgres") {
#ifdef POSTGRES_FOUND
        return std::make_unique<DatabasePostgres>();
#else
        log(ERROR, "Database specified as 'postgres', but this server is not built with Postgres SQL support.");
        throw std::runtime_error("Database specified as 'postgres', but this server is not built with Postgres SQL support.");
#endif
    } else {
        return std::make_unique<DatabaseSQLite>();
    }
}