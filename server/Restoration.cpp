// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Restoration.h"

Restoration::Restoration(ServerRouting & svr) : server(svr)
{
}

void Restoration::read()
{
    getRootFromTable();
    handleRootAttrs();
    getListChildren();
    getChildrenCorrectTable();
    reconstruct();
    recurse();
}
