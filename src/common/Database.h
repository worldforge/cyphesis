// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001-2004 Alistair Riddoch
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


#ifndef COMMON_DATABASE_H
#define COMMON_DATABASE_H

#include "Singleton.h"

#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Message/Element.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Factories.h>

#include <set>
#include <memory>

/// \brief Class to handle decoding Atlas encoded database records
class Decoder : public Atlas::Message::DecoderBase
{
    private:
        void messageArrived(Atlas::Message::MapType msg) override
        {
            m_check = true;
            m_msg = std::move(msg);
        }

        bool m_check;
        Atlas::Message::MapType m_msg;
    public:
        Decoder() : m_check(false)
        {
        }

        bool check() const
        {
            return m_check;
        }

        const Atlas::Message::MapType& get()
        {
            m_check = false;
            return m_msg;
        }
};
//
///// \brief Class to handle decoding Atlas encoded database records
//class ObjectDecoder : public Atlas::Objects::ObjectsDecoder
//{
//    private:
//        void objectArrived(const Atlas::Objects::Root& obj) override
//        {
//            m_check = true;
//            m_obj = obj;
//        }
//
//        bool m_check;
//        Atlas::Objects::Root m_obj;
//    public:
//        ObjectDecoder() : ObjectsDecoder(*Atlas::Objects::Factories::instance()), m_check(false)
//        {
//        }
//
//        bool check() const
//        {
//            return m_check;
//        }
//
//        const Atlas::Objects::Root& get()
//        {
//            m_check = false;
//            return m_obj;
//        }
//};

class DatabaseResult;

typedef std::set<std::string> TableSet;

/// \brief Class to provide interface to Database connection
///
/// Most SQL is generated from here, including queries for handling all
/// table creation, queries to simple non-inherited tables and more
class Database : public Singleton<Database>
{
    protected:

        bool m_queryInProgress;

        Decoder m_d;

    public:
        typedef enum
        {
            OneToMany, ManyToMany, ManyToOne, OneToOne
        } RelationType;

        typedef std::map<std::string, std::string> KeyValues;

        Database();

        ~Database() override;

        bool queryInProgress() const
        { return m_queryInProgress; }

        virtual size_t queryQueueSize() const = 0;

        int decodeMessage(const std::string& data,
                          Atlas::Message::MapType&);

        virtual int encodeObject(const Atlas::Message::MapType&,
                                 std::string&) = 0;

        virtual int getObject(const std::string& table,
                              const std::string& key,
                              Atlas::Message::MapType&) = 0;

        virtual int connect(const std::string& context, std::string& error_msg) = 0;

        virtual int initConnection() = 0;

        int createInstanceDatabase();

        virtual void shutdownConnection() = 0;

        virtual DatabaseResult runSimpleSelectQuery(const std::string& query) = 0;

        virtual int runCommandQuery(const std::string& query) = 0;

        // Interface for relations between tables.

        virtual int registerRelation(std::string& tablename,
                                     const std::string& sourcetable,
                                     const std::string& targettable,
                                     RelationType kind) = 0;

        DatabaseResult selectRelation(const std::string& name,
                                      const std::string& id);

        int createRelationRow(const std::string& name,
                              const std::string& id,
                              const std::string& other);

        int removeRelationRowByOther(const std::string& name,
                                     const std::string& other);

        // Interface for simple tables that mainly just store Atlasish data.

        virtual int registerSimpleTable(const std::string& name,
                                        const Atlas::Message::MapType& row) = 0;

        DatabaseResult selectSimpleRowBy(const std::string& name,
                                         const std::string& column,
                                         const std::string& value);

        int createSimpleRow(const std::string& name,
                            const std::string& id,
                            const std::string& columns,
                            const std::string& values);

        int updateSimpleRow(const std::string& name,
                            const std::string& key,
                            const std::string& value,
                            const std::string& columns);

        // Interface for the ID generation sequence.

        virtual int registerEntityIdGenerator() = 0;

        /// Creates a new unique id for the database.
        /// Note that this method will access the database, so it's a fairly expensive method.
        virtual long newId() = 0;

        // Interface for Entity and Property tables.

        virtual int registerEntityTable(const std::map<std::string, int>& chunks) = 0;

        int insertEntity(const std::string& id,
                         const std::string& loc,
                         const std::string& type,
                         int seq,
                         const std::string& value);

        int updateEntityWithoutLoc(const std::string& id,
                                   int seq,
                                   const std::string& location_data);

        int updateEntity(const std::string& id,
                         int seq,
                         const std::string& location_data,
                         const std::string& location_entity_id);

        DatabaseResult selectEntities(const std::string& loc);

        int dropEntity(long id);

        virtual int registerPropertyTable() = 0;

        int insertProperties(const std::string& id,
                             const KeyValues& tuples);

        DatabaseResult selectProperties(const std::string& loc);

        int updateProperties(const std::string& id,
                             const KeyValues& tuples);

        virtual int registerThoughtsTable() = 0;

        DatabaseResult selectThoughts(const std::string& loc);

        int replaceThoughts(const std::string& id,
                            const std::vector<std::string>& thoughts);

        // Interface for CommPSQLSocket, so it can give us feedback

        virtual int launchNewQuery() = 0;

        virtual int clearPendingQuery() = 0;

        /**
         * Blocks the current thread until all queries have completed.
         */
        virtual void blockUntilAllQueriesComplete() = 0;

        virtual int scheduleCommand(const std::string& query) = 0;


};


/// \brief Class to encapsulate a result from the database.
///
/// This allows the result to be used in the upper layers in a database
/// independant way.
class DatabaseResult
{
    public:
        struct DatabaseResultWorker;
    private:

        std::unique_ptr<DatabaseResultWorker> m_worker;


    public:
        DatabaseResult(DatabaseResult&& dr) noexcept;

        explicit DatabaseResult(std::unique_ptr<DatabaseResultWorker>&& worker)
            : m_worker(std::move(worker))
        {

        }

        struct const_iterator_worker
        {

            virtual ~const_iterator_worker() = default;

            virtual const char* column(int column) const = 0;

            virtual const char* column(const char* column) const = 0;

            virtual const_iterator_worker& operator++() = 0;

            virtual bool operator==(const const_iterator_worker& other) const = 0;
        };

        /// \brief Iterator for DatabaseResult
        ///
        /// Mimics STL iterator API
        class const_iterator
        {
            private:
                std::unique_ptr<const_iterator_worker> m_worker;
                const DatabaseResultWorker& m_dr;

            public:
                explicit const_iterator(std::unique_ptr<const_iterator_worker>&& worker, const DatabaseResultWorker& dr);

                const_iterator(const_iterator&& ci) noexcept;

                bool operator==(const const_iterator& other)
                {
                    return (*m_worker == *other.m_worker);
                }

                bool operator!=(const const_iterator& other)
                {
                    return !(*this == other);
                }

                const_iterator& operator++();

                const char* column(int column) const
                {
                    return m_worker->column(column);
                }

                const char* column(const char* column) const
                {
                    return m_worker->column(column);
                }

                friend class DatabaseResult;
        };

        struct DatabaseResultWorker
        {
            virtual ~DatabaseResultWorker() = default;

            virtual int size() const = 0;

            virtual int columns() const = 0;

            virtual bool error() const = 0;

            virtual const_iterator begin() const = 0;

            virtual const_iterator end() const = 0;

        };

        int size() const
        {
            return m_worker->size();
        }

        bool empty() const
        {
            return (size() == 0);
        }

        int columns() const
        {
            return m_worker->columns();
        }

        bool error() const
        {
            return m_worker->error();
        }

        const_iterator begin() const
        {
            return m_worker->begin();
        }

        const_iterator end() const
        {
            return m_worker->end();
        }

};

#endif // COMMON_DATABASE_H
