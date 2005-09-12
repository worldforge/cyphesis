// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

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
