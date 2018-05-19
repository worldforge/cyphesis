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

#ifndef CYPHESIS_DATABASENULL_H
#define CYPHESIS_DATABASENULL_H

#include "../common/Database.h"

class DatabaseNullResultWorker : public DatabaseResult::DatabaseResultWorker
{
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
            return DatabaseResult::const_iterator(nullptr, *this);
        }

        DatabaseResult::const_iterator end() const override
        {
            return DatabaseResult::const_iterator(nullptr, *this);
        }

        const char* field(int column) const override
        {
            return "";
        }

        const char* field(const char* column) const override
        {
            return "";
        }
};

class DatabaseNull : public Database
{

    public:
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
            return DatabaseResult(std::unique_ptr<DatabaseNullResultWorker>(new DatabaseNullResultWorker()));
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
        long newId(std::string& id) override
        {
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

};

#endif //CYPHESIS_DATABASENULL_H
