// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2015 Erik Ogenvik
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

#include "Transform.h"
#include <wfmath/atlasconv.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

int Transform::get(MapType & val) const {
    if (rotate.isValid()) {
        val["rotate"] = rotate.toAtlas();
    }
    if (translate.isValid()) {
        val["translate"] = translate.toAtlas();
    }
    if (translateScaled.isValid()) {
        val["translate-scaled"] = translateScaled.toAtlas();
    }
    return 0;
}

void Transform::set(const Element & val) {

    if (val.isMap()) {
        auto& valMap = val.Map();
        auto I = valMap.find("rotate");
        if (I != valMap.end()) {
            rotate = WFMath::Quaternion(I->second);
        }
        I = valMap.find("translate");
        if (I != valMap.end()) {
            translate = WFMath::Vector<3>(I->second);
        }
        I = valMap.find("translate-scaled");
        if (I != valMap.end()) {
            translateScaled = WFMath::Vector<3>(I->second);
        }

    }
}

