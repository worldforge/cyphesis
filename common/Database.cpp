// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "Database.h"

#include "log.h"
#include "debug.h"
#include "globals.h"
#include "stringstream.h"

#include <Atlas/Message/Encoder.h>
#include <Atlas/Codecs/XML.h>

#include <varconf/Config.h>

#include <iostream>

using Atlas::Message::Object;

static const bool debug_flag = false;

Database * Database::m_instance = NULL;

Database::Database() : account_db("account"),
                       world_db("world"),
                       mind_db("mind"),
                       server_db("server"),
                       rule_db("rules"),
                       m_connection(NULL)
{
    
}

bool Database::tuplesOk()
{
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
    bool status = false;

    PGresult * res;
    while ((res = PQgetResult(m_connection)) != NULL) {
        if (PQresultStatus(res) == PGRES_COMMAND_OK) {
            status = true;
        }
        PQclear(res);
    };
    return status;
}

bool Database::initConnection(bool createDatabase)
{
    std::stringstream conninfos;
    if (global_conf->findItem("cyphesis", "dbserver")) {
        conninfos << "host=" << std::string(global_conf->getItem("cyphesis", "dbserver")) << " ";
    }

    std::string dbname = "cyphesis";
    if (global_conf->findItem("cyphesis", "dbname")) {
        dbname = std::string(global_conf->getItem("cyphesis", "dbname"));
    }
    conninfos << "dbname=" << dbname << " ";

    if (global_conf->findItem("cyphesis", "dbuser")) {
        conninfos << "user=" << std::string(global_conf->getItem("cyphesis", "dbuser")) << " ";
    }

    if (global_conf->findItem("cyphesis", "dbpasswd")) {
        conninfos << "password=" << std::string(global_conf->getItem("cyphesis", "dbpasswd")) << " ";
    }

    const std::string cinfo = conninfos.str();

    if (createDatabase) {
        // Currently not able to create the database
    }

    m_connection = PQconnectdb(cinfo.c_str());

    if ((m_connection == NULL) || (PQstatus(m_connection) != CONNECTION_OK)) {
        log(ERROR, "Database connection failed");
        return false;
    }

    return true;
}

bool Database::initAccount(bool createTables)
{
    int status = 0;
    status = PQsendQuery(m_connection, "SELECT * FROM account WHERE id = 'admin';");
    if (!status) {
        reportError();
        return false;
    }

    if (!tuplesOk()) {
        debug(std::cout << "Account table does not exist"
                        << std::endl << std::flush;);
        reportError();
        if (createTables) {
            debug(std::cout << "Account table does not exist"
                            << std::endl << std::flush;);
            status = PQsendQuery(m_connection, "CREATE TABLE account ( id varchar(80)  PRIMARY KEY, contents text );");
            if (!status) {
                reportError();
                return false;
            }
            if (!commandOk()) {
                log(ERROR, "Error creating account table in database");
                return false;
            }
        } else {
            log(ERROR, "Account table does not exist in database");
            return false;
        }
    }
    return true;
}

bool Database::initWorld(bool createTables)
{
    int status = 0;
    status = PQsendQuery(m_connection, "SELECT * FROM world WHERE id = 'test' AND contents = 'test';");
    if (!status) {
        reportError();
        return false;
    }
    
    if (!tuplesOk()) {
        debug(std::cout << "World table does not exist"
                        << std::endl << std::flush;);
        if (createTables) {
            status = PQsendQuery(m_connection, "CREATE TABLE world ( id varchar(80) PRIMARY KEY, contents text );");
            if (!status) {
                reportError();
                return false;
            }
            if (!commandOk()) {
                log(ERROR, "Error creating world table in database");
                reportError();
                return false;
            }
        } else {
            log(ERROR, "World table does not exist in database");
            return false;
        }
    }
    return true;
}

bool Database::initMind(bool createTables)
{
    int status = 0;
    status = PQsendQuery(m_connection, "SELECT * FROM mind WHERE id = 'test' AND contents = 'test';");
    if (!status) {
        reportError();
        return false;
    }
    
    if (!tuplesOk()) {
        debug(std::cout << "Mind table does not exist"
                        << std::endl << std::flush;);
        if (createTables) {
            status = PQsendQuery(m_connection, "CREATE TABLE mind ( id varchar(80) PRIMARY KEY, contents text );");
            if (!status) {
                reportError();
                return false;
            }
            if (!commandOk()) {
                log(ERROR, "Error creating mind table in database");
                reportError();
                return false;
            }
        } else {
            log(ERROR, "Mind table does not exist in database");
            return false;
        }
    }
    return true;
}

bool Database::initServer(bool createTables)
{
    int status = 0;
    status = PQsendQuery(m_connection, "SELECT * FROM server WHERE id = 'test' AND contents = 'test';");
    if (!status) {
        reportError();
        return false;
    }
    
    if (!tuplesOk()) {
        debug(std::cout << "Server table does not exist"
                        << std::endl << std::flush;);
        if (createTables) {
            status = PQsendQuery(m_connection, "CREATE TABLE server ( id varchar(80) PRIMARY KEY, contents text );");
            if (!status) {
                reportError();
                return false;
            }
            if (!commandOk()) {
                log(ERROR, "Error creating server table in database");
                reportError();
                return false;
            }
        } else {
            log(ERROR, "Server table does not exist in database");
            return false;
        }
    }
    return true;
}

bool Database::initRule(bool createTables)
{
    int status = 0;
    status = PQsendQuery(m_connection, "SELECT * FROM rules WHERE id = 'test' AND contents = 'test';");
    if (!status) {
        reportError();
        return false;
    }
    
    if (!tuplesOk()) {
        debug(std::cout << "Rule table does not exist"
                        << std::endl << std::flush;);
        if (createTables) {
            status = PQsendQuery(m_connection, "CREATE TABLE rules ( id varchar(80) PRIMARY KEY, contents text );");
            if (!status) {
                reportError();
                return false;
            }
            if (!commandOk()) {
                log(ERROR, "Error creating rules table in database");
                reportError();
                return false;
            }
        } else {
            log(ERROR, "Server table does not exist in database");
            return false;
        }
    }
    return true;
}

void Database::shutdownConnection()
{
    PQfinish(m_connection);
}

Database * Database::instance()
{
    if (m_instance == NULL) {
        m_instance = new Database();
    }
    return m_instance;
}

bool Database::decodeObject(const std::string & data,
                            Atlas::Message::Object::MapType &o)
{
    std::stringstream str(data, std::ios::in);

    Atlas::Codecs::XML codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

    // Clear the decoder
    m_d.get();

    codec.Poll();

    if (!m_d.check()) {
        log(WARNING, "Database entry does not appear to be decodable");
        return false;
    }
    
    o = m_d.get();
    return true;
}

bool Database::getObject(const std::string & table, const std::string & key,
                         Atlas::Message::Object::MapType & o)
{
    debug(std::cout << "Database::getObject() " << table << "." << key
                    << std::endl << std::flush;);
    std::string query = std::string("SELECT * FROM ") + table + " WHERE id = '" + key + "';";

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
    if ((PQntuples(res) < 1) || (PQnfields(res) < 2)) {
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

    bool ret = decodeObject(data, o);
    PQclear(res);

    while ((res = PQgetResult(m_connection)) != NULL) {
        PQclear(res);
        log(ERROR, "Extra database result to simple query.");
    };

    return ret;
}

bool Database::putObject(const std::string & table,
                         const std::string & key,
                         const Atlas::Message::Object::MapType & o)
{
    debug(std::cout << "Database::putObject() " << table << "." << key
                    << std::endl << std::flush;);
    std::stringstream str;

    Atlas::Codecs::XML codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

    codec.StreamBegin();
    enc.StreamMessage(o);
    codec.StreamEnd();

    debug(std::cout << "Encoded to: " << str.str().c_str() << " "
               << str.str().size() << std::endl << std::flush;);
    std::string query = std::string("INSERT INTO ") + table + " VALUES ('" + key + "', '" + str.str() + "');";
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        reportError();
        return false;
    }
    if (!commandOk()) {
        debug(std::cerr << "Failed to insert item " << key << " into " << table
                        << " table" << std::endl << std::flush;);
        return false;
    }
    return true;
}

bool Database::updateObject(const std::string & table,
                            const std::string & key,
                            const Atlas::Message::Object::MapType & o)
{
    debug(std::cout << "Database::updateObject() " << table << "." << key
                    << std::endl << std::flush;);
    std::stringstream str;

    Atlas::Codecs::XML codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

    codec.StreamBegin();
    enc.StreamMessage(o);
    codec.StreamEnd();

    std::string query = std::string("UPDATE ") + table + " SET contents = '" +
                        str.str() + "' WHERE id='" + key + "';";
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        reportError();
        return false;
    }
    if (!commandOk()) {
        debug(std::cerr << "Failed to update item " << key << " into " << table
                        << " table" << std::endl << std::flush;);
        return false;
    }
    return true;
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

bool Database::getTable(const std::string & table, Object::MapType &o)
{
    std::string query = std::string("SELECT * FROM ") + table + ";";

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
    if ((results < 1) || (PQnfields(res) < 2)) {
        debug(std::cout << "No entries in " << table
                        << " table" << std::endl << std::flush;);
        PQclear(res);
        while ((res = PQgetResult(m_connection)) != NULL) {
            PQclear(res);
        }
        return false;
    }
    const char * data = PQgetvalue(res, 0, 1);
    Object::MapType t;
    for(int i = 0; i < results; i++) {
        const char * key = PQgetvalue(res, i, 0);
        const char * data = PQgetvalue(res, i, 1);
        debug(std::cout << "Got record " << key << " from database, value "
                   << data << std::endl << std::flush;);
    
        if (decodeObject(data, t)) {
            o[key] = t;
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
    std::string query = std::string("DELETE FROM ") + table + ";";
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        reportError();
        return false;
    }
    if (!commandOk()) {
        debug(std::cout << "Error clearing " << table
                        << " table" << std::endl << std::flush;);
        reportError();
        return false;
    }
    return true;
}

void Database::reportError()
{
    std::string msg = std::string("DATABASE ERROR: ") + PQerrorMessage(m_connection);
    log(ERROR, msg.c_str());
}
