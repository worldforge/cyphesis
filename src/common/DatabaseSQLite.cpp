#include <memory>

#include <memory>

// Cyphesis Online RPG Server and AI Engine
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


#include "DatabaseSQLite.h"

#include "id.h"
#include "log.h"
#include "debug.h"
#include "globals.h"
#include "compose.hpp"
#include "const.h"
#include "Remotery.h"

#include <Atlas/Codecs/Packed.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using String::compose;

using namespace sqlite3pp;

typedef Atlas::Codecs::Packed Serialiser;

static const bool debug_flag = false;


DatabaseSQLite::DatabaseSQLite() :
    Database(),
    m_active(true),
    m_workerThread([&]() { this->poll_tasks(); })
{
}

DatabaseSQLite::~DatabaseSQLite()
{
    m_active = false;
    m_workerCondition.notify_all();
    m_workerThread.join();
    if (!pendingQueries.empty()) {
        log(ERROR, compose("Database delete with %1 queries pending",
                           pendingQueries.size()));

    }
}


void DatabaseSQLite::poll_tasks()
{
#ifdef __APPLE__
    pthread_setname_np("SQLite task processor");
#else
    pthread_setname_np(pthread_self(), "SQLite task processor");
#endif
    while (true) {
        std::unique_lock<std::mutex> lock(m_pendingQueriesMutex);
        if (!pendingQueries.empty()) {
            rmt_ScopedCPUSample(DatabaseSQLite_poll_task, 0)
            auto command = std::move(pendingQueries.front());
            lock.unlock();
            runCommandQuery(command);
            lock.lock();
            pendingQueries.pop_front();
        } else {
            if (m_active) {
                m_queueEmptyCondition.notify_all();
                m_workerCondition.wait(lock);
            } else {
                return;
            }
        }
    }
}

void DatabaseSQLite::blockUntilAllQueriesComplete() {
    std::unique_lock<std::mutex> lock(m_pendingQueriesMutex);
    if (!pendingQueries.empty()) {
        m_queueEmptyCondition.wait(lock);
    }
}


int DatabaseSQLite::connect(const std::string& context, std::string& error_msg)
{
    return 0;
}

int DatabaseSQLite::initConnection()
{
    boost::filesystem::path db_path = boost::filesystem::path(var_directory) / "lib" / "cyphesis" / "cyphesis.db3";

    try {
        boost::filesystem::create_directories(db_path.parent_path());
        m_database = std::make_unique<database>(db_path.c_str());
    } catch (const std::runtime_error& e) {
        log(WARNING, "Error when opening SQLite database.");
        return -1;
    }

    return 0;
}

void DatabaseSQLite::shutdownConnection()
{
    m_database.reset(nullptr);
}

int DatabaseSQLite::encodeObject(const MapType& o,
                                 std::string& data)
{
    std::stringstream str;

    Serialiser codec(str, str, m_d);
    Atlas::Message::Encoder enc(codec);

    codec.streamBegin();
    enc.streamMessageElement(o);
    codec.streamEnd();

    data = str.str();

    //Escape ' characters by doubling them.
    boost::algorithm::replace_all(data, "'", "''");

    return 0;
}

int DatabaseSQLite::getObject(const std::string& table,
                              const std::string& key,
                              MapType& o)
{
    assert(m_database);

    debug_print("Database::getObject() " << table << "." << key)
    std::string query = std::string("SELECT * FROM ") + table + " WHERE id = '" + key + "'";

    try {
        sqlite3pp::query qry(*m_database, query.c_str());

        auto firstRowIterator = qry.begin();
        if (firstRowIterator == qry.end()) {
            debug_print("No entry for " << key << " in " << table
                            << " table")
            return -1;
        }

        auto data = (*firstRowIterator).get<const char*>(1);

        debug_print("Got record " << key << " from database, value " << data)

        int ret = decodeMessage(data, o);

        return ret;
    } catch (const database_error& e) {
        log(WARNING, String::compose("Error when running database query: %1", e.what()));
        return -1;
    }
}

void DatabaseSQLite::reportError(const char* errorMsg)
{
    std::string msg = std::string("DATABASE: ") + errorMsg;
    msg = msg.substr(0, msg.size() - 1);
    log(ERROR, msg);
}

DatabaseResult DatabaseSQLite::runSimpleSelectQuery(const std::string& query)
{
    assert(m_database);

    debug_print("QUERY: " << query)
    auto query_instance = std::make_unique<sqlite3pp::query>(*m_database, query.c_str());

    return DatabaseResult(std::make_unique<DatabaseResultWorkerSqlite>(std::move(query_instance)));
}

int DatabaseSQLite::runCommandQuery(const std::string& query)
{
    assert(m_database);

    try {
        command cmd(*m_database, query.c_str());
        cmd.execute();
    } catch (const database_error& e) {
        log(ERROR, String::compose("runCommandQuery('%1'): Database query error.", query));
        reportError(e.what());
        return -1;
    }
    return 0;
}

int DatabaseSQLite::registerRelation(std::string& tablename,
                                     const std::string& sourcetable,
                                     const std::string& targettable,
                                     RelationType kind)
{
    assert(m_database);

    tablename = sourcetable + "_" + targettable;


    std::string query = "CREATE TABLE IF NOT EXISTS ";
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
    query += " (id) ON DELETE CASCADE )";

    debug_print("CREATE QUERY: " << query)
    if (runCommandQuery(query) != 0) {
        return -1;
    }

    return 0;
}

int DatabaseSQLite::registerSimpleTable(const std::string& name,
                                        const MapType& row)
{
    assert(m_database);

    if (row.empty()) {
        log(ERROR, "Attempt to create empty database table");
    }
    std::string createquery = "CREATE TABLE IF NOT EXISTS ";
    createquery += name;
    createquery += " (id integer UNIQUE PRIMARY KEY";
    auto Iend = row.end();
    for (auto I = row.begin(); I != Iend; ++I) {
        createquery += ", ";
        const std::string& column = I->first;
        createquery += column;
        const Element& type = I->second;
        if (type.isString()) {
            std::size_t size = type.String().size();
            if (size == 0) {
                createquery += " text";
            } else {
                createquery += " varchar(";
                createquery += std::to_string(size);
                createquery += ")";
            }
        } else if (type.isInt()) {
            createquery += " integer";
        } else if (type.isFloat()) {
            createquery += " float";
        } else {
            log(ERROR, "Illegal column type in database simple row");
        }
    }
    createquery += ")";

    debug_print("CREATE QUERY: " << createquery)
    int ret = runCommandQuery(createquery);
    return ret;
}

static long idGenerator = 0;

int DatabaseSQLite::registerEntityIdGenerator()
{
    assert(m_database);

    query qryEntities(*m_database, "SELECT MAX(id) FROM entities;");
    auto entitiesI = qryEntities.begin();
    if (entitiesI != qryEntities.end()) {
        idGenerator = std::max(idGenerator, static_cast<long>((*entitiesI).get<int>(0)));
    }
    query qryAccounts(*m_database, "SELECT MAX(id) FROM accounts;");
    auto accountsI = qryAccounts.begin();
    if (accountsI != qryAccounts.end()) {
        idGenerator = std::max(idGenerator, static_cast<long>((*accountsI).get<int>(0)));
    }

    return 0;
}


long DatabaseSQLite::newId()
{
    long new_id = ++idGenerator;
    return new_id;
}

int DatabaseSQLite::registerEntityTable(const std::map<std::string, int>& chunks)
{
    assert(m_database);

    std::string query = compose("CREATE TABLE IF NOT EXISTS entities ("
                                "id integer UNIQUE PRIMARY KEY, "
                                "loc integer, "
                                "type char(%1), "
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
    query = compose("INSERT INTO entities VALUES (%1, null, 'world', 0, '')",
                    consts::rootWorldIntId);
    if (runCommandQuery(query) != 0) {
        return -1;
    }
    return 0;
}

int DatabaseSQLite::registerPropertyTable()
{
    assert(m_database);

    std::string query = compose("CREATE TABLE IF NOT EXISTS properties ("
                                "id integer REFERENCES entities "
                                "ON DELETE CASCADE, "
                                "name varchar(%1), "
                                "value text)", consts::id_len);
    if (runCommandQuery(query) != 0) {
        return -1;
    }
    query = "CREATE INDEX IF NOT EXISTS property_names on properties (name)";
    if (runCommandQuery(query) != 0) {
        return -1;
    }
    return 0;
}


int DatabaseSQLite::registerThoughtsTable()
{
    assert(m_database);

    std::string query = "CREATE TABLE IF NOT EXISTS thoughts ("
                        "id integer REFERENCES entities "
                        "ON DELETE CASCADE, "
                        "thought text)";
    if (runCommandQuery(query) != 0) {
        return -1;
    }
    return 0;
}

// General functions for handling queries at the low level.

int DatabaseSQLite::scheduleCommand(const std::string& query)
{
    {
        std::unique_lock<std::mutex> lock(m_pendingQueriesMutex);
        pendingQueries.push_back(query);
    }
    m_workerCondition.notify_all();
    return 0;
}


int DatabaseSQLite::runMaintainance()
{
    scheduleCommand("VACUUM");

    return 0;
}


const char* DatabaseResultWorkerSqlite::const_iterator_worker_sqlite::column(int column) const
{
    return (*m_iterator).get<const char*>(column);
}

const char* DatabaseResultWorkerSqlite::const_iterator_worker_sqlite::column(const char* column) const
{
    for (int i = 0; i < m_dr.m_res->column_count(); ++i) {
        if (std::strcmp(m_dr.m_res->column_name(i), column) == 0) {
            return (*m_iterator).get<const char*>(i);
        }
    }
    return nullptr;
}

DatabaseResult::const_iterator_worker& DatabaseResultWorkerSqlite::const_iterator_worker_sqlite::operator++()
{
    m_iterator.operator++();


    return *this;
}

bool DatabaseResultWorkerSqlite::const_iterator_worker_sqlite::operator==(const const_iterator_worker& other) const
{
    return (m_iterator == ((const const_iterator_worker_sqlite&) other).m_iterator);
}

int DatabaseResultWorkerSqlite::size() const
{
    int size = 0;
    auto beginI = m_res->begin();
    auto endI = m_res->end();
    while (beginI != endI) {
        size++;
        beginI.operator++();
    }
    return size;
}

int DatabaseResultWorkerSqlite::columns() const
{
    return m_res->column_count();
}

bool DatabaseResultWorkerSqlite::error() const
{ return (m_res == nullptr); }

DatabaseResult::const_iterator DatabaseResultWorkerSqlite::begin() const
{
    return DatabaseResult::const_iterator(std::make_unique<const_iterator_worker_sqlite>(*this, m_res->begin()), *this);
}

DatabaseResult::const_iterator DatabaseResultWorkerSqlite::end() const
{
    return DatabaseResult::const_iterator(std::make_unique<const_iterator_worker_sqlite>(*this, m_res->end()), *this);
}
