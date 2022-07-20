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


#ifndef COMMON_DATABASESQLITE_H
#define COMMON_DATABASESQLITE_H

#include "sqlite3pp/sqlite3pp.h"
#include <thread>
#include <atomic>
#include <condition_variable>
#include "Database.h"


namespace sqlite3pp {
    class database;

    class query;
}

class DatabaseSQLite : public Database
{
    protected:

        std::deque<std::string> pendingQueries;
        std::unique_ptr<sqlite3pp::database> m_database;

        std::atomic<bool> m_active;
        std::condition_variable m_workerCondition;
        /**
         * Emitted when the queue is emptied, which is used when the method blockUntilAllQueriesComplete is called.
         */
        std::condition_variable m_queueEmptyCondition;
        std::mutex m_pendingQueriesMutex;
        std::thread m_workerThread;

        void poll_tasks();

    public:

        DatabaseSQLite();

        ~DatabaseSQLite() override;

        int initConnection() override;

        void shutdownConnection() override;

//        sqlite3* getConnection() const
//        {
//            return m_connection;
//        }


        size_t queryQueueSize() const override
        {
            return pendingQueries.size();
        }


        int getObject(const std::string& table,
                      const std::string& key,
                      Atlas::Message::MapType&) override;

        int encodeObject(const Atlas::Message::MapType&,
                         std::string&) override;

        void reportError(const char* errorMsg);

        int connect(const std::string& context, std::string& error_msg) override;


        DatabaseResult runSimpleSelectQuery(const std::string& query) override;

        int runCommandQuery(const std::string& query) override;


        int registerRelation(std::string& tablename,
                             const std::string& sourcetable,
                             const std::string& targettable,
                             RelationType kind) override;

        int registerThoughtsTable() override;

        int registerEntityTable(const std::map<std::string, int>& chunks) override;

        int registerPropertyTable() override;


        /// Creates a new unique id for the database.
        /// Note that this method will access the database, so it's a fairly expensive method.
        long newId() override;

        int registerEntityIdGenerator() override;


        int registerSimpleTable(const std::string& name,
                                const Atlas::Message::MapType& row) override;

        int scheduleCommand(const std::string& query) override;

        int runMaintainance();

        int launchNewQuery() override
        {
            return 0;
        }

        int clearPendingQuery() override
        {
            return 0;
        }

        void blockUntilAllQueriesComplete() override;


};

/// \brief Class to encapsulate a result from the database.
///
/// This allows the result to be used in the upper layers in a database
/// independant way.
struct DatabaseResultWorkerSqlite : public DatabaseResult::DatabaseResultWorker
{

    std::unique_ptr<sqlite3pp::query> m_res;

    explicit DatabaseResultWorkerSqlite(std::unique_ptr<sqlite3pp::query>&& r) : m_res(std::move(r))
    {}

    ~DatabaseResultWorkerSqlite() override = default;

    struct const_iterator_worker_sqlite : public DatabaseResult::const_iterator_worker
    {
        const DatabaseResultWorkerSqlite& m_dr;
        sqlite3pp::query::query_iterator m_iterator;

        const_iterator_worker_sqlite(const DatabaseResultWorkerSqlite& dr, sqlite3pp::query::query_iterator iterator)
            : m_dr(dr),
              m_iterator(iterator)
        {
        }

        const char* column(int column) const override;

        const char* column(const char* column) const override;

        const_iterator_worker& operator++() override;

        bool operator==(const const_iterator_worker& other) const override;
    };


    int size() const override;

    int columns() const override;

    bool error() const override;

    DatabaseResult::const_iterator begin() const override;

    DatabaseResult::const_iterator end() const override;

};

#endif // COMMON_DATABASESQLITE_H
