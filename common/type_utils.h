// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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

// $Id$

#ifndef COMMON_TYPE_UTILS_H
#define COMMON_TYPE_UTILS_H

#include <Atlas/Message/Element.h>

typedef std::vector<std::string> IdList;

inline void idListasObject(const IdList & l, Atlas::Message::ListType & ol)
{
    ol.clear();
    IdList::const_iterator Iend = l.end();
    for (IdList::const_iterator I = l.begin(); I != Iend; ++I) {
        ol.push_back(*I);
    }
}

inline void idListFromAtlas(const Atlas::Message::ListType & l, IdList & ol)
{
    ol.clear();
    Atlas::Message::ListType::const_iterator Iend = l.end();
    for (Atlas::Message::ListType::const_iterator I = l.begin(); I != Iend; ++I) {
        ol.push_back(I->asString());
    }
}

// This could probably be made into a template, as it will work
// on any type which implements asMessage()
template<typename List_T>
void objectListAsMessage(const List_T & l, Atlas::Message::ListType & ol)
{
    ol.clear();
    typename List_T::const_iterator Iend = l.end();
    for (typename List_T::const_iterator I = l.begin(); I != Iend; ++I) {
        ol.push_back(I->toAtlas());
    }
}

template<typename T, typename List_T>
inline int objectListFromMessage(const Atlas::Message::ListType & l,
                                 List_T & ol)
{
    ol.clear();
    
    Atlas::Message::ListType::const_iterator Iend = l.end();
    for (Atlas::Message::ListType::const_iterator I = l.begin(); I != Iend; ++I) {
        try {
            ol.push_back(T(I->asList()));
        }
        catch (Atlas::Message::WrongTypeException) {
            return -1;
        }
    }
    return 0;
}

#endif // COMMON_TYPE_UTILS_H
