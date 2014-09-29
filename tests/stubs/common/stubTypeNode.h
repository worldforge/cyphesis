/*
 Copyright (C) 2014 Erik Ogenvik

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

#include "common/TypeNode.h"


TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

TypeNode::~TypeNode()
{
}

void TypeNode::addProperties(const Atlas::Message::MapType & attributes)
{
}

void TypeNode::updateProperties(const Atlas::Message::MapType & attributes)
{
}

bool TypeNode::isTypeOf(std::string const&) const {
    return true;
}

bool TypeNode::isTypeOf(const TypeNode * base_type) const
{
    return true;
}
