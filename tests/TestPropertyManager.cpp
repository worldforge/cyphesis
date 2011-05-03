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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestPropertyManager.h"

#include "common/Property.h"

#include <Atlas/Message/Element.h>

using Atlas::Message::Element;

TestPropertyManager::TestPropertyManager()
{
}

TestPropertyManager::~TestPropertyManager()
{
}

PropertyBase * TestPropertyManager::addProperty(const std::string & name,
                                                int type)
{
    PropertyBase * p = 0;
    switch (type) {
      case Element::TYPE_INT:
        p = new Property<int>;
        break;
      case Element::TYPE_FLOAT:
        p = new Property<double>;
        break;
      case Element::TYPE_STRING:
        p = new Property<std::string>;
        break;
      default:
        p = new SoftProperty;
        break;
    }

    return p;
}
