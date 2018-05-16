// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#ifndef SERVER_PERSISTENCE_H
#define SERVER_PERSISTENCE_H

#include "common/Singleton.h"
#include <Atlas/Objects/ObjectsFwd.h>

#include <sigc++/signal.h>

#include <string>
#include <map>

class Account;
class Database;
class LocatedEntity;

typedef std::map<long, LocatedEntity *> EntityDict;

/// \brief Class for managing the required database tables for persisting
/// in-game entities and server accounts
class Persistence : public Singleton<Persistence> {
  private:

    std::string m_characterRelation;

  public:
    explicit Persistence(Database& database);

    /// \brief Data about a character being tied to an account.
    ///
    /// This is used when signaling that an entity has been tied to an account.
    /// Since this operation is dependent on the entity having been persisted to the database
    /// we can't do it here; instead we rely on the StorageManager to do it.
    /// However, the interface between these two components isn't the smoothest, so we'll
    /// have to do it through signals.
    struct AddCharacterData {
        /// \brief The id of the character entity.
        std::string entity_id;

        /// \brief The id of the account.
        std::string account_id;
    };

    Database & m_db;

    int init();
    void shutdown();

    bool findAccount(const std::string &);
    Account * getAccount(const std::string &);
    void putAccount(const Account &);
    void registerCharacters(Account &, const EntityDict & worldObjects);
    void addCharacter(const Account &, const LocatedEntity &);
    void delCharacter(const std::string &);

    /// \brief Gets the name of the DB relation tying Accounts to Entities.
    const std::string& getCharacterAccountRelationName() const;

    /// \brief Emitted when a character has been tied to an account.
    ///
    /// It's expected that the StorageManager should handle this.
    /// Any listener should return "true" if the action was properly handled.
    sigc::signal<bool, const AddCharacterData&> characterAdded;

    /// \brief Emitted when a character has been deleted, and the
    /// tie to any account should be removed.
    ///
    /// It's expected that the StorageManager should handle this.
    /// Any listener should return "true" if the action was properly handled.
    sigc::signal<bool, const std::string&> characterDeleted;
};

#endif // SERVER_PERSISTENCE_H
