// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2007 Alistair Riddoch
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


#include "Database.h"

#include "id.h"
#include "log.h"
#include "debug.h"
#include "globals.h"
#include "compose.hpp"
#include "const.h"

#include <Atlas/Message/MEncoder.h>
#include <Atlas/Message/Element.h>
#include <Atlas/Codecs/XML.h>

#include <varconf/config.h>

#include <sstream>

#include <cstring>
#include <cstdlib>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using String::compose;

typedef Atlas::Codecs::XML Serialiser;

static const bool debug_flag = false;

Database * Database::m_instance = NULL;

static void databaseNotice(void *, const char * message)
{
    log(NOTICE, "Notice from database:");
    log_formatted(NOTICE, message);
}

Database::Database() : m_rule_db("rules"),
                       m_queryInProgress(false),
                       m_connection(NULL)
{
}

Database::~Database()
{
    if (pendingQueries.size() != 0) {
        log(ERROR, compose("Database delete with %1 queries pending",
                           pendingQueries.size()));
    
    }
}

bool Database::tuplesOk()
{
    assert(m_connection != 0);

    bool status = false;

    PGresult * res;
    while ((res = PQgetResult(m_connection)) != NULL) {
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            status = true;
        }
        PQclear(res);
    };
    return status;
}

int Database::commandOk()
{
    assert(m_connection != 0);

    int status = -1;

    PGresult * res;
    while ((res = PQgetResult(m_connection)) != NULL) {
        if (PQresultStatus(res) == PGRES_COMMAND_OK) {
            status = 0;
        } else {
            reportError();
        }
        PQclear(res);
    };
    return status;
}

int Database::createInstanceDatabase()
{
    assert(::instance != CYPHESIS);

    std::string error_message;

    if (connect(CYPHESIS, error_message) != 0) {
        log(ERROR, compose("Connection to master database failed: \n%1",
                           error_message));
        return -1;
    }

    std::string dbname;
    if (::instance == CYPHESIS) {
        dbname = CYPHESIS;
    } else {
        dbname = compose("%1_%2", CYPHESIS, ::instance);
    }
    readConfigItem(::instance, "dbname", dbname);

    if (runCommandQuery(compose("CREATE DATABASE %1", dbname)) != 0) {
        shutdownConnection();
        return -1;
    }

    shutdownConnection();

    return 0;
}

int Database::connect(const std::string & context, std::string & error_msg)
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

    if (m_connection == NULL) {
        error_msg = "Unknown error";
        return -1;
    }

    if (PQstatus(m_connection) != CONNECTION_OK) {
        error_msg = PQerrorMessage(m_connection);
        PQfinish(m_connection);
        m_connection = 0;
        return -1;
    }

    return 0;
}

int Database::initConnection()
{
    std::string error_message;

    if (connect(::instance, error_message) != 0) {
        log(ERROR, "Connection to database failed:");
        log_formatted(ERROR, error_message);
        return -1;
    }

    PQsetNoticeProcessor(m_connection, databaseNotice, 0);

    return 0;
}

int Database::initRule(bool createTables)
{
    assert(m_connection != 0);

    int status = 0;
    clearPendingQuery();
    status = PQsendQuery(m_connection, "SELECT * FROM rules WHERE "
                                       "id = 'test' AND contents = 'test'");
    if (!status) {
        reportError();
        return -1;
    }

    if (!tuplesOk()) {
        debug(std::cout << "Rule table does not exist"
                        << std::endl << std::flush;);
        if (createTables) {
            std::string query = compose("CREATE TABLE rules ( "
                                        "id varchar(%1) PRIMARY KEY, "
                                        "ruleset varchar(%1), "
                                        "contents text ) "
                                        "WITHOUT OIDS", consts::id_len);
            status = PQsendQuery(m_connection, query.c_str());
            if (!status) {
                reportError();
                return -1;
            }
            if (commandOk() != 0) {
                log(ERROR, "Error creating rules table in database");
                reportError();
                return -1;
            }
            allTables.insert("rules");
        } else {
            log(ERROR, "Server table does not exist in database");
            return -1;
        }
    }
    allTables.insert("rules");
    return 0;
}

void Database::shutdownConnection()
{
    if (m_connection != 0) {
        PQfinish(m_connection);
        m_connection = 0;
    }
}

Database * Database::instance()
{
    if (m_instance == NULL) {
        m_instance = new Database();
    }
    return m_instance;
}

void Database::cleanup()
{
    delete m_instance;

    m_instance = 0;
}

int Database::decodeObject(const std::string & data,
                           Root &o)
{
    if (data.empty()) {
        return 0;
    }

    std::stringstream str(data, std::ios::in);

    Serialiser codec(str, m_od);
    Atlas::Message::Encoder enc(codec);

    // Clear the decoder
    m_od.get();

    codec.poll();

    if (!m_od.check()) {
        log(WARNING, "Database entry does not appear to be decodable");
        return -1;
    }

    o = m_od.get();
    return 0;
}

int Database::decodeMessage(const std::string & data,
                            MapType &o)
{
    if (data.empty()) {
        return 0;
    }

    std::stringstream str(data, std::ios::in);

    Serialiser codec(str, m_d);
    Atlas::Message::Encoder enc(codec);

    // Clear the decoder
    m_d.get();

    codec.poll();

    if (!m_d.check()) {
        log(WARNING, "Database entry does not appear to be decodable");
        return -1;
    }

    o = m_d.get();
    return 0;
}

int Database::encodeObject(const MapType & o,
                           std::string & data)
{
    std::stringstream str;

    Serialiser codec(str, m_d);
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

int Database::getObject(const std::string & table,
                        const std::string & key,
                        MapType & o)
{
    assert(m_connection != 0);

    debug(std::cout << "Database::getObject() " << table << "." << key
                    << std::endl << std::flush;);
    std::string query = std::string("SELECT * FROM ") + table + " WHERE id = '" + key + "'";

    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        reportError();
        return -1;
    }

    PGresult * res;
    if ((res = PQgetResult(m_connection)) == NULL) {
        debug(std::cout << "Error accessing " << key << " in " << table
                        << " table" << std::endl << std::flush;);
        return -1;
    }
    if (PQntuples(res) < 1 || PQnfields(res) < 2) {
        debug(std::cout << "No entry for " << key << " in " << table
                        << " table" << std::endl << std::flush;);
        PQclear(res);
        while ((res = PQgetResult(m_connection)) != NULL) {
            PQclear(res);
        }
        return -1;
    }
    const char * data = PQgetvalue(res, 0, 1);
    debug(std::cout << "Got record " << key << " from database, value " << data
                    << std::endl << std::flush;);

    int ret = decodeMessage(data, o);
    PQclear(res);

    while ((res = PQgetResult(m_connection)) != NULL) {
        PQclear(res);
        log(ERROR, "Extra database result to simple query.");
    };

    return ret;
}

int Database::putObject(const std::string & table,
                        const std::string & key,
                        const MapType & o,
                        const StringVector & c)
{
    debug(std::cout << "Database::putObject() " << table << "." << key
                    << std::endl << std::flush;);
    std::stringstream str;

    Serialiser codec(str, m_d);
    Atlas::Message::Encoder enc(codec);

    codec.streamBegin();
    enc.streamMessageElement(o);
    codec.streamEnd();

    debug(std::cout << "Encoded to: " << str.str() << " "
               << str.str().size() << std::endl << std::flush;);
    std::string query = std::string("INSERT INTO ") + table + " VALUES ('" + key;
    StringVector::const_iterator Iend = c.end();
    for (StringVector::const_iterator I = c.begin(); I != Iend; ++I) {
        query += "', '";
        query += *I;
    }
    query += "', '";
    query += str.str();
    query +=  "')";
    return scheduleCommand(query);
}

int Database::updateObject(const std::string & table,
                           const std::string & key,
                           const MapType & o)
{
    debug(std::cout << "Database::updateObject() " << table << "." << key
                    << std::endl << std::flush;);
    std::stringstream str;

    Serialiser codec(str, m_d);
    Atlas::Message::Encoder enc(codec);

    codec.streamBegin();
    enc.streamMessageElement(o);
    codec.streamEnd();

    std::string query = std::string("UPDATE ") + table + " SET contents = '" +
                        str.str() + "' WHERE id='" + key + "'";
    return scheduleCommand(query);
}

int Database::delObject(const std::string & table, const std::string & key)
{
#if 0
    Dbt key, data;

    key.set_data((void*)keystr);
    key.set_size(strlen(keystr) + 1);

    int err;
    if ((err = db.del(NULL, &key, 0)) != 0) {
        debug(cout << "db.del.ERROR! " << err << endl << flush;);
        return false;
    }
    return true;
#endif
    return 0;
}

bool Database::hasKey(const std::string & table, const std::string & key)
{
    assert(m_connection != 0);

    std::string query = std::string("SELECT id FROM ") + table +
                        " WHERE id='" + key + "'";

    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());

    if (!status) {
        reportError();
        return false;
    }

    PGresult * res;
    bool ret = false;
    if ((res = PQgetResult(m_connection)) == NULL) {
        debug(std::cout << "Error accessing " << table
                        << " table" << std::endl << std::flush;);
        return false;
    }
    int results = PQntuples(res);
    if (results > 0) {
        ret = true;
    }
    PQclear(res);
    while ((res = PQgetResult(m_connection)) != NULL) {
        PQclear(res);
    }
    return ret;
}

int Database::getTable(const std::string & table,
                       std::map<std::string, Root> & contents)
{
    if (m_connection == 0) {
        log(CRITICAL, "Database connection is down. This is okay during tests");
        return -1;
    }

    std::string query = std::string("SELECT * FROM ") + table;

    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());

    if (!status) {
        reportError();
        return -1;
    }

    PGresult * res;
    if ((res = PQgetResult(m_connection)) == NULL) {
        debug(std::cout << "Error accessing " << table
                        << " table" << std::endl << std::flush;);
        return -1;
    }
    int results = PQntuples(res);
    if (results < 1 || PQnfields(res) < 2) {
        debug(std::cout << "No entries in " << table
                        << " table" << std::endl << std::flush;);
        PQclear(res);
        while ((res = PQgetResult(m_connection)) != NULL) {
            PQclear(res);
        }
        return -1;
    }
    int id_column = PQfnumber(res, "id"),
        contents_column = PQfnumber(res, "contents");

    if (id_column == -1 || contents_column == -1) {
        log(ERROR, "Could not find 'id' and 'contents' columns in database result");
        return -1;
    }

    Root t;
    for(int i = 0; i < results; ++i) {
        const char * key = PQgetvalue(res, i, id_column);
        const char * data = PQgetvalue(res, i, contents_column);
        debug(std::cout << "Got record " << key << " from database, value "
                   << data << std::endl << std::flush;);

        if (decodeObject(data, t) == 0) {
            contents[key] = t;
        }

    }
    PQclear(res);

    while ((res = PQgetResult(m_connection)) != NULL) {
        PQclear(res);
        log(ERROR, "Extra database result to simple query.");
    };

    return 0;
}

int Database::clearTable(const std::string & table)
{
    std::string query = std::string("DELETE FROM ") + table;
    return scheduleCommand(query);
}

void Database::reportError()
{
    assert(m_connection != 0);

    char * message = PQerrorMessage(m_connection);
    assert(message != NULL);

    if (strlen(message) < 2) {
        log(WARNING, "Zero length database error message");
    }
    std::string msg = std::string("DATABASE: ") + message;
    msg = msg.substr(0, msg.size() - 1);
    log(ERROR, msg);
}

const DatabaseResult Database::runSimpleSelectQuery(const std::string & query)
{
    assert(m_connection != 0);

    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "runSimpleSelectQuery(): Database query error.");
        reportError();
        return DatabaseResult(0);
    }
    debug(std::cout << "done" << std::endl << std::flush;);
    PGresult * res;
    if ((res = PQgetResult(m_connection)) == NULL) {
        log(ERROR, "Error selecting.");
        reportError();
        debug(std::cout << "Row query didn't work"
                        << std::endl << std::flush;);
        return DatabaseResult(0);
    }
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        log(ERROR, "Error selecting row.");
        debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
        reportError();
        PQclear(res);
        res = 0;
    }
    PGresult * nres;
    while ((nres = PQgetResult(m_connection)) != NULL) {
        PQclear(nres);
        log(ERROR, "Extra database result to simple query.");
    };
    return DatabaseResult(res);
}

int Database::runCommandQuery(const std::string & query)
{
    assert(m_connection != 0);

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
        debug(std::cout << "Row query didn't work"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Query worked" << std::endl << std::flush;);
        return 0;
    }
    return -1;
}

int Database::registerRelation(std::string & tablename,
                               const std::string & sourcetable,
                               const std::string & targettable,
                               RelationType kind)
{
    assert(m_connection != 0);

    tablename = sourcetable + "_" + targettable;

    std::string query = compose("SELECT * FROM %1 WHERE source = 0 "
                                "AND target = 0", tablename);

    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "registerRelation(): Database query error.");
        reportError();
        return -1;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug(std::cout << "Table does not yet exist"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Table exists" << std::endl << std::flush;);
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

    debug(std::cout << "CREATE QUERY: " << query
                    << std::endl << std::flush;);
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

const DatabaseResult Database::selectRelation(const std::string & name,
                                              const std::string & id)
{
    std::string query = "SELECT target FROM ";
    query += name;
    query += " WHERE source = ";
    query += id;

    debug(std::cout << "Selecting on id = " << id << " ... " << std::flush;);

    return runSimpleSelectQuery(query);
}

int Database::createRelationRow(const std::string & name,
                                const std::string & id,
                                const std::string & other)
{
    std::string query = "INSERT INTO ";
    query += name;
    query += " (source, target) VALUES (";
    query += id;
    query += ", ";
    query += other;
    query += ")";

    return scheduleCommand(query);
}

int Database::removeRelationRow(const std::string & name,
                                const std::string & id)
{
    std::string query = "DELETE FROM ";
    query += name;
    query += " WHERE source = ";
    query += id;

    return scheduleCommand(query);
}

int Database::removeRelationRowByOther(const std::string & name,
                                       const std::string & other)
{
    std::string query = "DELETE FROM ";
    query += name;
    query += " WHERE target = ";
    query += other;

    return scheduleCommand(query);
}

int Database::registerSimpleTable(const std::string & name,
                                  const MapType & row)
{
    assert(m_connection != 0);

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
    MapType::const_iterator Iend = row.end();
    for (MapType::const_iterator I = row.begin(); I != Iend; ++I) {
        query += " AND ";
        createquery += ", ";
        const std::string & column = I->first;
        query += column;
        createquery += column;
        const Element & type = I->second;
        if (type.isString()) {
            query += " LIKE 'foo'";
            std::size_t size = type.String().size();
            if (size == 0) {
                createquery += " text";
            } else {
                char buf[32];
                snprintf(buf, 32, "%zd", size);
                createquery += " varchar(";
                createquery += buf;
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

    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "registerSimpleTable(): Database query error.");
        reportError();
        return -1;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug(std::cout << "Table does not yet exist"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Table exists" << std::endl << std::flush;);
        allTables.insert(name);
        return 0;
    }

    createquery += ") WITHOUT OIDS";
    debug(std::cout << "CREATE QUERY: " << createquery
                    << std::endl << std::flush;);
    int ret = runCommandQuery(createquery);
    if (ret == 0) {
        allTables.insert(name);
    }
    return ret;
}

const DatabaseResult Database::selectSimpleRow(const std::string & id,
                                               const std::string & name)
{
    std::string query = "SELECT * FROM ";
    query += name;
    query += " WHERE id = ";
    query += id;

    debug(std::cout << "Selecting on id = " << id << " ... " << std::flush;);

    return runSimpleSelectQuery(query);
}

const DatabaseResult Database::selectSimpleRowBy(const std::string & name,
                                                 const std::string & column,
                                                 const std::string & value)
{
    std::string query = "SELECT * FROM ";
    query += name;
    query += " WHERE ";
    query += column;
    query += " = ";
    query += value;

    debug(std::cout << "Selecting on " << column << " = " << value
                    << " ... " << std::flush;);

    return runSimpleSelectQuery(query);
}

int Database::createSimpleRow(const std::string & name,
                               const std::string & id,
                               const std::string & columns,
                               const std::string & values)
{
    std::string query = "INSERT INTO ";
    query += name;
    query += " ( id, ";
    query += columns;
    query += " ) VALUES ( ";
    query += id;
    query += ", ";
    query += values;
    query += ")";

    return scheduleCommand(query);
}

int Database::updateSimpleRow(const std::string & name,
                              const std::string & key,
                              const std::string & value,
                              const std::string & columns)
{
    std::string query = "UPDATE ";
    query += name;
    query += " SET ";
    query += columns;
    query += " WHERE ";
    query += key;
    query += "='";
    query += value;
    query += "'";

    return scheduleCommand(query);
}

int Database::registerEntityIdGenerator()
{
    assert(m_connection != 0);

    clearPendingQuery();
    int status = PQsendQuery(m_connection, "SELECT * FROM entity_ent_id_seq");
    if (!status) {
        log(ERROR, "registerEntityIdGenerator(): Database query error.");
        reportError();
        return -1;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug(std::cout << "Sequence does not yet exist"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Sequence exists" << std::endl << std::flush;);
        return 0;
    }
    return runCommandQuery("CREATE SEQUENCE entity_ent_id_seq");
}

long Database::newId(std::string & id)
{
    assert(m_connection != 0);

    clearPendingQuery();
    int status = PQsendQuery(m_connection,
                             "SELECT nextval('entity_ent_id_seq')");
    if (!status) {
        log(ERROR, "newId(): Database query error.");
        reportError();
        return -1;
    }
    PGresult * res;
    if ((res = PQgetResult(m_connection)) == NULL) {
        log(ERROR, "Error getting new ID.");
        reportError();
        return -1;
    }
    const char * cid = PQgetvalue(res, 0, 0);
    id = cid;
    PQclear(res);
    while ((res = PQgetResult(m_connection)) != NULL) {
        PQclear(res);
        log(ERROR, "Extra database result to simple query.");
    };
    if (id.empty()) {
        log(ERROR, "Unknown error getting ID from database.");
        return -1;
    }
    return forceIntegerId(id);
}

int Database::registerEntityTable(const std::map<std::string, int> & chunks)
{
    assert(m_connection != 0);

    clearPendingQuery();
    int status = PQsendQuery(m_connection, "SELECT * FROM entities");
    if (!status) {
        log(ERROR, "registerEntityIdGenerator(): Database query error.");
        reportError();
        return -1;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug(std::cout << "Table does not yet exist"
                        << std::endl << std::flush;);
    } else {
        allTables.insert("entities");
        // FIXME Flush out the whole state of the databases, to ensure they
        // don't clog up while we are testing.
        // runCommandQuery("DELETE FROM properties");
        // runCommandQuery(compose("DELETE FROM entities WHERE id!=%1",
                                // consts::rootWorldIntId));
        debug(std::cout << "Table exists" << std::endl << std::flush;);
        return 0;
    }
    std::string query = compose("CREATE TABLE entities ("
                                "id integer UNIQUE PRIMARY KEY, "
                                "loc integer, "
                                "type varchar(%1), "
                                "seq integer", consts::id_len);
    std::map<std::string, int>::const_iterator I = chunks.begin();
    std::map<std::string, int>::const_iterator Iend = chunks.end();
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

int Database::insertEntity(const std::string & id,
                           const std::string & loc,
                           const std::string & type,
                           int seq,
                           const std::string & value)
{
    std::string query = compose("INSERT INTO entities VALUES "
                                "(%1, %2, '%3', %4, '%5')",
                                id, loc, type, seq, value);
    return scheduleCommand(query);
}

int Database::updateEntity(const std::string & id,
                           int seq,
                           const std::string & location_data,
                           const std::string & location_entity_id)
{
    std::string query = compose("UPDATE entities SET seq = %1, location = '%2',"
                                " loc = '%3'"
                                " WHERE id = %4", seq, location_data,
                                location_entity_id, id);
    return scheduleCommand(query);
}

int Database::updateEntityWithoutLoc(const std::string & id,
                 int seq,
                 const std::string & location_data)
{
    std::string query = compose("UPDATE entities SET seq = %1, location = '%2'"
                                " WHERE id = %3", seq, location_data, id);
    return scheduleCommand(query);
}


const DatabaseResult Database::selectEntities(const std::string & loc)
{
    std::string query = compose("SELECT id, type, seq, location FROM entities"
                                " WHERE loc = %1", loc);

    debug(std::cout << "Selecting on loc = " << loc << " ... " << std::flush;);

    return runSimpleSelectQuery(query);
}

int Database::dropEntity(long id)
{
    std::string query = compose("DELETE FROM properties WHERE id = '%1'", id);

    scheduleCommand(query);

    query = compose("DELETE FROM entities WHERE id = %1", id);

    scheduleCommand(query);

    query = compose("DELETE FROM thoughts WHERE id = %1", id);

    scheduleCommand(query);

    return 0;
}

int Database::registerPropertyTable()
{
    assert(m_connection != 0);

    clearPendingQuery();
    int status = PQsendQuery(m_connection, "SELECT * FROM properties");
    if (!status) {
        log(ERROR, "registerPropertyIdGenerator(): Database query error.");
        reportError();
        return -1;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug(std::cout << "Table does not yet exist"
                        << std::endl << std::flush;);
    } else {
        allTables.insert("properties");
        debug(std::cout << "Table exists" << std::endl << std::flush;);
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

int Database::insertProperties(const std::string & id,
                               const KeyValues & tuples)
{
    int first = 1;
    std::string query("INSERT INTO properties VALUES ");
    KeyValues::const_iterator I = tuples.begin();
    KeyValues::const_iterator Iend = tuples.end();
    for (; I != Iend; ++I) {
        if (first) {
            query += compose("(%1, '%2', '%3')", id, I->first, I->second);
            first = 0;
        } else {
            query += compose(", (%1, '%2', '%3')", id, I->first, I->second);
        }
    }
    return scheduleCommand(query);
}

const DatabaseResult Database::selectProperties(const std::string & id)
{
    std::string query = compose("SELECT name, value FROM properties"
                                " WHERE id = %1", id);

    debug(std::cout << "Selecting on id = " << id << " ... "
                    << std::endl << std::flush;);

    return runSimpleSelectQuery(query);
}

int Database::updateProperties(const std::string & id,
                               const KeyValues & tuples)
{
    KeyValues::const_iterator I = tuples.begin();
    KeyValues::const_iterator Iend = tuples.end();
    for (; I != Iend; ++I) {
        std::string query = compose("UPDATE properties SET value = '%3' WHERE"
                                    " id=%1 AND name='%2'",
                                    id, I->first, I->second);
        scheduleCommand(query);
    }
    return 0;
}

int Database::registerThoughtsTable()
{
    assert(m_connection != 0);

    clearPendingQuery();
    int status = PQsendQuery(m_connection, "SELECT * FROM thoughts");
    if (!status) {
        log(ERROR, "registerThoughtsTable(): Database query error.");
        reportError();
        return -1;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug(std::cout << "Table does not yet exist"
                        << std::endl << std::flush;);
    } else {
        allTables.insert("thoughts");
        debug(std::cout << "Table exists" << std::endl << std::flush;);
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

const DatabaseResult Database::selectThoughts(const std::string & loc)
{
    std::string query = compose("SELECT thought FROM thoughts"
                                " WHERE id = %1", loc);

    debug(std::cout << "Selecting on id = " << loc << " ... "
                    << std::endl << std::flush;);

    return runSimpleSelectQuery(query);
}

int Database::replaceThoughts(const std::string & id,
                         const std::vector<std::string>& thoughts)
{

    std::string deleteQuery = compose("DELETE FROM thoughts WHERE id=%1", id);
    scheduleCommand(deleteQuery);

    for (auto& thought : thoughts) {
        std::string insertQuery = compose("INSERT INTO thoughts (id, thought)"
                                          " VALUES (%1, '%2')", id, thought);
        scheduleCommand(insertQuery);
    }
    return 0;
}

#if 0
// Interface for tables for sparse sequences or arrays of data. Terrain
// control points and other spatial data.

static const char * array_axes[] = { "i", "j", "k", "l", "m" }; 

bool Database::registerArrayTable(const std::string & name,
                                  unsigned int dimension,
                                  const MapType & row)
{
    if (m_connection == 0) {
        log(CRITICAL, "Database connection is down. This is okay during tests");
        return false;
    }


    assert(dimension <= 5);

    if (row.empty()) {
        log(ERROR, "Attempt to create empty array table");
    }

    std::string query("SELECT * from ");
    std::string createquery("CREATE TABLE ");
    std::string indexquery("CREATE UNIQUE INDEX ");

    query += name;
    query += " WHERE id = 0";

    createquery += name;
    createquery += " (id integer REFERENCES entities NOT NULL";

    indexquery += name;
    indexquery += "_point_idx on ";
    indexquery += name;
    indexquery += " (id";

    for (unsigned int i = 0; i < dimension; ++i) {
        query += " AND ";
        query += array_axes[i];
        query += " = 0";

        createquery += ", ";
        createquery += array_axes[i];
        createquery += " integer NOT NULL";

        indexquery += ", ";
        indexquery += array_axes[i];
    }

    MapType::const_iterator Iend = row.end();
    for (MapType::const_iterator I = row.begin(); I != Iend; ++I) {
        const std::string & column = I->first;

        query += " AND ";
        query += column;

        createquery += ", ";
        createquery += column;

        const Element & type = I->second;

        if (type.isString()) {
            query += " LIKE 'foo'";
            int size = type.String().size();
            if (size == 0) {
                createquery += " text";
            } else {
                char buf[32];
                snprintf(buf, 32, "%d", size);
                createquery += " varchar(";
                createquery += buf;
                createquery += ")";
            }
        } else if (type.isInt()) {
            query += " = 1";
            createquery += " integer";
        } else if (type.isFloat()) {
            query += " = 1.0";
            createquery += " float";
        } else {
            log(ERROR, "Illegal column type in database array row");
        }
    }

    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "registerArrayTable(): Database query error.");
        reportError();
        return false;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug(std::cout << "Table does not yet exist"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Table exists" << std::endl << std::flush;);
        allTables.insert(name);
        return true;
    }

    createquery += ") WITHOUT OIDS";
    debug(std::cout << "CREATE QUERY: " << createquery
                    << std::endl << std::flush;);
    int ret = runCommandQuery(createquery);
    if (ret != 0) {
        return false;
    }
    indexquery += ")";
    debug(std::cout << "INDEX QUERY: " << indexquery
                    << std::endl << std::flush;);
    ret = runCommandQuery(indexquery);
    if (ret != 0) {
        return false;
    }
    allTables.insert(name);
    return true;
}

const DatabaseResult Database::selectArrayRows(const std::string & name,
                                               const std::string & id)
{
    std::string query("SELECT * FROM ");
    query += name;
    query += " WHERE id = ";
    query += id;

    debug(std::cout << "ARRAY QUERY: " << query << std::endl << std::flush;);

    return runSimpleSelectQuery(query);
}

int Database::createArrayRow(const std::string & name,
                             const std::string & id,
                             const std::vector<int> & key,
                             const MapType & data)
{
    assert(key.size() > 0);
    assert(key.size() <= 5);
    assert(!data.empty());

    std::stringstream query;
    query << "INSERT INTO " << name << " ( id";
    for (unsigned int i = 0; i < key.size(); ++i) {
        query << ", " << array_axes[i];
    }
    MapType::const_iterator Iend = data.end();
    for (MapType::const_iterator I = data.begin(); I != Iend; ++I) {
        query << ", " << I->first;
    }
    query << " ) VALUES ( " << id;
    std::vector<int>::const_iterator Jend = key.end();
    for (std::vector<int>::const_iterator J = key.begin(); J != Jend; ++J) {
        query << ", " << *J;
    }
    // We assume data has not been modified, so Iend is still valid
    for (MapType::const_iterator I = data.begin(); I != Iend; ++I) {
        const Element & e = I->second;
        switch (e.getType()) {
          case Element::TYPE_INT:
            query << ", " << e.Int();
            break;
          case Element::TYPE_FLOAT:
            query << ", " << e.Float();
            break;
          case Element::TYPE_STRING:
            query << ", " << e.String();
            break;
          default:
            log(ERROR, "Bad type constructing array database row for insert");
            break;
        }
    }
    query << ")";

    std::string qstr = query.str();
    debug(std::cout << "QUery: " << qstr << std::endl << std::flush;);
    return scheduleCommand(qstr);
}

int Database::updateArrayRow(const std::string & name,
                             const std::string & id,
                             const std::vector<int> & key,
                             const Atlas::Message::MapType & data)
{
    assert(key.size() > 0);
    assert(key.size() <= 5);
    assert(!data.empty());

    std::stringstream query;

    query << "UPDATE " << name << " SET ";
    MapType::const_iterator Iend = data.end();
    for (MapType::const_iterator I = data.begin(); I != Iend; ++I) {
        if (I != data.begin()) {
            query << ", ";
        }
        query << I->first << " = ";
        const Element & e = I->second;
        switch (e.getType()) {
          case Element::TYPE_INT:
            query << e.Int();
            break;
          case Element::TYPE_FLOAT:
            query << e.Float();
            break;
          case Element::TYPE_STRING:
            query << "'" << e.String() << "'";
            break;
          default:
            log(ERROR, "Bad type constructing array database row for update");
            break;
        }
    }
    query << " WHERE id='" << id << "'";
    for (unsigned int i = 0; i < key.size(); ++i) {
        query << " AND " << array_axes[i] << " = " << key[i];
    }
    
    std::string qstr = query.str();
    debug(std::cout << "QUery: " << qstr << std::endl << std::flush;);
    return scheduleCommand(qstr);
}

int Database::removeArrayRow(const std::string & name,
                             const std::string & id,
                             const std::vector<int> & key)
{
    /// Not sure we need this one yet, so lets no bother for now ;)
    return -1;
}
#endif // 0

// General functions for handling queries at the low level.

void Database::queryResult(ExecStatusType status)
{
    if (!m_queryInProgress || pendingQueries.empty()) {
        log(ERROR, "Got database result when no query was pending.");
        return;
    }
    DatabaseQuery & q = pendingQueries.front();
    if (q.second == PGRES_EMPTY_QUERY) {
        log(ERROR, "Got database result which is already done.");
        return;
    }
    if (q.second == status) {
        debug(std::cout << "Query status ok" << std::endl << std::flush;);
        // Mark this query as done
        q.second = PGRES_EMPTY_QUERY;
    } else {
        log(ERROR, "Database error from async query");
        std::cerr << "Query error in : " << q.first << std::endl << std::flush;
        reportError();
        q.second = PGRES_EMPTY_QUERY;
    }
}

void Database::queryComplete()
{
    if (!m_queryInProgress || pendingQueries.empty()) {
        log(ERROR, "Got database query complete when no query was pending");
        return;
    }
    DatabaseQuery & q = pendingQueries.front();
    if (q.second != PGRES_EMPTY_QUERY) {
        abort();
        log(ERROR, "Got database query complete when query was not done");
        return;
    }
    debug(std::cout << "Query complete" << std::endl << std::flush;);
    pendingQueries.pop_front();
    m_queryInProgress = false;
}

int Database::launchNewQuery()
{
    if (m_connection == 0) {
        log(ERROR, "Can't launch new query while database is offline.");
        return -1;
    }
    if (m_queryInProgress) {
        log(ERROR, "Launching new query when query is in progress");
        return -1;
    }
    if (pendingQueries.empty()) {
        debug(std::cout << "No queries to launch" << std::endl << std::flush;);
        return -1;
    }
    debug(std::cout << pendingQueries.size() << " queries pending"
                    << std::endl << std::flush;);
    DatabaseQuery & q = pendingQueries.front();
    debug(std::cout << "Launching async query: " << q.first
                    << std::endl << std::flush;);
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

int Database::scheduleCommand(const std::string & query)
{
    pendingQueries.push_back(std::make_pair(query, PGRES_COMMAND_OK));
    if (!m_queryInProgress) {
        debug(std::cout << "Query: " << query << " launched"
                        << std::endl << std::flush;);
        return launchNewQuery();
    } else {
        debug(std::cout << "Query: " << query << " scheduled"
                        << std::endl << std::flush;);
        return 0;
    }
}

int Database::clearPendingQuery()
{
    if (!m_queryInProgress) {
        return 0;
    }

    assert(!pendingQueries.empty());
    debug(std::cout << "Clearing a pending query" << std::endl << std::flush;);

    DatabaseQuery & q = pendingQueries.front();
    if (q.second == PGRES_COMMAND_OK) {
        m_queryInProgress = false;
        pendingQueries.pop_front();
        return commandOk();
    } else {
        log(ERROR, "Pending query wants unknown status");
        return -1;
    }
}

int Database::runMaintainance(int command)
{
    // VACUUM and REINDEX tables from a common store
    if ((command & MAINTAIN_REINDEX) == MAINTAIN_REINDEX) {
        std::string query("REINDEX TABLE ");
        TableSet::const_iterator Iend = allTables.end();
        for (TableSet::const_iterator I = allTables.begin(); I != Iend; ++I) {
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
        TableSet::const_iterator Iend = allTables.end();
        for(TableSet::const_iterator I = allTables.begin(); I != Iend; ++I) {
            scheduleCommand(query + *I);
        }
    }
    return 0;
}

const char * DatabaseResult::field(const char * column, int row) const
{
    int col_num = PQfnumber(m_res, column);
    if (col_num == -1) {
        return "";
    }
    return PQgetvalue(m_res, row, col_num);
}

const char * DatabaseResult::const_iterator::column(const char * column) const
{
    int col_num = PQfnumber(m_dr.m_res, column);
    if (col_num == -1) {
        return "";
    }
    return PQgetvalue(m_dr.m_res, m_row, col_num);
}

void DatabaseResult::const_iterator::readColumn(const char * column,
                                                int & val) const
{
    int col_num = PQfnumber(m_dr.m_res, column);
    if (col_num == -1) {
        return;
    }
    const char * v = PQgetvalue(m_dr.m_res, m_row, col_num);
    val = strtol(v, 0, 10);
}

void DatabaseResult::const_iterator::readColumn(const char * column,
                                                float & val) const
{
    int col_num = PQfnumber(m_dr.m_res, column);
    if (col_num == -1) {
        return;
    }
    const char * v = PQgetvalue(m_dr.m_res, m_row, col_num);
    val = strtof(v, 0);
}

void DatabaseResult::const_iterator::readColumn(const char * column,
                                                double & val) const
{
    int col_num = PQfnumber(m_dr.m_res, column);
    if (col_num == -1) {
        return;
    }
    const char * v = PQgetvalue(m_dr.m_res, m_row, col_num);
    val = strtod(v, 0);
}

void DatabaseResult::const_iterator::readColumn(const char * column,
                                                std::string & val) const
{
    int col_num = PQfnumber(m_dr.m_res, column);
    if (col_num == -1) {
        return;
    }
    const char * v = PQgetvalue(m_dr.m_res, m_row, col_num);
    val = v;
}

void DatabaseResult::const_iterator::readColumn(const char * column,
                                                MapType & val) const
{
    int col_num = PQfnumber(m_dr.m_res, column);
    if (col_num == -1) {
        return;
    }
    const char * v = PQgetvalue(m_dr.m_res, m_row, col_num);
    Database::instance()->decodeMessage(v, val);
}
