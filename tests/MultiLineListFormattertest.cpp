// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Atlas/MultiLineListFormatter.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>
#include <Atlas/Message/MEncoder.h>
#include <Atlas/Message/QueuedDecoder.h>

#include <iostream>
#include <sstream>

using Atlas::Message::ListType;
using Atlas::Message::MapType;

int main()
{
    {
        Atlas::Message::QueuedDecoder decoder;
        std::stringstream str;
        Atlas::Codecs::XML codec(str, str, decoder);
        Atlas::MultiLineListFormatter  formatter(str, codec);
        Atlas::Message::Encoder encoder(formatter);

        formatter.streamBegin();

        MapType map;
        map["string_list"] = ListType(2, "foo");
        map["string_list_list"] = ListType(2, ListType(2, "foo"));
        map["int_list"] = ListType(2, 23);
        map["float_list"] = ListType(2, 3.14159265);
        map["map_list"] = ListType(2, MapType());

        encoder.streamMessageElement(map);

        formatter.streamEnd();

        std::cout << str.str() << std::endl;
    }
    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}
