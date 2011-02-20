// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

#include "Shape_impl.h"

#include <Atlas/Message/Element.h>

#include <wfmath/polygon.h>
#include <wfmath/stream.h>

using Atlas::Message::MapType;

Shape::Shape()
{
}

template<>
const char * MathShape<WFMath::Polygon, 2>::getType() const
{
    return "polygon";
}

Shape * Shape::newFromAtlas(const MapType & data)
{
    MapType::const_iterator I = data.find("type");
    if (I == data.end() || !I->second.isString()) {
        return 0;
    }
    const std::string & type = I->second.String();
    if (type == "polygon") {
        return new MathShape<WFMath::Polygon>(WFMath::Polygon<2>());
    } else if (type == "ball") {
        return new MathShape<WFMath::Ball>(WFMath::Ball<2>());
    } else if (type == "rotbox") {
        return new MathShape<WFMath::RotBox>(WFMath::RotBox<2>());
    }
    return 0;
}

// template class MathShape<WFMath::Polygon>;
