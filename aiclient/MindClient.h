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

#include "BaseClient.h"

/**
 * Handles on mind of an entity on the server.
 */
class MindClient: public BaseClient
{
    public:
        MindClient();
        virtual ~MindClient();

        virtual void idle();

        Atlas::Objects::Root login(const std::string& username, const std::string& password);

        void takePossession(const std::string& possessEntityId, const std::string& possessKey);

    protected:
        virtual void operation(const Operation & op, OpVector & res);
        void InfoOperation(const Operation & op, OpVector & res);

};

#endif /* MINDCLIENT_H_ */
