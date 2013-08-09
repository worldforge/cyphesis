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

#ifndef MINDCLIENT_H_
#define MINDCLIENT_H_

#include "common/SystemTime.h"
#include "common/OperationRouter.h"

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Root.h>

#include <string>

class MindFactory;
class BaseMind;
class ClientConnection;

/**
 * Handles on mind of an entity on the server.
 */
class MindClient
{
    public:
        MindClient(MindFactory& mindFactory);
        virtual ~MindClient();

        void takePossession(OpVector& res, ClientConnection& connection,
                std::string& accountId, const std::string& possessEntityId,
                const std::string& possessKey);

        void operation(const Operation & op, OpVector & res);

        void idle(OpVector & res);

        /**
         * Returns true if the entity to which the mind belongs to has been destroyed.
         * @return True if the entiyt to which the mind belongs to has been destroyed.
         */
        bool isMindDestroyed() const;

    protected:
        void operationToMind(const Operation & op, OpVector & res);

        void createMind(const Operation & op, OpVector & res);
        MindFactory& m_mindFactory;

        BaseMind* m_mind;
        std::string m_entityId;

        SystemTime m_systemTime;
        time_t m_nextTick;

};

#endif /* MINDCLIENT_H_ */
