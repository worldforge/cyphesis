// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

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
