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

static void output(std::ostream & out, const Element & item, int depth)
{
    switch (item.getType()) {
        case Element::TYPE_INT:
            out << item.Int();
            break;
        case Element::TYPE_FLOAT:
            out << item.Float();
            break;
        case Element::TYPE_STRING:
            out << "\"" << item.String() << "\"";
            break;
        case Element::TYPE_LIST:
            {
                out << "[ ";
                ListType::const_iterator I = item.List().begin();
                ListType::const_iterator Iend = item.List().end();
                for(; I != Iend; ++I) {
                    output(out, *I, depth + 1);
                    out << " ";
                }
                out << "]";
            }
            break;
        case Element::TYPE_MAP:
            {
                out << "{" << std::endl << std::flush;
                MapType::const_iterator I = item.Map().begin();
                MapType::const_iterator Iend = item.Map().end();
                for(; I != Iend; ++I) {
                    out << std::string((depth + 1) * 4, ' ') << I->first << ": ";
                    output(out, I->second, depth + 1);
                    out << std::endl;
                }
                out << std::string(depth * 4, ' ') << "}";
                out << std::endl;
            }
            break;
        default:
            out << "(\?\?\?)";
            break;
    }
}

template <typename T>
void debug_dump(const T & t)
{
    output(std::cout, t, 0);
}

template
void debug_dump<MapType>(const MapType & map);

template
void debug_dump<ListType>(const ListType & list);

template
void debug_dump<Element>(const Element & e);
