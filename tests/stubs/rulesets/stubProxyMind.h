/*
 Copyright (C) 2013 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "rulesets/ProxyMind.h"


using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

ProxyMind::ProxyMind(const std::string & id, long intId) :
        BaseMind(id, intId)
{

}

ProxyMind::~ProxyMind()
{
}

void ProxyMind::thinkSetOperation(const Operation & op, OpVector & res)
{
}

void ProxyMind::thinkDeleteOperation(const Operation & op, OpVector & res)
{
}

void ProxyMind::thinkGetOperation(const Operation & op, OpVector & res)
{
}

void ProxyMind::thinkLookOperation(const Operation & op, OpVector & res)
{

}

std::vector<Atlas::Objects::Root> ProxyMind::getThoughts() const
{
    std::vector<Atlas::Objects::Root> thoughts;
    return thoughts;
}

void ProxyMind::clearThoughts()
{
}

