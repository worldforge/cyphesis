//
// Copyright (C) 2009 Alistair Riddoch
// Copyright (C) 2012 Erik Ogenvik
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

#ifndef EMBER_ENTITYEXPORTER_H
#define EMBER_ENTITYEXPORTER_H

#include "EntityExporterBase.h"
#include "common/ClientTask.h"

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>
#include <unordered_map>

/**
 * @author Erik Ogenvik
 *
 * @brief Cyphesis specific subclass of EntityExporter.
 *
 */
class EntityExporter: public ClientTask, public EntityExporterBase
{
    public:
        /**
         * @brief Ctor.
         */
        explicit EntityExporter(const std::string & accountId,
                const std::string & avatarId);

        /**
         * @brief Dtor.
         */
        ~EntityExporter() override = default;

    protected:

        /**
         * Keeps track of the current OpVector in which operations to be
         * sent can be placed.
         *
         * Note that this setup is a little unintuitive. This is because the
         * EntityExporterBase class is written for Ember which has a slightly
         * different way of dispatching ops.
         * The way we need to do it is to act on the setup() and operation()
         * methods and while in those methods set this field to point to the
         * current OpVector.
         *
         */
        OpVector* mCurrentRes;

        Atlas::Objects::Root m_server_info;

        std::unordered_map<long int, CallbackFunction> mCallbacks;

        void setup(const std::string & arg, OpVector & ret) override;

        void operation(const Operation & op, OpVector & res) override;

        /**
         * @brief Create a new unique serial number for operations.
         * @return A new serial number.
         */
        long int newSerialNumber() override;

        /**
         * @brief Send an object to the server.
         * @param op An object which is to be sent.
         */
        void send(const Atlas::Objects::Operation::RootOperation& op) override;

        /**
         * @brief Send an object to the server and await a response.
         * @param op An object which is to be sent.
         * @param callback A callback, called when a response is received.
         */
        void sendAndAwaitResponse(
                const Atlas::Objects::Operation::RootOperation& op,
                CallbackFunction& callback) override;

        /**
         * @brief Creates a multi line formatter.
         * @param s A stream.
         * @param b A bridge.
         * @return A formatter instance. Ownership is transferred.
         */
        Atlas::Formatter* createMultiLineFormatter(std::iostream& s,
                Atlas::Bridge& b) override;

        /**
         * @brief Gets the current time as a unit timestamp string.
         * @return A time string.
         */
        virtual std::string getCurrentTime();

        /**
         * @brief Fills the supplied map with server meta data.
         * @param serverMap An empty map.
         */
        void fillWithServerData(Atlas::Message::MapType& serverMap) override;

        /**
         * Called when server info is received.
         * @param op
         */
        void operationGetServerInfo(const Atlas::Objects::Operation::RootOperation& op);

};

#endif
