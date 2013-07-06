// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#include "common/type_utils_impl.h"

void idListasObject(const IdList & l, Atlas::Message::ListType & ol)
{
    ol.clear();
    IdList::const_iterator Iend = l.end();
    for (IdList::const_iterator I = l.begin(); I != Iend; ++I) {
        ol.push_back(*I);
    }
}

int idListFromAtlas(const Atlas::Message::ListType & l, IdList & ol)
{
    ol.clear();
    Atlas::Message::ListType::const_iterator Iend = l.end();
    Atlas::Message::ListType::const_iterator I = l.begin();
    for (; I != Iend; ++I) {
        if (!I->isString()) {
            return -1;
        }
        ol.push_back(I->asString());
    }
    return 0;
}
