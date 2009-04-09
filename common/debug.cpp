// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

#include "debug.h"

#include <Atlas/Message/Element.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

static void output(const Element & item, int depth)
{
    switch (item.getType()) {
        case Element::TYPE_INT:
            std::cout << item.Int();
            break;
        case Element::TYPE_FLOAT:
            std::cout << item.Float();
            break;
        case Element::TYPE_STRING:
            std::cout << "\"" << item.String() << "\"";
            break;
        case Element::TYPE_LIST:
            {
                std::cout << "[ ";
                ListType::const_iterator I = item.List().begin();
                ListType::const_iterator Iend = item.List().end();
                for(; I != Iend; ++I) {
                    output(*I, depth + 1);
                    std::cout << " ";
                }
                std::cout << "]";
            }
            break;
        case Element::TYPE_MAP:
            {
                std::cout << "{" << std::endl << std::flush;
                MapType::const_iterator I = item.Map().begin();
                MapType::const_iterator Iend = item.Map().end();
                for(; I != Iend; ++I) {
                    std::cout << std::string((depth + 1) * 4, ' ') << I->first << ": ";
                    output(I->second, depth + 1);
                    std::cout << std::endl;
                }
                std::cout << std::string(depth * 4, ' ') << "}";
                std::cout << std::endl;
            }
            break;
        default:
            std::cout << "(\?\?\?)";
            break;
    }
}

template <>
void debug_dump<MapType>(const MapType & map)
{
    output(map, 0);
}

template <>
void debug_dump<ListType>(const ListType & list)
{
    output(list, 0);
}
