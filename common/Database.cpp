// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001-2003 Alistair Riddoch

#include "Database.h"

#include "log.h"
#include "debug.h"
#include "globals.h"
#include "stringstream.h"

#include <Atlas/Message/Encoder.h>
#include <Atlas/Codecs/XML.h>

#include <varconf/Config.h>

#include <iostream>

#include <cassert>

using Atlas::Message::Element;

typedef Atlas::Codecs::XML Serialiser;

static const bool debug_flag = false;

Database * Database::m_instance = NULL;

Database::Database() : m_rule_db("rules"),
                       m_queryInProgress(false),
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
        } else {
            reportError();
        }
        PQclear(res);
    };
    return status;
}

bool Database::initConnection(bool createDatabase)
{
    std::stringstream conninfos;
    if (global_conf->findItem("cyphesis", "dbserver")) {
        std::string db_server(global_conf->getItem("cyphesis", "dbserver"));
        if (db_server.empty()) {
            log(WARNING, "Empty database hostname specified in config file. Using none.");
        } else {
            conninfos << "host=" << " ";
        }
    }

    std::string dbname = "cyphesis";
    if (global_conf->findItem("cyphesis", "dbname")) {
        dbname = std::string(global_conf->getItem("cyphesis", "dbname"));
    }
    conninfos << "dbname=" << dbname << " ";

    if (global_conf->findItem("cyphesis", "dbuser")) {
        std::string db_user(global_conf->getItem("cyphesis", "dbuser"));
        if (db_user.empty()) {
            log(WARNING, "Empty username specified in config file. Using current user.");
        } else {
            conninfos << "user=" << db_user << " ";
        }
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

bool Database::initRule(bool createTables)
{
    int status = 0;
    clearPendingQuery();
    status = PQsendQuery(m_connection, "SELECT * FROM rules WHERE id = 'test' AND contents = 'test';");
    if (!status) {
        reportError();
        return false;
    }

    if (!tuplesOk()) {
        debug(std::cout << "Rule table does not exist"
                        << std::endl << std::flush;);
        if (createTables) {
            status = PQsendQuery(m_connection, "CREATE TABLE rules ( id varchar(80) PRIMARY KEY, ruleset varchar(32), contents text ) WITHOUT OIDS;");
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
                            Atlas::Message::Element::MapType &o)
{
    if (data.empty()) {
        return true;
    }

    std::stringstream str(data, std::ios::in);

    Serialiser codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

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

bool Database::encodeObject(const Atlas::Message::Element::MapType & o,
                            std::string & data)
{
    std::stringstream str;

    Serialiser codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

    codec.streamBegin();
    enc.streamMessage(o);
    codec.streamEnd();

    data = str.str();
    return true;
}

bool Database::getObject(const std::string & table, const std::string & key,
                         Atlas::Message::Element::MapType & o)
{
    debug(std::cout << "Database::getObject() " << table << "." << key
                    << std::endl << std::flush;);
    std::string query = std::string("SELECT * FROM ") + table + " WHERE id = '" + key + "';";

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
                         const Atlas::Message::Element::MapType & o,
                         const StringVector & c)
{
    debug(std::cout << "Database::putObject() " << table << "." << key
                    << std::endl << std::flush;);
    std::stringstream str;

    Serialiser codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

    codec.streamBegin();
    enc.streamMessage(o);
    codec.streamEnd();

    debug(std::cout << "Encoded to: " << str.str().c_str() << " "
               << str.str().size() << std::endl << std::flush;);
    std::string query = std::string("INSERT INTO ") + table + " VALUES ('" + key;
    for (StringVector::const_iterator I = c.begin(); I != c.end(); ++I) {
        query += "', '";
        query += *I;
    }
    query += "', '";
    query += str.str();
    query +=  "');";
    return scheduleCommand(query);
}

bool Database::updateObject(const std::string & table,
                            const std::string & key,
                            const Atlas::Message::Element::MapType & o)
{
    debug(std::cout << "Database::updateObject() " << table << "." << key
                    << std::endl << std::flush;);
    std::stringstream str;

    Serialiser codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

    codec.streamBegin();
    enc.streamMessage(o);
    codec.streamEnd();

    std::string query = std::string("UPDATE ") + table + " SET contents = '" +
                        str.str() + "' WHERE id='" + key + "';";
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

bool Database::getTable(const std::string & table, Element::MapType &o)
{
    std::string query = std::string("SELECT * FROM ") + table + ";";

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
    if ((results < 1) || (PQnfields(res) < 2)) {
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

    Element::MapType t;
    for(int i = 0; i < results; i++) {
        const char * key = PQgetvalue(res, i, id_column);
        const char * data = PQgetvalue(res, i, contents_column);
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
    return scheduleCommand(query);
}

void Database::reportError()
{
    std::string msg = std::string("DATABASE ERROR: ") +
                      PQerrorMessage(m_connection);
    log(ERROR, msg.c_str());
}

const DatabaseResult Database::runSimpleSelectQuery(const std::string & query)
{
    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "Database query error.");
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
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "Database query error.");
        reportError();
        return false;
    }
    if (!commandOk()) {
        log(ERROR, "Error running command query row.");
        log(NOTICE, query.c_str());
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
    tablename = sourcetable + "_" + targettable;

    std::string query = "SELECT * FROM ";
    query += tablename;
    query += " WHERE source = 0 AND target = 0;";

    std::string createquery = "CREATE TABLE ";
    createquery += tablename;
    if ((kind == OneToOne) || (kind == ManyToOne)) {
        createquery += " (source integer UNIQUE REFERENCES ";
    } else {
        createquery += " (source integer REFERENCES ";
    }
    createquery += sourcetable;
#if 0
    // FIXME Referential integrity not supported on inherited tables.
    if ((kind == OneToOne) || (kind == OneToMany)) {
        createquery += " (id), target integer UNIQUE REFERENCES ";
    } else {
        createquery += " (id), target integer REFERENCES ";
    }
    createquery += targettable;
    createquery += " (id));";
#else
    if ((kind == OneToOne) || (kind == OneToMany)) {
        createquery += " (id), target integer UNIQUE) WITHOUT OIDS;";
    } else {
        createquery += " (id), target integer) WITHOUT OIDS;";
    }
#endif

    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    clearPendingQuery();
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
    if ((kind == ManyToOne) || (kind == OneToOne)) {
        return true;
    } else {
        std::string indexQuery = "CREATE INDEX ";
        indexQuery += tablename;
        indexQuery += "_source_idx ON ";
        indexQuery += tablename;
        indexQuery += " (source);";
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
    query += ";";

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
    query += ");";

    // return runCommandQuery(query);
    return scheduleCommand(query);
}

bool Database::removeRelationRow(const std::string & name,
                                 const std::string & id)
{
    std::string query = "DELETE FROM ";
    query += name;
    query += " WHERE id=";
    query += id;
    query += ";";

    // return runCommandQuery(query);
    return scheduleCommand(query);
}

bool Database::removeRelationRowByOther(const std::string & name,
                                        const std::string & other)
{
    std::string query = "DELETE FROM ";
    query += name;
    query += " WHERE ";
    query += name;
    query += "=";
    query += other;
    query += ";";

    // return runCommandQuery(query);
    return scheduleCommand(query);
}

bool Database::registerSimpleTable(const std::string & name,
                                   const Atlas::Message::Element::MapType & row)
{
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
    Atlas::Message::Element::MapType::const_iterator I = row.begin();
    for(; I != row.end(); ++I) {
        query += " AND ";
        createquery += ", ";
        const std::string & column = I->first;
        query += column;
        createquery += column;
        const Atlas::Message::Element & type = I->second;
        if (type.isString()) {
            query += " LIKE 'foo'";
            int size = type.asString().size();
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
            log(ERROR, "Illegal column type in database entity row");
        }
    }
    query += ";";

    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    clearPendingQuery();
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
        allTables.insert(name);
        return true;
    }

    createquery += ") WITHOUT OIDS;";
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
    query += ";";

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
    query += ";";

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
    query += ");";

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
    query += "';";

    // return runCommandQuery(query);
    return scheduleCommand(query);
}

bool Database::registerEntityIdGenerator()
{
    clearPendingQuery();
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
    return runCommandQuery("CREATE SEQUENCE entity_ent_id_seq;");
}

bool Database::getEntityId(std::string & id)
{
    clearPendingQuery();
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
        return false;
    }
    const char * cid = PQgetvalue(res, 0, 0);
    id = cid;
    PQclear(res);
    while ((res = PQgetResult(m_connection)) != NULL) {
        PQclear(res);
        log(ERROR, "Extra database result to simple query.");
    };
    return true;
}

bool Database::registerEntityTable(const std::string & classname,
                                   const Atlas::Message::Element::MapType & row,
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
    Atlas::Message::Element::MapType::const_iterator I = row.begin();
    for(; I != row.end(); ++I) {
        if (I != row.begin()) {
            query += " AND ";
            createquery += ", ";
        }
        const std::string & column = I->first;
        query += column;
        createquery += column;
        const Atlas::Message::Element & type = I->second;
        if (type.isString()) {
            query += " LIKE 'foo'";
            int size = type.asString().size();
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
    query += ";";

    debug(std::cout << "QUERY: " << query << std::endl << std::flush;);
    clearPendingQuery();
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
        if (parent.empty()) {
            allTables.insert(tablename);
        }
        debug(std::cout << "Table exists" << std::endl << std::flush;);
        return true;
    }
    // create table
    createquery += ")";
    if (parent.empty()) {
        createquery += " WITHOUT OIDS;";
    } else {
        createquery += " INHERITS (";
        createquery += parent;
        createquery += "_ent);";
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
    query += ");";
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
    query += "';";
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
    query += "';";
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
        log(ERROR, "Attempt to select from entity table not registered.");
        return DatabaseResult(0);
    }
    std::string query = "SELECT * FROM ";
    query += table;
    query += "_ent WHERE id='";
    query += id;
    query += "';";

    debug(std::cout << "Selecting on id = " << id << " ... " << std::flush;);

    return runSimpleSelectQuery(query);
}

const DatabaseResult Database::selectOnlyByLoc(const std::string & loc,
                                               const std::string & classname)
{
    TableDict::const_iterator I = entityTables.find(classname);
    if (I == entityTables.end()) {
        log(ERROR, "Attempt to select from entity table not registered.");
        return DatabaseResult(0);
    }

    std::string query = "SELECT * FROM ONLY ";
    query += classname;
    query += "_ent WHERE loc";
    if (loc.empty()) {
        query += " is null;";
    } else {
        query += "=";
        query += loc;
        query += ";";
    }


    return runSimpleSelectQuery(query);
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

    debug(std::cout << "Selecting on loc = " << loc << " ... " << std::flush;);

    return runSimpleSelectQuery(query);
}

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
        TableSet::const_iterator I = allTables.begin();
        for(; I != allTables.end(); ++I) {
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
        TableSet::const_iterator I = allTables.begin();
        for(; I != allTables.end(); ++I) {
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
