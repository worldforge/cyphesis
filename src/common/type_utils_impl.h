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


#ifndef COMMON_TYPE_UTILS_IMPL_H
#define COMMON_TYPE_UTILS_IMPL_H

#include "common/type_utils.h"

// This will work on any type which implements toAtlas()
template<typename List_T>
void objectListAsMessage(const List_T & l, Atlas::Message::ListType & ol)
{
    ol.clear();
    for (auto& entry : l) {
        ol.push_back(entry.toAtlas());
    }
}

template<typename T, typename List_T>
inline int objectListFromMessage(const Atlas::Message::ListType & l,
                                 List_T & ol)
{
    ol.clear();

    for (auto& entry : l) {
        try {
            ol.push_back(T(entry.asList()));
        }
        catch (const Atlas::Message::WrongTypeException&) {
            return -1;
        }
    }
    return 0;
}

#endif // COMMON_TYPE_UTILS_IMPL_H
