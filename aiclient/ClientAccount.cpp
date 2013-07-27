// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
// Copyright (C) 2013 Erik Ogenvik
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


#include "ClientAccount.h"

#include <Atlas/Objects/Operation/Info.h>

ClientAccount::ClientAccount(const std::string & name, ClientConnection & con) :
    connection(con)
{
    setId(name);
}

oplist ClientAccount::OtherOperation(const RootOperation & op)
{
    cout << "Account got miscellaneous op" << endl << flush;
    return oplist();
}

oplist ClientAccount::InfoOperation(const Info & op)
{
    cout << "Account got info op" << endl << flush;
    try {
        Object ent = op.getArgs().front();
        const std::string & entid = ent.asMap()["id"].asString();
        // This idea is that this info operation contains details of a new
        // entity created in the server which belongs to this account.
        // See existing server code for how the mind objects is created.
        // We then store this mind in the connections dictionary
    }
    catch (...) {
        cerr << "WARNING: Malformed info op from server" << endl << flush;
    }

    return oplist();
}
