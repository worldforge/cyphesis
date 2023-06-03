// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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


#ifndef RULESETS_WORLD_H
#define RULESETS_WORLD_H

#include "Thing.h"

#include <sigc++/slot.h>
#include <unordered_map>



/// \brief This is the in-game entity class used to represent the world.
///
/// I added this because I was not happy with the way the old object model
/// used an out of game object of type WorldRouter to represent the world.
///
/// The World is special in that it ALWAYS exists. This in contrast to all other
/// kind of entities which may or may not exist. As such the World has extra
/// Relay functionality, as it's suitable to act as a relay for any Out-Of-Game
/// router which wants to interact with entities through operations (without
/// having to create an In-Game entity).
///
/// The World also always contain a Void domain. This means that any interim entity
/// can be created in the World and then moved into whatever regular entity is needed.
/// \ingroup EntityClasses
class World : public Thing {

    /// \brief Keeps track of relayed operations.
    struct Relay {
        /// \brief The entity to which the operation was relayed.
        std::string entityId;
        /// \brief A callback to call when a response is received.
        sigc::slot<void(const Operation&, const std::string&)> callback;
    };

public:
    explicit World();

    ~World() override;

    void LookOperation(const Operation &, OpVector &) override;
    void DeleteOperation(const Operation &, OpVector &) override;
    void MoveOperation(const Operation &, OpVector &) override;
    void RelayOperation(const Operation & op, OpVector & res) override;

    /// \brief Relays an operation to an in game entity.
    ///
    /// When a response is received, or if the timeout is exceeded,
    ///  the callback is called.
    ///
    /// \param entity The destination entity.
    /// \param op The operation to send.
    /// \param callback A callback which will be called when either a
    ///             response is received or a timeout is reached.
    void sendRelayToEntity(const LocatedEntity& entity, const Operation& op,
            sigc::slot<void(const Operation&, const std::string&)> callback);

  protected:

    /// \brief Keeps track of serial numbers for relayed ops.
    long int m_serialNumber;

    /**
     * \brief A store of registered outgoing relays for the world.
     *
     * Key is the serialno/refno of the Relay op.
     */
    std::unordered_map<long int, Relay> m_relays;

    /**
     * @brief Clears the world of all entities and properties.
     *
     * Once this method has been run the server should be reset back to
     * a "clean" state.
     * @param res
     */
    void clearWorld(OpVector & res);

};

#endif // RULESETS_WORLD_H
