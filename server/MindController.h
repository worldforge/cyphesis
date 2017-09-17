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

#ifndef MINDCONTROLLER_H_
#define MINDCONTROLLER_H_

#include "Account.h"

/**
 * \brief An account type used for controlling minds.
 */
class MindController : public Account
{
    public:
        MindController(Connection * conn, const std::string & username,
                const std::string & passwd,
                const std::string & id, long intId);
        virtual ~MindController();

    protected:

        /// \brief Sets an attribute on the account instance itself.
        void setAttribute(const Atlas::Objects::Root& arg);

};

#endif /* MINDCONTROLLER_H_ */
