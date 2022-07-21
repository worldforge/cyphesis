// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2007 Alistair Riddoch
// Copyright (C) 2018 Erik Ogenvik
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "DatabasePostgres.h"

#include "id.h"
#include "log.h"
#include "debug.h"
#include "globals.h"
#include "compose.hpp"
#include "const.h"

#include <Atlas/Codecs/XML.h>

#include <varconf/config.h>

#include <cstring>
#include <memory>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using String::compose;

typedef Atlas::Codecs::XML Serialiser;

static const bool debug_flag = false;

static void databaseNotice(void*, const char* message)
{
    log(NOTICE, "Notice from database:");
    log_formatted(NOTICE, message);
}

DatabasePostgres::DatabasePostgres() : Database(),
                                       m_connection(nullptr)
{
}

DatabasePostgres::~DatabasePostgres()
{
    if (!pendingQueries.empty()) {
        log(ERROR, compose("Database delete with %1 queries pending",
                           pendingQueries.size()));

    }
    shutdownConnection();
}

bool DatabasePostgres::tuplesOk()
{
    assert(m_connection != nullptr);

    bool status = false;

    PGresult* res;
    while ((res = PQgetResult(m_connection)) != nullptr) {
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            status = true;
        }
        PQclear(res);
    };
    return status;
}

int DatabasePostgres::commandOk()
{
    assert(m_connection != nullptr);

    int status = -1;

    PGresult* res;
    while ((res = PQgetResult(m_connection)) != nullptr) {
        if (PQresultStatus(res) == PGRES_COMMAND_OK) {
            status = 0;
        } else {
            reportError();
        }
        PQclear(res);
    };
    return status;
}

int DatabasePostgres::connect(const std::string& context, std::string& error_msg)
{
    std::stringstream conninfos;

    std::string db_server;
    if (readConfigItem(context, "dbserver", db_server) == 0) {
        if (db_server.empty()) {
            log(WARNING, "Empty database hostname specified in config file. "
                         "Using none.");
        } else {
            conninfos << "host=" << db_server << " ";
        }
    }

    std::string dbname;
    if (context == CYPHESIS) {
        dbname = CYPHESIS;
    } else {
        dbname = compose("%1_%2", CYPHESIS, ::instance);
    }
    readConfigItem(context, "dbname", dbname);
    conninfos << "dbname=" << dbname << " ";

    std::string db_user;
    if (readConfigItem(context, "dbuser", db_user) == 0) {
        if (db_user.empty()) {
            log(WARNING, "Empty username specified in config file. "
                         "Using current user.");
        } else {
            conninfos << "user=" << db_user << " ";
        }
    }

    std::string db_passwd;
    if (readConfigItem(context, "dbpasswd", db_passwd) == 0) {
        conninfos << "password=" << db_passwd << " ";
    }

    const std::string cinfo = conninfos.str();

    m_connection = PQconnectdb(cinfo.c_str());

    if (m_connection == nullptr) {
        error_msg = "Unknown error";
        return -1;
    }

    if (PQstatus(m_connection) != CONNECTION_OK) {
        error_msg = PQerrorMessage(m_connection);
        PQfinish(m_connection);
        m_connection = nullptr;
        return -1;
    }

    return 0;
}

int DatabasePostgres::initConnection()
{
    std::string error_message;

    if (connect(::instance, error_message) != 0) {
        log(ERROR, "Connection to database failed:");
        log_formatted(ERROR, error_message);
        return -1;
    }

    PQsetNoticeProcessor(m_connection, databaseNotice, nullptr);

    return 0;
}

void DatabasePostgres::shutdownConnection()
{
    if (m_connection != nullptr) {
        PQfinish(m_connection);
        m_connection = nullptr;
    }
}

int DatabasePostgres::encodeObject(const MapType& o,
                                   std::string& data)
{
    std::stringstream str;

    Serialiser codec(str, str, m_d);
    Atlas::Message::Encoder enc(codec);

    codec.streamBegin();
    enc.streamMessageElement(o);
    codec.streamEnd();

    std::string raw = str.str();
    char safe[raw.size() * 2 + 1];
    int errcode;

    PQescapeStringConn(m_connection, safe, raw.c_str(), raw.size(), &errcode);

    if (errcode != 0) {
        std::cerr << "ERROR: " << errcode << std::endl << std::flush;
    }

    data = safe;

    return 0;
}

int DatabasePostgres::getObject(const std::string& table,
                                const std::string& key,
                                MapType& o)
{
    assert(m_connection != nullptr);

    debug_print("Database::getObject() " << table << "." << key);
    std::string query = std::string("SELECT * FROM ") + table + " WHERE id = '" + key + "'";

    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        reportError();
        return -1;
    }

    PGresult* res;
    if ((res = PQgetResult(m_connection)) == nullptr) {
        debug_print("Error accessing " << key << " in " << table
                        << " table");
        return -1;
    }
    if (PQntuples(res) < 1 || PQnfields(res) < 2) {
        debug_print("No entry for " << key << " in " << table
                        << " table");
        PQclear(res);
        while ((res = PQgetResult(m_connection)) != nullptr) {
            PQclear(res);
        }
        return -1;
    }
    const char* data = PQgetvalue(res, 0, 1);
    debug_print("Got record " << key << " from database, value " << data);

    int ret = decodeMessage(data, o);
    PQclear(res);

    while ((res = PQgetResult(m_connection)) != nullptr) {
        PQclear(res);
        log(ERROR, "Extra database result to simple query.");
    };

    return ret;
}

void DatabasePostgres::reportError()
{
    assert(m_connection != nullptr);

    char* message = PQerrorMessage(m_connection);
    assert(message != nullptr);

    if (strlen(message) < 2) {
        log(WARNING, "Zero length database error message");
    }
    std::string msg = std::string("DATABASE: ") + message;
    msg = msg.substr(0, msg.size() - 1);
    log(ERROR, msg);
}

DatabaseResult DatabasePostgres::runSimpleSelectQuery(const std::string& query)
{
    assert(m_connection != nullptr);

    debug_print("QUERY: " << query)
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "runSimpleSelectQuery(): Database query error.");
        reportError();
        return DatabaseResult(nullptr);
    }
    debug_print("done")
    PGresult* res;
    if ((res = PQgetResult(m_connection)) == nullptr) {
        log(ERROR, "Error selecting.");
        reportError();
        debug_print("Row query didn't work"
                       )
        return DatabaseResult(nullptr);
    }
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        log(ERROR, "Error selecting row.");
        debug_print("QUERY: " << query)
        reportError();
        PQclear(res);
        res = nullptr;
    }
    PGresult* nres;
    while ((nres = PQgetResult(m_connection)) != nullptr) {
        PQclear(nres);
        log(ERROR, "Extra database result to simple query.");
    };
    return DatabaseResult(std::make_unique<DatabaseResultWorkerPostgres>(res));
}

int DatabasePostgres::runCommandQuery(const std::string& query)
{
    assert(m_connection != nullptr);

    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "runCommandQuery(): Database query error.");
        reportError();
        return -1;
    }
    if (commandOk() != 0) {
        log(ERROR, "Error running command query row.");
        log(NOTICE, query);
        reportError();
        debug_print("Row query didn't work"
                       )
    } else {
        debug_print("Query worked")
        return 0;
    }
    return -1;
}

int DatabasePostgres::registerRelation(std::string& tablename,
                                       const std::string& sourcetable,
                                       const std::string& targettable,
                                       RelationType kind)
{
    assert(m_connection != nullptr);

    tablename = sourcetable + "_" + targettable;

    std::string query = compose("SELECT * FROM %1 WHERE source = 0 "
                                "AND target = 0", tablename);

    debug_print("QUERY: " << query)
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "registerRelation(): Database query error.");
        reportError();
        return -1;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug_print("Table does not yet exist"
                       )
    } else {
        debug_print("Table exists")
        allTables.insert(tablename);
        return 0;
    }

    query = "CREATE TABLE ";
    query += tablename;
    if (kind == OneToOne || kind == ManyToOne) {
        query += " (source integer UNIQUE REFERENCES ";
    } else {
        query += " (source integer REFERENCES ";
    }
    query += sourcetable;
    if (kind == OneToOne || kind == OneToMany) {
        query += " (id), target integer UNIQUE REFERENCES ";
    } else {
        query += " (id), target integer REFERENCES ";
    }
    query += targettable;
    query += " (id) ON DELETE CASCADE ) WITHOUT OIDS";

    debug_print("CREATE QUERY: " << query);
    if (runCommandQuery(query) != 0) {
        return -1;
    }
    allTables.insert(tablename);
#if 0
    if (kind == ManyToOne || kind == OneToOne) {
        return true;
    } else {
        std::string indexQuery = "CREATE INDEX ";
        indexQuery += tablename;
        indexQuery += "_source_idx ON ";
        indexQuery += tablename;
        indexQuery += " (source)";
        return runCommandQuery(indexQuery) == 0;
    }
#else
    return 0;
#endif
}

int DatabasePostgres::registerSimpleTable(const std::string& name,
                                          const MapType& row)
{
    assert(m_connection != nullptr);

    if (row.empty()) {
        log(ERROR, "Attempt to create empty database table");
    }
    // Check whether the table exists
    std::string query = "SELECT * FROM ";
    std::string createquery = "CREATE TABLE ";
    query += name;
    createquery += name;
    query += " WHERE id = 0";
    createquery += " (id integer UNIQUE PRIMARY KEY";
    auto Iend = row.end();
    for (auto I = row.begin(); I != Iend; ++I) {
        query += " AND ";
        createquery += ", ";
        const std::string& column = I->first;
        query += column;
        createquery += column;
        const Element& type = I->second;
        if (type.isString()) {
            query += " LIKE 'foo'";
            std::size_t size = type.String().size();
            if (size == 0) {
                createquery += " text";
            } else {
                createquery += " varchar(";
                createquery += std::to_string(size);
                createquery += ")";
            }
        } else if (type.isInt()) {
            query += " = 1";
            createquery += " integer";
        } else if (type.isFloat()) {
            query += " = 1.0";
            createquery += " float";
        } else {
            log(ERROR, "Illegal column type in database simple row");
        }
    }

    debug_print("QUERY: " << query)
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "registerSimpleTable(): Database query error.");
        reportError();
        return -1;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug_print("Table does not yet exist"
                       )
    } else {
        debug_print("Table exists")
        allTables.insert(name);
        return 0;
    }

    createquery += ") WITHOUT OIDS";
    debug_print("CREATE QUERY: " << createquery);
    int ret = runCommandQuery(createquery);
    if (ret == 0) {
        allTables.insert(name);
    }
    return ret;
}

int DatabasePostgres::registerEntityIdGenerator()
{
    assert(m_connection != nullptr);

    clearPendingQuery();
    int status = PQsendQuery(m_connection, "SELECT * FROM entity_ent_id_seq");
    if (!status) {
        log(ERROR, "registerEntityIdGenerator(): Database query error.");
        reportError();
        return -1;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug_print("Sequence does not yet exist"
                       )
    } else {
        debug_print("Sequence exists")
        return 0;
    }
    return runCommandQuery("CREATE SEQUENCE entity_ent_id_seq");
}

long DatabasePostgres::newId()
{
    assert(m_connection != nullptr);

    clearPendingQuery();
    int status = PQsendQuery(m_connection,
                             "SELECT nextval('entity_ent_id_seq')");
    if (!status) {
        log(ERROR, "newId(): Database query error.");
        reportError();
        return -1;
    }
    PGresult* res;
    if ((res = PQgetResult(m_connection)) == nullptr) {
        log(ERROR, "Error getting new ID.");
        reportError();
        return -1;
    }
    std::string cid = PQgetvalue(res, 0, 0);
    PQclear(res);
    while ((res = PQgetResult(m_connection)) != nullptr) {
        PQclear(res);
        log(ERROR, "Extra database result to simple query.");
    };
    if (cid.empty()) {
        log(ERROR, "Unknown error getting ID from database.");
        return -1;
    }
    return forceIntegerId(cid);
}

int DatabasePostgres::registerEntityTable(const std::map<std::string, int>& chunks)
{
    assert(m_connection != nullptr);

    clearPendingQuery();
    int status = PQsendQuery(m_connection, "SELECT * FROM entities");
    if (!status) {
        log(ERROR, "registerEntityIdGenerator(): Database query error.");
        reportError();
        return -1;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug_print("Table does not yet exist"
                       )
    } else {
        allTables.insert("entities");
        // FIXME Flush out the whole state of the databases, to ensure they
        // don't clog up while we are testing.
        // runCommandQuery("DELETE FROM properties");
        // runCommandQuery(compose("DELETE FROM entities WHERE id!=%1",
        // consts::rootWorldIntId));
        debug_print("Table exists")
        return 0;
    }
    std::string query = compose("CREATE TABLE entities ("
                                "id integer UNIQUE PRIMARY KEY, "
                                "loc integer, "
                                "type varchar(%1), "
                                "seq integer", consts::id_len);
    auto I = chunks.begin();
    auto Iend = chunks.end();
    for (; I != Iend; ++I) {
        query += compose(", %1 varchar(1024)", I->first);
    }
    query += ")";
    if (runCommandQuery(query) != 0) {
        return -1;
    }
    allTables.insert("entities");
    query = compose("INSERT INTO entities VALUES (%1, null, 'world')",
                    consts::rootWorldIntId);
    if (runCommandQuery(query) != 0) {
        return -1;
    }
    return 0;
}

int DatabasePostgres::registerPropertyTable()
{
    assert(m_connection != nullptr);

    clearPendingQuery();
    int status = PQsendQuery(m_connection, "SELECT * FROM properties");
    if (!status) {
        log(ERROR, "registerPropertyIdGenerator(): Database query error.");
        reportError();
        return -1;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug_print("Table does not yet exist"
                       )
    } else {
        allTables.insert("properties");
        debug_print("Table exists")
        return 0;
    }
    allTables.insert("properties");
    std::string query = compose("CREATE TABLE properties ("
                                "id integer REFERENCES entities "
                                "ON DELETE CASCADE, "
                                "name varchar(%1), "
                                "value text)", consts::id_len);
    if (runCommandQuery(query) != 0) {
        reportError();
        return -1;
    }
    query = "CREATE INDEX property_names on properties (name)";
    if (runCommandQuery(query) != 0) {
        reportError();
        return -1;
    }
    return 0;
}


int DatabasePostgres::registerThoughtsTable()
{
    assert(m_connection != nullptr);

    clearPendingQuery();
    int status = PQsendQuery(m_connection, "SELECT * FROM thoughts");
    if (!status) {
        log(ERROR, "registerThoughtsTable(): Database query error.");
        reportError();
        return -1;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug_print("Table does not yet exist"
                       )
    } else {
        allTables.insert("thoughts");
        debug_print("Table exists")
        return 0;
    }
    allTables.insert("properties");
    std::string query = "CREATE TABLE thoughts ("
                        "id integer REFERENCES entities "
                        "ON DELETE CASCADE, "
                        "thought text)";
    if (runCommandQuery(query) != 0) {
        reportError();
        return -1;
    }
    return 0;
}

// General functions for handling queries at the low level.

void DatabasePostgres::queryResult(ExecStatusType status)
{
    if (!m_queryInProgress || pendingQueries.empty()) {
        log(ERROR, "Got database result when no query was pending.");
        return;
    }
    DatabaseQuery& q = pendingQueries.front();
    if (q.second == PGRES_EMPTY_QUERY) {
        log(ERROR, "Got database result which is already done.");
        return;
    }
    if (q.second == status) {
        debug_print("Query status ok")
        // Mark this query as done
        q.second = PGRES_EMPTY_QUERY;
    } else {
        log(ERROR, "Database error from async query");
        std::cerr << "Query error in : " << q.first << std::endl << std::flush;
        reportError();
        q.second = PGRES_EMPTY_QUERY;
    }
}

void DatabasePostgres::queryComplete()
{
    if (!m_queryInProgress || pendingQueries.empty()) {
        log(ERROR, "Got database query complete when no query was pending");
        return;
    }
    DatabaseQuery& q = pendingQueries.front();
    if (q.second != PGRES_EMPTY_QUERY) {
        log(ERROR, "Got database query complete when query was not done");
        return;
    }
    debug_print("Query complete")
    pendingQueries.pop_front();
    m_queryInProgress = false;
}

int DatabasePostgres::launchNewQuery()
{
    if (m_connection == nullptr) {
        log(ERROR, "Can't launch new query while database is offline.");
        return -1;
    }
    if (m_queryInProgress) {
        log(ERROR, "Launching new query when query is in progress");
        return -1;
    }
    if (pendingQueries.empty()) {
        debug_print("No queries to launch")
        return -1;
    }
    debug_print(pendingQueries.size() << " queries pending");
    DatabaseQuery& q = pendingQueries.front();
    debug_print("Launching async query: " << q.first);
    int status = PQsendQuery(m_connection, q.first.c_str());
    if (!status) {
        log(ERROR, "Database query error when launching.");
        reportError();
        return -1;
    } else {
        m_queryInProgress = true;
        PQflush(m_connection);
        return 0;
    }
}

int DatabasePostgres::scheduleCommand(const std::string& query)
{
    pendingQueries.push_back(std::make_pair(query, PGRES_COMMAND_OK));
    if (!m_queryInProgress) {
        debug_print("Query: " << query << " launched");
        return launchNewQuery();
    } else {
        debug_print("Query: " << query << " scheduled");
        return 0;
    }
}

int DatabasePostgres::clearPendingQuery()
{
    if (!m_queryInProgress) {
        return 0;
    }

    assert(!pendingQueries.empty());
    debug_print("Clearing a pending query")

    DatabaseQuery& q = pendingQueries.front();
    if (q.second == PGRES_COMMAND_OK) {
        m_queryInProgress = false;
        pendingQueries.pop_front();
        return commandOk();
    } else {
        log(ERROR, "Pending query wants unknown status");
        return -1;
    }
}

void DatabasePostgres::blockUntilAllQueriesComplete() {
    while (queryQueueSize()) {
        if (!queryInProgress()) {
            launchNewQuery();
        } else {
            clearPendingQuery();
        }
    }
}

int DatabasePostgres::runMaintainance(unsigned int command)
{
    // VACUUM and REINDEX tables from a common store
    if ((command & MAINTAIN_REINDEX) == MAINTAIN_REINDEX) {
        std::string query("REINDEX TABLE ");
        auto Iend = allTables.end();
        for (auto I = allTables.begin(); I != Iend; ++I) {
            scheduleCommand(query + *I);
        }
    }
    if ((command & MAINTAIN_VACUUM) == MAINTAIN_VACUUM) {
        std::string query("VACUUM ");
        if ((command & MAINTAIN_VACUUM_ANALYZE) == MAINTAIN_VACUUM_ANALYZE) {
            query += "ANALYZE ";
        }
        if ((command & MAINTAIN_VACUUM_FULL) == MAINTAIN_VACUUM_FULL) {
            query += "FULL ";
        }
        auto Iend = allTables.end();
        for (auto I = allTables.begin(); I != Iend; ++I) {
            scheduleCommand(query + *I);
        }
    }
    return 0;
}
//
//const char* DatabaseResultPostgres::field(const char* column, int row) const
//{
//    int col_num = PQfnumber(m_res.get(), column);
//    if (col_num == -1) {
//        return "";
//    }
//    return PQgetvalue(m_res.get(), row, col_num);
//}
//
//const char* DatabaseResultPostgres::const_iterator_postgres::column(const char* column) const
//{
//    int col_num = PQfnumber(m_dr.m_res.get(), column);
//    if (col_num == -1) {
//        return "";
//    }
//    return PQgetvalue(m_dr.m_res.get(), m_row, col_num);
//}
//
//void DatabaseResultPostgres::const_iterator_postgres::readColumn(const char* column,
//                                                int& val) const
//{
//    int col_num = PQfnumber(m_dr.m_res.get(), column);
//    if (col_num == -1) {
//        return;
//    }
//    const char* v = PQgetvalue(m_dr.m_res.get(), m_row, col_num);
//    val = static_cast<int>(strtol(v, 0, 10));
//}
//
//void DatabaseResultPostgres::const_iterator_postgres::readColumn(const char* column,
//                                                float& val) const
//{
//    int col_num = PQfnumber(m_dr.m_res.get(), column);
//    if (col_num == -1) {
//        return;
//    }
//    const char* v = PQgetvalue(m_dr.m_res.get(), m_row, col_num);
//    val = strtof(v, 0);
//}
//
//void DatabaseResultPostgres::const_iterator_postgres::readColumn(const char* column,
//                                                double& val) const
//{
//    int col_num = PQfnumber(m_dr.m_res.get(), column);
//    if (col_num == -1) {
//        return;
//    }
//    const char* v = PQgetvalue(m_dr.m_res.get(), m_row, col_num);
//    val = strtod(v, 0);
//}
//
//void DatabaseResultPostgres::const_iterator_postgres::readColumn(const char* column,
//                                                std::string& val) const
//{
//    int col_num = PQfnumber(m_dr.m_res.get(), column);
//    if (col_num == -1) {
//        return;
//    }
//    const char* v = PQgetvalue(m_dr.m_res.get(), m_row, col_num);
//    val = v;
//}
//
//void DatabaseResultPostgres::const_iterator_postgres::readColumn(const char* column,
//                                                MapType& val) const
//{
//    int col_num = PQfnumber(m_dr.m_res.get(), column);
//    if (col_num == -1) {
//        return;
//    }
//    const char* v = PQgetvalue(m_dr.m_res.get(), m_row, col_num);
//    Database::instance().decodeMessage(v, val);
//}
