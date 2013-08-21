/*
 Copyright (C) 2013 Erik Ogenvik

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

#ifndef MINDINSPECTOR_H_
#define MINDINSPECTOR_H_

#include "common/OperationRouter.h"

#include <sigc++/signal.h>
#include <sigc++/trackable.h>

#include <unordered_map>
#include <string>

/**
 * \brief Queries minds for their thoughts.
 *
 * This is done by relaying operations through the World object.
 * When a query result is received the ThoughtsReceived signal is emitted.
 */
class MindInspector: public virtual sigc::trackable
{
    public:
        MindInspector();
        virtual ~MindInspector();

        /**
         * \brief Query an entity for its thoughts.
         * @param entityId The id of the entity.
         */
        void queryEntityForThoughts(const std::string& entityId);

        /**
         * \brief Emitted when thoughts for an entity have been received.
         */
        sigc::signal<void, const std::string&, const Operation&> ThoughtsReceived;

    protected:

        /**
         * \brief A maps of outstanding relays.
         *
         * The key represent the refno and the value the id of the entity.
         */
        std::unordered_map<long int, const std::string&> m_relaysToEntities;

        /**
         * \brief Serial number counter for outgoing ops.
         */
        long int m_serial;

        /**
         * Called when a relayed operation is received.
         * @param op The relayed op.
         * @param entityId The entity id from which the op was sent.
         */
        void relayResponseReceived(const Operation& op,
                const std::string& entityId);

};

#endif /* MINDINSPECTOR_H_ */
