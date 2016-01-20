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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "common/FormattedXMLWriter.h"

#include <iostream>

#include <cassert>

int main()
{
    FormattedXMLWriter writer(std::cout);

    writer.closeOutput();

    Atlas::Message::MapType foo;
    foo["a"] = 1;
    foo["b"] = true;
    foo["c"] = 1.;
    foo["d"] = "one";
    foo["e"] = Atlas::Message::ListType(1, 1);
    foo["f"] = Atlas::Message::Element();

    Atlas::Message::MapType bar;
    bar["a"] = 1;
    bar["b"] = true;
    bar["c"] = 1.;
    bar["d"] = "one";
    bar["e"] = Atlas::Message::ListType(1, 1);

    Atlas::Message::MapType mim;
    mim["a"] = 1;
    mim["b"] = true;
    mim["c"] = 1.;
    mim["d"] = "one";
    mim["e"] = Atlas::Message::ListType(1, 1);

    bar["z"] = mim;

    foo["z"] = bar;

    writer.outputObject(foo);

    writer.openOutput();

    return 0;
}
