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
        while ((res = PQgetResult(m_connection)) != NULL);
        return false;
    }
    const char * data = PQgetvalue(res, 0, 1);
    debug(std::cout << "Got record " << key << " from database, value " << data
               << std::endl << std::flush;);

    bool ret = decodeObject(data, o);

    while (PQgetResult(m_connection) != NULL) {
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
        while ((res = PQgetResult(m_connection)) != NULL);
        return false;
    }
    // const char * data = PQgetvalue(res, 0, 1);
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

    while ((res = PQgetResult(m_connection)) != NULL) {
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
    std::string msg = std::string("DATABASE ERROR: ") +
                      PQerrorMessage(m_connection);
    log(ERROR, msg.c_str());
}

bool Database::registerEntityIdGenerator()
{
    int status = PQsendQuery(m_connection, "SELECT * FROM entity_ent_id_seq;");
    if (!status) {
        log(ERROR, "Database query error.");
        reportError();
        return false;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug(std::cout << "Sequence does not yet exist"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Sequence exists" << std::endl << std::flush;);
        return true;
    }
    status = PQsendQuery(m_connection, "CREATE SEQUENCE entity_ent_id_seq;");
    if (!status) {
        log(ERROR, "Database query error.");
        reportError();
        return false;
    }
    if (!commandOk()) {
        debug(reportError(););
        debug(std::cout << "Sequence creation failed"
                        << std::endl << std::flush;);
        return false;
    } else {
        debug(std::cout << "Sequence created" << std::endl << std::flush;);
        return true;
    }

}

std::string Database::getEntityId()
{
    int status = PQsendQuery(m_connection,
                             "SELECT nextval('entity_ent_id_seq');");
    if (!status) {
        log(ERROR, "Database query error.");
        reportError();
        return false;
    }
    PGresult * res;
    if ((res = PQgetResult(m_connection)) == NULL) {
        log(ERROR, "Error getting new ID.");
        reportError();
        return "";
    }
    while (PQgetResult(m_connection) != NULL) {
        log(ERROR, "Extra database result to simple query.");
    };
    const char * id = PQgetvalue(res, 0, 0);
    return id;
}

bool Database::registerEntityTable(const std::string & classname,
                                   const Atlas::Message::Object::MapType & row,
                                   const std::string & parent)
// TODO
// row probably needs to be richer to provide a more detailed, and possibly
// ordered description of each the columns required.
{
    if (entityTables.find(classname) != entityTables.end()) {
        log(ERROR, "Attempt to register entity table already registered.");
        debug(std::cerr << "Table for class " << classname
                        << " already registered." << std::endl << std::flush;);
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
    } else {
        if (!registerEntityIdGenerator()) {
            log(ERROR, "Faled to register Id generator in database.");
        }
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
    createquery += "(";
    if (parent.empty()) {
        createquery += "id integer UNIQUE PRIMARY KEY, ";
    }
    Atlas::Message::Object::MapType::const_iterator I = row.begin();
    for(; I != row.end(); ++I) {
        if (I != row.begin()) {
            query += " AND ";
            createquery += ", ";
        }
        const std::string & column = I->first;
        query += column;
        createquery += column;
        const Atlas::Message::Object & type = I->second;
        if (type.IsString()) {
            query += " LIKE 'foo'";
            int size = type.AsString().size();
            if (size == 0) {
                createquery += " text";
            } else {
                char buf[32];
                snprintf(buf, 32, "%d", size);
                createquery += " varchar(";
                createquery += buf;
                createquery += ")";
            }
        } else if (type.IsInt()) {
            query += " = 1";
            createquery += " integer";
        } else if (type.IsFloat()) {
            query += " = 1.0";
            createquery += " float";
        } else {
            log(ERROR, "Illegal column type in database entity row");
        }
    }
    query += ";";

    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "Database query error.");
        reportError();
        return false;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug(std::cout << "Table does not yet exist"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Table exists" << std::endl << std::flush;);
        return true;
    }
    // create table
    createquery += ")";
    if (!parent.empty()) {
        createquery += " INHERITS (";
        createquery += parent;
        createquery += "_ent)";
    }
    createquery += ";";
    debug(std::cout << "CREATE QUERY: " << createquery
                    << std::endl << std::flush;);
    status = PQsendQuery(m_connection, createquery.c_str());
    if (!status) {
        log(ERROR, "Database query error.");
        reportError();
        return false;
    }
    if (!commandOk()) {
        log(ERROR, "Error creating database table.");
        reportError();
        debug(std::cout << "Table create didn't work"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Table created" << std::endl << std::flush;);
        return true;
    }
    return true;
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
    query += " ) VALUES ( '";
    query += id;
    query += "', ";
    query += values;
    query += ");";
    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);

    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "Database query error.");
        reportError();
        return false;
    }
    if (!commandOk()) {
        log(ERROR, "Error creating entity row.");
        reportError();
        debug(std::cout << "Row create didn't work"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Query worked" << std::endl << std::flush;);
        return true;
    }
    return false;
}

bool Database::updateEntityRow(const std::string & classname,
                               const std::string & id,
                               const std::string & columns)
{
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
    query += "';";
    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);

    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "Database query error.");
        reportError();
        return false;
    }
    if (!commandOk()) {
        log(ERROR, "Error updating entity row.");
        std::cout << "QUERY: " << query << std::endl << std::flush;
        reportError();
        debug(std::cout << "Row update didn't work"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Query worked" << std::endl << std::flush;);
        return true;
    }
    return false;
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
    query += "';";
    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);

    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "Database query error.");
        reportError();
        return false;
    }
    if (!commandOk()) {
        log(ERROR, "Error removing entity row.");
        reportError();
        debug(std::cout << "Row remove didn't work"
                        << std::endl << std::flush;);
    } else {
        debug(std::cout << "Query worked" << std::endl << std::flush;);
        return true;
    }
    return false;
}

const DatabaseResult Database::selectEntityRow(const std::string & id,
                                               const std::string & classname)
{
    std::string table = (classname == "" ? "entity" : classname);
    TableDict::const_iterator I = entityTables.find(classname);
    if (I == entityTables.end()) {
        log(ERROR, "Attempt to select from entity table not registered.");
        return DatabaseResult(0);
    }
    std::string query = "SELECT * FROM ";
    query += table;
    query += "_ent WHERE id='";
    query += id;
    query += "';";
    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "Database query error.");
        reportError();
        return DatabaseResult(0);
    }

    PGresult * res;
    if ((res = PQgetResult(m_connection)) == NULL) {
        log(ERROR, "Error updating entity row.");
        reportError();
        debug(std::cout << "Row query didn't work"
                        << std::endl << std::flush;);
        return DatabaseResult(0);
    }
    while (PQgetResult(m_connection) != NULL) {
        log(ERROR, "Extra database result to simple query.");
    };
    return DatabaseResult(res);
}

const DatabaseResult Database::selectClassByLoc(const std::string & loc)
{
    std::string query = "SELECT id, class FROM entity_ent WHERE loc";
    if (loc.empty()) {
        query += " is null;";
    } else {
        query += "=";
        query += loc;
        query += ";";
    }

    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "Database query error.");
        reportError();
        return DatabaseResult(0);
    }

    PGresult * res;
    if ((res = PQgetResult(m_connection)) == 0) {
        log(ERROR, "Error selecting entity row.");
        reportError();
        std::cout << "QUERY: " << query << std::endl << std::flush;
        debug(std::cout << "Row query didn't work"
                        << std::endl << std::flush;);
        return DatabaseResult(0);
    }
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        log(ERROR, "Error selecting entity row.");
        reportError();
        std::cout << "QUERY: " << query << std::endl << std::flush;
        res = 0;
    }

    while (PQgetResult(m_connection) != 0) {
        log(ERROR, "Extra database result to simple query.");
    };
    return DatabaseResult(res);
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
