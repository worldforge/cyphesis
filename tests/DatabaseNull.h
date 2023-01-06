/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef TESTS_DATABASE_NULL_H
#define TESTS_DATABASE_NULL_H

#include "common/Database.h"
#include <functional>
#include <memory>


struct const_iterator_worker_null : public DatabaseResult::const_iterator_worker
{

    const char* column(int column) const override
    { return ""; }

    const char* column(const char* column) const override
    { return ""; }

    DatabaseResult::const_iterator_worker& operator++() override
    { return *this; }

    bool operator==(const const_iterator_worker& other) const override
    { return true; }
};

class DatabaseNullResultWorker : public DatabaseResult::DatabaseResultWorker
{
    public:
        ~DatabaseNullResultWorker() override = default;

        int size() const override
        {
            return 0;
        }

        int columns() const override
        {
            return 0;
        }

        bool error() const override
        {
            return false;
        }

        DatabaseResult::const_iterator begin() const override
        {
            return DatabaseResult::const_iterator(std::unique_ptr<DatabaseResult::const_iterator_worker>(new const_iterator_worker_null), *this);
        }

        DatabaseResult::const_iterator end() const override
        {
            return DatabaseResult::const_iterator(std::unique_ptr<DatabaseResult::const_iterator_worker>(new const_iterator_worker_null), *this);
        }

};

class DatabaseNull : public Database
{

    public:

        long id = 1;
        std::function<long()> idGeneratorFn = [&]() -> long {
            return id++;
        };

        int initConnection() override
        {
            return 0;
        }

        void shutdownConnection() override
        {

        }

        size_t queryQueueSize() const override
        {
            return 0;
        }


        int getObject(const std::string& table,
                      const std::string& key,
                      Atlas::Message::MapType&) override
        {
            return 0;
        }

        int encodeObject(const Atlas::Message::MapType&,
                         std::string&) override
        {
            return 0;
        }

        void reportError(const char* errorMsg)
        {}

        int connect(const std::string& context, std::string& error_msg) override
        {
            return 0;
        }


        DatabaseResult runSimpleSelectQuery(const std::string& query) override
        {
            return DatabaseResult(std::make_unique<DatabaseNullResultWorker>());
        }

        int runCommandQuery(const std::string& query) override
        {
            return 0;
        }


        int registerRelation(std::string& tablename,
                             const std::string& sourcetable,
                             const std::string& targettable,
                             RelationType kind) override
        {
            return 0;
        }

        int registerThoughtsTable() override
        {
            return 0;
        }

        int registerEntityTable(const std::map<std::string, int>& chunks) override
        {
            return 0;
        }

        int registerPropertyTable() override
        {
            return 0;
        }


        /// Creates a new unique id for the database.
        /// Note that this method will access the database, so it's a fairly expensive method.
        long newId() override
        {
            if (idGeneratorFn) {
                return idGeneratorFn();
            }
            return 0;
        }

        int registerEntityIdGenerator() override
        {
            return 0;
        }


        int registerSimpleTable(const std::string& name,
                                const Atlas::Message::MapType& row) override
        {
            return 0;
        }

        int scheduleCommand(const std::string& query) override
        {
            return 0;
        }

        int runMaintainance()
        {
            return 0;
        }

        int launchNewQuery() override
        { return 0; }

        int clearPendingQuery() override
        { return 0; }

        void blockUntilAllQueriesComplete() override {}

};

#endif //TESTS_DATABASE_NULL_H
