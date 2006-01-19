// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#include "common/Property.h"

#include <cassert>

using Atlas::Message::Element;

int main()
{
    Element val;

    {
    long i = 23;
    PropertyBase * pb = new Property<long>(i, 0);
    assert(pb->flags() == 0);
    pb->get(val);
    assert(val == i);
    delete pb;
    }

    {
    float f = 17.2f;
    PropertyBase * pb = new Property<float>(f, 1);
    assert(pb->flags() == 1);
    pb->get(val);
    assert(val == f);
    delete pb;
    }

    {
    double d = 65.4;
    PropertyBase * pb = new Property<double>(d, 2);
    assert(pb->flags() == 2);
    pb->get(val);
    assert(val == d);
    delete pb;
    }

    {
    std::string s = "Test String";
    PropertyBase * pb = new Property<std::string>(s, 3);
    assert(pb->flags() == 3);
    pb->get(val);
    assert(val == s);
    delete pb;
    }

    {
    long i = 23;
    PropertyBase * pb = new Property<long>(i, 4);
    assert(pb->flags() == 4);
    pb->get(val);
    assert(val == i);
    delete pb;
    }
}
