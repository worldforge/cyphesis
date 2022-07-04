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


#ifndef SERVER_ACCOUNT_H
#define SERVER_ACCOUNT_H

#include "ConnectableRouter.h"

#include "modules/Ref.h"
#include "common/AutoCloseConnection.h"

#include <vector>

namespace Atlas {
    namespace Message {
        class Element;

        typedef std::vector<Element> ListType;
    }
}

class Connection;

class LocatedEntity;

class ExternalMind;

/// \brief This is the base class for storing information about uses who
/// can use this server.
///
/// The majority of functionality relating to user accounts is encapsulated
/// here. Sub-classes control privilege levels by implementing
/// characterError().
class Account : public ConnectableRouter
{
    protected:
        /// \brief The network connection currently subscribed to this object
        Connection* m_connection;

        /// \brief A store of Character entities belonging to this account
        std::map<long, Ref<LocatedEntity>> m_charactersDict;
        /// \brief The username of this account
        std::string m_username;
        /// \brief The password used to authenticate this account
        std::string m_password;

        struct MindEntry
        {
            std::unique_ptr<ExternalMind> mind;
            AutoCloseConnection destroyedConnection;
        };

        /**
         * A map of external characters->minds.
         */
        std::map<long, MindEntry> m_minds;

        void characterDestroyed(long);

        virtual void processExternalOperation(const Operation& op, OpVector& res);

        virtual std::unique_ptr<ExternalMind> createMind(const Ref<LocatedEntity>& entity) const;

        void removeMindFromEntity(ExternalMind* mind);

    public:
        /// \brief Connect and add a character to this account
        int connectCharacter(const Ref<LocatedEntity>& entity, OpVector& res);

        Account(Connection* conn, std::string username,
                std::string passwd,
                RouterId id);

        ~Account() override;

        const std::string& username() const
        {
            return m_username;
        }

        const std::string& password() const
        {
            return m_password;
        }

        /// \brief Get a string representation of the type of account
        virtual const char* getType() const;

        /// \brief Store this account in the database
        void store() const;

        /// \brief Returns true if the account should be stored.
        virtual bool isPersisted() const;

        void addToMessage(Atlas::Message::MapType&) const override;

        void addToEntity(const Atlas::Objects::Entity::RootEntity&) const override;

        void externalOperation(const Operation& op, Link&) override;

        //Operations sent to this instance will be sent on to m_connection, if such exists.
        void operation(const Operation&, OpVector&) override;

        virtual void LogoutOperation(const Operation&, OpVector&);

        virtual void CreateOperation(const Operation&, OpVector&);

        virtual void SetOperation(const Operation&, OpVector&);

        virtual void ImaginaryOperation(const Operation&, OpVector&);

        virtual void TalkOperation(const Operation&, OpVector&);

        virtual void LookOperation(const Operation&, OpVector&);

        virtual void GetOperation(const Operation&, OpVector&);

        virtual void OtherOperation(const Operation&, OpVector&);

        virtual void PossessOperation(const Operation&, OpVector&);

        void addCharacter(const Ref<LocatedEntity>&);

        /**
         * Sends an update to the client, after properties has changed.
         *
         * Currently sends the complete account state.
         */
        void sendUpdateToClient();

        void setConnection(Connection* connection) override;

        Connection* getConnection() const override;

        friend class Accounttest;

        friend class Accountintegration;

};

#endif // SERVER_ACCOUNT_H
