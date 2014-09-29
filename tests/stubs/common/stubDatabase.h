/*
 Copyright (C) 2014 Erik Ogenvik

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
#ifndef STUBDATABASE_H_
#define STUBDATABASE_H_


Database * Database::m_instance = NULL;

int Database::initConnection()
{
    return 0;
}

Database::Database() : m_rule_db("rules"),
                       m_queryInProgress(false),
                       m_connection(NULL)
{
}

Database::~Database()
{
}

void Database::shutdownConnection()
{
}

int Database::registerRelation(std::string & tablename,
                               const std::string & sourcetable,
                               const std::string & targettable,
                               RelationType kind)
{
    return 0;
}

const DatabaseResult Database::selectSimpleRowBy(const std::string & name,
                                                 const std::string & column,
                                                 const std::string & value)
{
    return DatabaseResult(0);
}

Database * Database::instance()
{
    if (m_instance == NULL) {
        m_instance = new Database();
    }
    return m_instance;
}

int Database::createInstanceDatabase()
{
    return 0;
}

int Database::registerEntityIdGenerator()
{
    return 0;
}

int Database::registerEntityTable(const std::map<std::string, int> & chunks)
{
    return 0;
}

int Database::registerPropertyTable()
{
    return 0;
}

int Database::initRule(bool createTables)
{
    return 0;
}

int Database::registerSimpleTable(const std::string & name,
                                  const MapType & row)
{
    return 0;
}

int Database::createSimpleRow(const std::string & name,
                              const std::string & id,
                              const std::string & columns,
                              const std::string & values)
{
    return 0;
}

const DatabaseResult Database::selectRelation(const std::string & name,
                                              const std::string & id)
{
    return DatabaseResult(0);
}

int Database::createRelationRow(const std::string & name,
                                const std::string & id,
                                const std::string & other)
{
    return 0;
}

int Database::removeRelationRow(const std::string & name,
                                const std::string & id)
{
    return 0;
}

int Database::removeRelationRowByOther(const std::string & name,
                                       const std::string & other)
{
    return 0;
}

bool Database::hasKey(const std::string & table, const std::string & key)
{
    return false;
}

int Database::putObject(const std::string & table,
                        const std::string & key,
                        const MapType & o,
                        const StringVector & c)
{
    return 0;
}

int Database::getTable(const std::string & table,
                       std::map<std::string, Atlas::Objects::Root> & contents)
{
    return 0;
}

int Database::clearPendingQuery()
{
    return 0;
}

int Database::updateObject(const std::string & table,
                           const std::string & key,
                           const MapType & o)
{
    return 0;
}

int Database::clearTable(const std::string & table)
{
    return 0;
}

long Database::newId(std::string & id)
{
    return 0;
}

void Database::cleanup()
{
    if (m_instance != 0) {
        delete m_instance;
    }

    m_instance = 0;
}

int Database::registerThoughtsTable()
{
    return 0;
}


const DatabaseResult Database::selectProperties(const std::string & id)
{
    return DatabaseResult(0);
}

const DatabaseResult Database::selectEntities(const std::string & loc)
{
    return DatabaseResult(0);
}

int Database::encodeObject(const MapType & o,
                           std::string & data)
{
    return 0;
}

int Database::decodeMessage(const std::string & data,
                            MapType &o)
{
    return 0;
}

int Database::insertEntity(const std::string & id,
                           const std::string & loc,
                           const std::string & type,
                           int seq,
                           const std::string & value)
{
    return 0;
}

int Database::updateEntity(const std::string & id,
                           int seq,
                           const std::string & location_data,
                           const std::string & location)
{
    return 0;
}

int Database::updateEntityWithoutLoc(const std::string & id,
                           int seq,
                           const std::string & location_data)
{
    return 0;
}

int Database::dropEntity(long id)
{
    return 0;
}

int Database::insertProperties(const std::string & id,
                               const KeyValues & tuples)
{
    return 0;
}

int Database::updateProperties(const std::string & id,
                               const KeyValues & tuples)
{
    return 0;
}

const DatabaseResult Database::selectThoughts(const std::string & loc)
{
    return DatabaseResult(0);
}
int Database::replaceThoughts(const std::string & id,
                     const std::vector<std::string>& thoughts)
{
    return 0;
}

int Database::launchNewQuery()
{
    return 0;
}


#endif /* STUBDATABASE_H_ */
