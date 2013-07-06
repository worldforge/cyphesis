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


#ifndef COMMON_TYPE_UTILS_H
#define COMMON_TYPE_UTILS_H

#include <Atlas/Message/Element.h>

typedef std::vector<std::string> IdList;

void idListasObject(const IdList & l, Atlas::Message::ListType & ol);

int idListFromAtlas(const Atlas::Message::ListType & l, IdList & ol);

// This could probably be made into a template, as it will work
// on any type which implements asMessage()
template<typename List_T>
void objectListAsMessage(const List_T & l, Atlas::Message::ListType & ol);

template<typename T, typename List_T>
int objectListFromMessage(const Atlas::Message::ListType & l, List_T & ol);

#endif // COMMON_TYPE_UTILS_H
