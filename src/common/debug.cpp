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


#include "debug.h"

#include <Atlas/Message/Element.h>
#include <Atlas/PresentationBridge.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

void output_element(std::ostream& out, const Element& item, size_t depth)
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
        case Element::TYPE_LIST: {
            out << "[ ";
            for (auto& entry : item.List()) {
                output_element(out, entry, depth + 1);
                out << " ";
            }
            out << "]";
        }
            break;
        case Element::TYPE_MAP: {
            out << "{" << std::endl;
            for (auto& entry : item.Map()) {
                out << std::string((depth + 1) * 4, ' ') << entry.first << ": ";
                output_element(out, entry.second, depth + 1);
                out << std::endl;
            }
            out << std::string(depth * 4, ' ') << "}";
            out << std::endl;
        }
            break;
        default:
            out << R"((???))";
            break;
    }
}

template<>
void debug_dump(const Atlas::Objects::Operation::RootOperation& t, std::ostream& os)
{
    std::stringstream ss;
    Atlas::PresentationBridge bridge(ss);
    Atlas::Objects::ObjectsEncoder encoder(bridge);
    encoder.streamObjectsMessage(t);
    os << ss.str();
}

template<>
void debug_dump(const Atlas::Objects::Root& t, std::ostream& os)
{
    std::stringstream ss;
    Atlas::PresentationBridge bridge(ss);
    Atlas::Objects::ObjectsEncoder encoder(bridge);
    encoder.streamObjectsMessage(t);
    os << ss.str();
}

template<>
void debug_dump(const Atlas::Objects::Entity::Anonymous& t, std::ostream& os)
{
    std::stringstream ss;
    Atlas::PresentationBridge bridge(ss);
    Atlas::Objects::ObjectsEncoder encoder(bridge);
    encoder.streamObjectsMessage(t);
    os << ss.str();
}

template<typename T>
void debug_dump(const T& t, std::ostream& os)
{
    output_element(os, t, 0);
}


template<>
std::string debug_tostring(const Atlas::Objects::Root& t)
{
    std::stringstream out(std::ios::out);
    debug_dump(t, out);
    return out.str();
}

template<>
std::string debug_tostring(const Atlas::Objects::Operation::RootOperation& t)
{
    std::stringstream out(std::ios::out);
    debug_dump(t, out);
    return out.str();
}


template<typename T>
std::string debug_tostring(const T& t)
{
    std::stringstream out(std::ios::out);
    output_element(out, t, 0);
    return out.str();
}

template
void debug_dump<MapType>(const MapType& map, std::ostream& os);

template
void debug_dump<ListType>(const ListType& list, std::ostream& os);

template
void debug_dump<Element>(const Element& e, std::ostream& os);

template
std::string debug_tostring<MapType>(const MapType& map);

template
std::string debug_tostring<ListType>(const ListType& list);

template
std::string debug_tostring<Element>(const Element& e);

