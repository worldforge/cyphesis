// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Info.h>

#include "ClientAccount.h"

using Atlas::Message::Object;

ClientAccount::ClientAccount(const string & name, ClientConnection & con) :
    connection(con)
{
    fullid = name;
}

oplist ClientAccount::Operation(const RootOperation & op)
{
    cout << "Account got miscellaneous op" << endl << flush;
    return oplist();
}

oplist ClientAccount::Operation(const Info & op)
{
    cout << "Account got info op" << endl << flush;
    try {
        Object ent = op.GetArgs().front();
        const string & entid = ent.AsMap()["id"].AsString();
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
