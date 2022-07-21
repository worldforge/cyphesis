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

#include <Atlas/Codecs/Packed.h>

#include <varconf/config.h>

#include <cstring>

#include <cassert>
#include <boost/algorithm/string.hpp>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using String::compose;

typedef Atlas::Codecs::Packed Serialiser;

static const bool debug_flag = false;

Database::Database() : m_queryInProgress(false)
{
}

Database::~Database() = default;


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

int Database::decodeMessage(const std::string & data,
                            MapType &o)
{
    if (data.empty()) {
        return 0;
    }

    std::stringstream str(data, std::ios::in);

    Serialiser codec(str, str, m_d);

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

DatabaseResult Database::selectRelation(const std::string & name,
                                              const std::string & id)
{
    std::string query = "SELECT target FROM ";
    query += name;
    query += " WHERE source = ";
    query += id;

    debug_print("Selecting on id = " << id << " ... ");

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

int Database::removeRelationRowByOther(const std::string & name,
                                       const std::string & other)
{
    std::string query = "DELETE FROM ";
    query += name;
    query += " WHERE target = ";
    query += other;

    return scheduleCommand(query);
}

DatabaseResult Database::selectSimpleRowBy(const std::string & name,
                                                 const std::string & column,
                                                 const std::string & value)
{
    std::string query = "SELECT * FROM ";
    query += name;
    query += " WHERE ";
    query += column;
    query += " = ";
    query += value;

    debug_print("Selecting on " << column << " = " << value
                    << " ... ");

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


DatabaseResult Database::selectEntities(const std::string & loc)
{
    std::string query = compose("SELECT id, type, seq, location FROM entities"
                                " WHERE loc = %1", loc);

    debug_print("Selecting on loc = " << loc << " ... ");

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

int Database::insertProperties(const std::string & id,
                               const KeyValues & tuples)
{
    int first = 1;
    std::string query("INSERT INTO properties VALUES ");
    for (auto& tuple : tuples) {
        if (first) {
            query += compose("(%1, '%2', '%3')", id, tuple.first, tuple.second);
            first = 0;
        } else {
            query += compose(", (%1, '%2', '%3')", id, tuple.first, tuple.second);
        }
    }
    return scheduleCommand(query);
}

DatabaseResult Database::selectProperties(const std::string & id)
{
    std::string query = compose("SELECT name, value FROM properties"
                                " WHERE id = %1", id);

    debug_print("Selecting on id = " << id << " ... ");

    return runSimpleSelectQuery(query);
}

int Database::updateProperties(const std::string & id,
                               const KeyValues & tuples)
{
    for (const auto& entry : tuples) {
        std::string query = compose("UPDATE properties SET value = '%3' WHERE"
                                    " id=%1 AND name='%2'",
                                    id, entry.first, entry.second);
        scheduleCommand(query);
    }
    return 0;
}


DatabaseResult Database::selectThoughts(const std::string & loc)
{
    std::string query = compose("SELECT thought FROM thoughts"
                                " WHERE id = %1", loc);

    debug_print("Selecting on id = " << loc << " ... ");

    return runSimpleSelectQuery(query);
}

int Database::replaceThoughts(const std::string & id,
                         const std::vector<std::string>& thoughts)
{

    std::string deleteQuery = compose("DELETE FROM thoughts WHERE id=%1", id);
    scheduleCommand(deleteQuery);

    for (auto& thought : thoughts) {
        std::string insertQuery = compose("INSERT INTO thoughts (id, thought)"
                                          " VALUES (%1, '%2')", id, boost::replace_all_copy(thought, "'", "''"));
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
    createquery += " (id integer REFERENCES entities NOT nullptr";

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
        createquery += " integer NOT nullptr";

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

    debug_print("QUERY: " << query)
    clearPendingQuery();
    int status = PQsendQuery(m_connection, query.c_str());
    if (!status) {
        log(ERROR, "registerArrayTable(): Database query error.");
        reportError();
        return false;
    }
    if (!tuplesOk()) {
        debug(reportError(););
        debug_print("Table does not yet exist"
                       )
    } else {
        debug_print("Table exists")
        allTables.insert(name);
        return true;
    }

    createquery += ") WITHOUT OIDS";
    debug_print("CREATE QUERY: " << createquery);
    int ret = runCommandQuery(createquery);
    if (ret != 0) {
        return false;
    }
    indexquery += ")";
    debug_print("INDEX QUERY: " << indexquery);
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

    debug_print("ARRAY QUERY: " << query)

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
    debug_print("QUery: " << qstr)
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
    debug_print("QUery: " << qstr)
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


DatabaseResult::DatabaseResult(DatabaseResult&& dr) noexcept
: m_worker(std::move(dr.m_worker))
{

}

DatabaseResult::const_iterator::const_iterator(std::unique_ptr<DatabaseResult::const_iterator_worker>&& worker, const DatabaseResult::DatabaseResultWorker& dr)
: m_worker(std::move(worker)),
m_dr(dr)
{

}

DatabaseResult::const_iterator::const_iterator(const_iterator&& ci) noexcept
: m_worker(std::move(ci.m_worker)),
  m_dr(ci.m_dr)
{}


DatabaseResult::const_iterator& DatabaseResult::const_iterator::operator++()
{
    m_worker->operator++();

    return *this;
}
