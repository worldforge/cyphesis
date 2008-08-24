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

// $Id$

#include "Database.h"

#include "id.h"
#include "log.h"
#include "debug.h"
#include "globals.h"
#include "compose.hpp"

#include <Atlas/Message/MEncoder.h>
#include <Atlas/Message/Element.h>
#include <Atlas/Codecs/XML.h>

#include <varconf/config.h>

#include <iostream>
#include <sstream>

#include <cstring>
#include <cstdlib>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;

typedef Atlas::Codecs::XML Serialiser;

static const bool debug_flag = false;

Database * Database::m_instance = NULL;

static void databaseNotice(void * arg, const char * message)
{
    std::string msg = std::string("DATABASE: ") + message;
    // Remove the trailing \n from the message.
    msg = msg.substr(0, msg.size() - 1);
    log(NOTICE, msg);
}

Database::Database() : m_rule_db("rules"),
                       m_queryInProgress(false),
                       m_connection(NULL)
{
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

bool Database::commandOk()
{
    assert(m_connection != 0);

    bool status = false;

    PGresult * res;
    while ((res = PQgetResult(m_connection)) != NULL) {
        if (PQresultStatus(res) == PGRES_COMMAND_OK) {
            status = true;
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
        log(ERROR, String::compose("Connection to master database failed: \n%1",
                                   error_message));
        return -1;
    }

    std::string dbname;
    if (::instance == CYPHESIS) {
        dbname = CYPHESIS;
    } else {
        dbname = String::compose("%1_%2", CYPHESIS, ::instance);
    }
    readConfigItem(::instance, "dbname", dbname);

    if (!runCommandQuery(String::compose("CREATE DATABASE %1", dbname))) {
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
        dbname = String::compose("%1_%2", CYPHESIS, ::instance);
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
        log(ERROR, String::compose("Connection to database failed: \n%1",
                                   error_message));
        return -1;
    }

    PQsetNoticeProcessor(m_connection, databaseNotice, 0);

    return 0;
}

bool Database::initRule(bool createTables)
{
    assert(m_connection != 0);

    int status = 0;
    clearPendingQuery();
    status = PQsendQuery(m_connection, "SELECT * FROM rules WHERE id = 'test' AND contents = 'test'");
    if (!status) {
        reportError();
        return false;
    }

    if (!tuplesOk()) {
        debug(std::cout << "Rule table does not exist"
                        << std::endl << std::flush;);
        if (createTables) {
            status = PQsendQuery(m_connection, "CREATE TABLE rules ( id varchar(80) PRIMARY KEY, ruleset varchar(32), contents text ) WITHOUT OIDS");
            if (!status) {
                reportError();
                return false;
            }
            if (!commandOk()) {
                log(ERROR, "Error creating rules table in database");
                reportError();
                return false;
            }
            allTables.insert("rules");
        } else {
            log(ERROR, "Server table does not exist in database");
            return false;
        }
    }
    allTables.insert("rules");
    return true;
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
    if (m_instance != 0) {
        delete m_instance;
    }

    m_instance = 0;
}

bool Database::decodeObject(const std::string & data,
                            Root &o)
{
    if (data.empty()) {
        return true;
    }

    std::stringstream str(data, std::ios::in);

    Serialiser codec(str, m_od);
    Atlas::Message::Encoder enc(codec);

    // Clear the decoder
    m_d.get();

    codec.poll();

    if (!m_od.check()) {
        log(WARNING, "Database entry does not appear to be decodable");
        return false;
    }

    o = m_od.get();
    return true;
}

bool Database::decodeMessage(const std::string & data,
                             MapType &o)
{
    if (data.empty()) {
        return true;
    }

    std::stringstream str(data, std::ios::in);

    Serialiser codec(str, m_d);
    Atlas::Message::Encoder enc(codec);

    // Clear the decoder
    m_d.get();

    codec.poll();

    if (!m_d.check()) {
        log(WARNING, "Database entry does not appear to be decodable");
        return false;
    }

    o = m_d.get();
    return true;
}

bool Database::encodeObject(const MapType & o,
                            std::string & data)
{
    std::stringstream str;

    Serialiser codec(str, m_d);
    Atlas::Message::Encoder enc(codec);

    codec.streamBegin();
    enc.streamMessageElement(o);
    codec.streamEnd();

    data = str.str();
    return true;
}

bool Database::getObject(const std::string & table, const std::string & key,
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
        return false;
    }

    PGresult * res;
    if ((res = PQgetResult(m_connection)) == NULL) {
        debug(std::cout << "Error accessing " << key << " in " << table
                        << " table" << std::endl << std::flush;);
        return false;
    }
    if (PQntuples(res) < 1 || PQnfields(res) < 2) {
        debug(std::cout << "No entry for " << key << " in " << table
                        << " table" << std::endl << std::flush;);
        PQclear(res);
        while ((res = PQgetResult(m_connection)) != NULL) {
            PQclear(res);
        }
        return false;
    }
    const char * data = PQgetvalue(res, 0, 1);
    debug(std::cout << "Got record " << key << " from database, value " << data
                    << std::endl << std::flush;);

    bool ret = decodeMessage(data, o);
    PQclear(res);

    while ((res = PQgetResult(m_connection)) != NULL) {
        PQclear(res);
        log(ERROR, "Extra database result to simple query.");
    };

    return ret;
}

bool Database::putObject(const std::string & table,
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

bool Database::updateObject(const std::string & table,
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

bool Database::delObject(const std::string & table, const std::string & key)
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
    return true;
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

bool Database::getTable(const std::string & table,
                        std::map<std::string, Root> & contents)
{
    if (m_connection == 0) {
        log(CRITICAL, "Database connection is down. This is okay during tests");
        return false;
    }

    std::string query = std::string("SELECT * FROM ") + table;

    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());

    if (!status) {
        reportError();
        return false;
    }

    PGresult * res;
    if ((res = PQgetResult(m_connection)) == NULL) {
        debug(std::cout << "Error accessing " << table
                        << " table" << std::endl << std::flush;);
        return false;
    }
    int results = PQntuples(res);
    if (results < 1 || PQnfields(res) < 2) {
        debug(std::cout << "No entries in " << table
                        << " table" << std::endl << std::flush;);
        PQclear(res);
        while ((res = PQgetResult(m_connection)) != NULL) {
            PQclear(res);
        }
        return false;
    }
    int id_column = PQfnumber(res, "id"),
        contents_column = PQfnumber(res, "contents");

    if (id_column == -1 || contents_column == -1) {
        log(ERROR, "Could not find 'id' and 'contents' columns in database result");
        return false;
    }

    Root t;
    for(int i = 0; i < results; ++i) {
        const char * key = PQgetvalue(res, i, id_column);
        const char * data = PQgetvalue(res, i, contents_column);
        debug(std::cout << "Got record " << key << " from database, value "
                   << data << std::endl << std::flush;);

        if (decodeObject(data, t)) {
            contents[key] = t;
        }

    }
    PQclear(res);

    while ((res = PQgetResult(m_connection)) != NULL) {
        PQclear(res);
        log(ERROR, "Extra database result to simple query.");
    };

    return true;
}

bool Database::clearTable(const std::string & table)
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

bool Database::runCommandQuery(const std::string & query)
{
    assert(m_connection != 0);

    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "runCommandQuery(): Database query error.");
        reportError();
        return false;
    }
    if (!commandOk()) {
        log(ERROR, "Error running command query row.");
        log(NOTICE, query);
        reportError();
        debug(std::cout << "Row query didn't work"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Query worked" << std::endl << std::flush;);
        return true;
    }
    return false;
}

bool Database::registerRelation(std::string & tablename,
                                const std::string & sourcetable,
                                const std::string & targettable,
                                RelationType kind)
{
    assert(m_connection != 0);

    tablename = sourcetable + "_" + targettable;

    std::string query = "SELECT * FROM ";
    query += tablename;
    query += " WHERE source = 0 AND target = 0";

    std::string createquery = "CREATE TABLE ";
    createquery += tablename;
    if (kind == OneToOne || kind == ManyToOne) {
        createquery += " (source integer UNIQUE REFERENCES ";
    } else {
        createquery += " (source integer REFERENCES ";
    }
    createquery += sourcetable;
#if 0
    // FIXME Referential integrity not supported on inherited tables.
    if (kind == OneToOne || kind == OneToMany) {
        createquery += " (id), target integer UNIQUE REFERENCES ";
    } else {
        createquery += " (id), target integer REFERENCES ";
    }
    createquery += targettable;
    createquery += " (id))";
#else
    if (kind == OneToOne || kind == OneToMany) {
        createquery += " (id), target integer UNIQUE) WITHOUT OIDS";
    } else {
        createquery += " (id), target integer) WITHOUT OIDS";
    }
#endif

    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "registerRelation(): Database query error.");
        reportError();
        return false;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug(std::cout << "Table does not yet exist"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Table exists" << std::endl << std::flush;);
        allTables.insert(tablename);
        return true;
    }

    debug(std::cout << "CREATE QUERY: " << createquery
                    << std::endl << std::flush;);
    if (!runCommandQuery(createquery)) {
        return false;
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
        return runCommandQuery(indexQuery);
    }
#else
    return true;
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

bool Database::createRelationRow(const std::string & name,
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

bool Database::removeRelationRow(const std::string & name,
                                 const std::string & id)
{
    std::string query = "DELETE FROM ";
    query += name;
    query += " WHERE source = ";
    query += id;

    return scheduleCommand(query);
}

bool Database::removeRelationRowByOther(const std::string & name,
                                        const std::string & other)
{
    std::string query = "DELETE FROM ";
    query += name;
    query += " WHERE target = ";
    query += other;

    // return runCommandQuery(query);
    return scheduleCommand(query);
}

bool Database::registerSimpleTable(const std::string & name,
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
            log(ERROR, "Illegal column type in database simple row");
        }
    }

    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "registerSimpleTable(): Database query error.");
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
    bool ret = runCommandQuery(createquery);
    if (ret) {
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

bool Database::createSimpleRow(const std::string & name,
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

    // return runCommandQuery(query);
    return scheduleCommand(query);
}

bool Database::updateSimpleRow(const std::string & name,
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

    // return runCommandQuery(query);
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
    return runCommandQuery("CREATE SEQUENCE entity_ent_id_seq") ? 0 : -1;
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
        debug(std::cout << "Table exists" << std::endl << std::flush;);
        return 0;
    }
    std::string query = "CREATE TABLE entities (id integer UNIQUE PRIMARY KEY, "
                        "loc integer REFERENCES entities (id), "
                        "type text, seq integer";
    std::map<std::string, int>::const_iterator I = chunks.begin();
    std::map<std::string, int>::const_iterator Iend = chunks.end();
    for (; I != Iend; ++I) {
        query += String::compose(", %1 bytea", I->first);
    }
    query += ")";
    std::cout << query;
    return runCommandQuery(query) ? 0 : -1;
}

int Database::insertEntity(const std::string & id,
                           const std::string & loc,
                           const std::string & type,
                           int seq)
{
    std::string query = String::compose("INSERT INTO entities VALUES "
                                        "(%1, %2, '%3', %4)",
                                        id, loc, type, seq);
    return scheduleCommand(query);
}

int Database::updateEntity(const std::string & id, int seq)
{
    std::string query = String::compose("UPDATE entities SET seq = %1 "
                                        "WHERE id = '%2'", seq, id);
    return scheduleCommand(query);
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
        debug(std::cout << "Table exists" << std::endl << std::flush;);
        return 0;
    }
    std::string query = "CREATE TABLE properties ("
                        "id integer REFERENCES entities (id), "
                        "name varchar(32), "
                        "value text)";
    std::cout << query;
    return runCommandQuery(query) ? 0 : -1;
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
            query += String::compose("(%1, '%2', '%3')", id, I->first, I->second);
            first = 0;
        } else {
            query += String::compose(", (%1, '%2', '%3')", id, I->first, I->second);
        }
    }
    return scheduleCommand(query);
}

int Database::updateProperties(const std::string & id,
                               const KeyValues & tuples)
{
    KeyValues::const_iterator I = tuples.begin();
    KeyValues::const_iterator Iend = tuples.end();
    for (; I != Iend; ++I) {
        std::string query = String::compose("UPATE properties value = '%3' where id=%1 and name='%2'", id, I->first, I->second);
        scheduleCommand(query);
    }
    return 0;
}

#if 0
bool Database::registerEntityTable(const std::string & classname,
                                   const MapType & row,
                                   const std::string & parent)
// TODO
// row probably needs to be richer to provide a more detailed, and possibly
// ordered description of each the columns required.
{
    if (m_connection == 0) {
        log(CRITICAL, "Database connection is down. This is okay during tests");
        return false;
    }

    if (entityTables.find(classname) != entityTables.end()) {
        log(ERROR, String::compose("Attempt to register entity table \"%1\" "
                                   "which is already registered.", classname));
        return false;
    }
    if (!parent.empty()) {
        if (entityTables.empty()) {
            log(ERROR, "Registering non-root entity table when no root registered.");
            debug(std::cerr << "Table for class " << classname
                            << " cannot be non-root."
                            << std::endl << std::flush;);
            return false;
        }
        if (entityTables.find(parent) == entityTables.end()) {
            log(ERROR, "Registering entity table with non existant parent.");
            debug(std::cerr << "Table for class " << classname
                            << " cannot have non-existant parent " << parent
                            << std::endl << std::flush;);
            return false;
        }
    } else if (!entityTables.empty()) {
        log(ERROR, "Attempt to create root entity class table when one already registered.");
        debug(std::cerr << "Table for class " << classname
                        << " cannot be root." << std::endl << std::flush;);
        return false;
    }
    // At this point we know the table request make sense.
    entityTables[classname] = parent;
    const std::string tablename = classname + "_ent";
    // Check whether the table exists
    std::string query = "SELECT * FROM ";
    std::string createquery = "CREATE TABLE ";
    query += tablename;
    createquery += tablename;
    if (!row.empty()) {
        query += " WHERE ";
    }
    createquery += " (";
    if (parent.empty()) {
        createquery += "id integer UNIQUE PRIMARY KEY, ";
    }
    MapType::const_iterator Iend = row.end();
    for (MapType::const_iterator I = row.begin(); I != Iend; ++I) {
        if (I != row.begin()) {
            query += " AND ";
            createquery += ", ";
        }
        const std::string & column = I->first;
        query += column;
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
            if (type.asInt() == 0xb001) {
                query += " = 't'";
                createquery += " boolean";
            } else {
                query += " = 1";
                createquery += " integer";
            }
        } else if (type.isFloat()) {
            query += " = 1.0";
            createquery += " float";
        } else {
            log(ERROR, "Illegal column type in database entity row");
        }
    }

    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "registerEntityTable(): Database query error.");
        reportError();
        return false;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug(std::cout << "Table does not yet exist"
                        << std::endl << std::flush;);
    } else {
        if (parent.empty()) {
            allTables.insert(tablename);
        }
        debug(std::cout << "Table exists" << std::endl << std::flush;);
        return true;
    }
    // create table
    createquery += ")";
    if (parent.empty()) {
        createquery += " WITHOUT OIDS";
    } else {
        createquery += " INHERITS (";
        createquery += parent;
        createquery += "_ent)";
    }
    debug(std::cout << "CREATE QUERY: " << createquery
                    << std::endl << std::flush;);

    bool ret = runCommandQuery(createquery);
    if (ret && parent.empty()) {
        allTables.insert(tablename);
    }
    return ret;
}

bool Database::createEntityRow(const std::string & classname,
                               const std::string & id,
                               const std::string & columns,
                               const std::string & values)
{
    TableDict::const_iterator I = entityTables.find(classname);
    if (I == entityTables.end()) {
        log(ERROR, "Attempt to insert into entity table not registered.");
        return false;
    }
    std::string query = "INSERT INTO ";
    query += classname;
    query += "_ent ( id, ";
    query += columns;
    query += " ) VALUES ( ";
    query += id;
    query += ", ";
    query += values;
    query += ")";
    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);

    // return runCommandQuery(query);
    return scheduleCommand(query);
}

bool Database::updateEntityRow(const std::string & classname,
                               const std::string & id,
                               const std::string & columns)
{
    if (columns.empty()) {
        log(WARNING, "Update query passed to database with no columns.");
        return false;
    }
    TableDict::const_iterator I = entityTables.find(classname);
    if (I == entityTables.end()) {
        log(ERROR, "Attempt to update entity table not registered.");
        return false;
    }
    std::string query = "UPDATE ";
    query += classname;
    query += "_ent SET ";
    query += columns;
    query += " WHERE id='";
    query += id;
    query += "'";
    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);

    return scheduleCommand(query);
}

bool Database::removeEntityRow(const std::string & classname,
                               const std::string & id)
{
    TableDict::const_iterator I = entityTables.find(classname);
    if (I == entityTables.end()) {
        log(ERROR, "Attempt to remove from entity table not registered.");
        return false;
    }
    std::string query = "DELETE FROM ";
    query += classname;
    query += "_ent WHERE id='";
    query += id;
    query += "'";
    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);

    // return runCommandQuery(query);
    return scheduleCommand(query);
}

const DatabaseResult Database::selectEntityRow(const std::string & id,
                                               const std::string & classname)
{
    std::string table = (classname == "" ? "entity" : classname);
    TableDict::const_iterator I = entityTables.find(classname);
    if (I == entityTables.end()) {
        log(ERROR, String::compose("Attempt to select from entity table \"%1\" "
                                   "which is not registered.", classname));
        return DatabaseResult(0);
    }
    std::string query = "SELECT * FROM ";
    query += table;
    query += "_ent WHERE id='";
    query += id;
    query += "'";

    debug(std::cout << "Selecting on id = " << id << " ... " << std::flush;);

    return runSimpleSelectQuery(query);
}

const DatabaseResult Database::selectOnlyByLoc(const std::string & loc,
                                               const std::string & classname)
{
    TableDict::const_iterator I = entityTables.find(classname);
    if (I == entityTables.end()) {
        log(ERROR, String::compose("Attempt to select from entity table \"%1\" "
                                   "which is not registered.", classname));
        return DatabaseResult(0);
    }

    std::string query = "SELECT * FROM ONLY ";
    query += classname;
    query += "_ent WHERE loc";
    if (loc.empty()) {
        query += " is null";
    } else {
        query += "=";
        query += loc;
    }


    return runSimpleSelectQuery(query);
}

const DatabaseResult Database::selectClassByLoc(const std::string & loc)
{
    std::string query = "SELECT id, class FROM entity_ent WHERE loc";
    if (loc.empty()) {
        query += " is null";
    } else {
        query += "=";
        query += loc;
    }

    debug(std::cout << "Selecting on loc = " << loc << " ... " << std::flush;);

    return runSimpleSelectQuery(query);
}
#endif // 0

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
    // FIXME This is a foreign key to an inherited table.
    // Implement referential integrity once PostgreSQL works.
    createquery += " (id integer NOT NULL";

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
    bool ret = runCommandQuery(createquery);
    if (!ret) {
        return false;
    }
    indexquery += ")";
    debug(std::cout << "INDEX QUERY: " << indexquery
                    << std::endl << std::flush;);
    ret = runCommandQuery(indexquery);
    if (!ret) {
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

bool Database::createArrayRow(const std::string & name,
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

bool Database::updateArrayRow(const std::string & name,
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

bool Database::removeArrayRow(const std::string & name,
                              const std::string & id,
                              const std::vector<int> & key)
{
    /// Not sure we need this one yet, so lets no bother for now ;)
    return false;
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

bool Database::launchNewQuery()
{
    assert(m_connection != 0);

    if (m_queryInProgress) {
        log(ERROR, "Launching new query when query is in progress");
        return false;
    }
    if (pendingQueries.empty()) {
        debug(std::cout << "No queries to launch" << std::endl << std::flush;);
        return false;
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
        return false;
    } else {
        m_queryInProgress = true;
        PQflush(m_connection);
        return true;
    }
}

bool Database::scheduleCommand(const std::string & query)
{
    pendingQueries.push_back(std::make_pair(query, PGRES_COMMAND_OK));
    if (!m_queryInProgress) {
        debug(std::cout << "Query: " << query << " launched"
                        << std::endl << std::flush;);
        return launchNewQuery();
    } else {
        debug(std::cout << "Query: " << query << " scheduled"
                        << std::endl << std::flush;);
        return true;
    }
}

bool Database::clearPendingQuery()
{
    if (!m_queryInProgress) {
        return true;
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
        return false;
    }
}

bool Database::runMaintainance(int command)
{
    // FIXME VACUUM and REINDEX tables from a common store
    if ((command & MAINTAIN_REINDEX) == MAINTAIN_REINDEX) {
        std::string query("REINDEX TABLE ");
        TableSet::const_iterator Iend = allTables.end();
        for (TableSet::const_iterator I = allTables.begin(); I != Iend; ++I) {
            debug(std::cout << (query + *I) << std::endl << std::flush;);
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
            debug(std::cout << (query + *I) << std::endl << std::flush;);
            scheduleCommand(query + *I);
        }
    }
    return true;
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
