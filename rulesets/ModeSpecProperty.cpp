// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2016 Erik Ogenvik
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
#include "ModeSpecProperty.h"

ModeSpecProperty::ModeSpecProperty() {
}

int ModeSpecProperty::get(Atlas::Message::Element & val) const {
    Atlas::Message::MapType transformMap;
    mTransform.get(transformMap);

    if (!transformMap.empty()) {
        Atlas::Message::MapType map;
        map.insert(std::make_pair("transform", transformMap));
        val = map;
        return 0;
    }

    return 1;

}

void ModeSpecProperty::set(const Atlas::Message::Element & val) {

    if (val.isMap()) {
        auto valMap = val.Map();
        auto I = valMap.find("transform");
        if (I != valMap.end()) {
            mTransform.set(I->second);
        }
    }
}

const Transform& ModeSpecProperty::getTransform() const
{
    return mTransform;
}

ModeSpecProperty * ModeSpecProperty::copy() const
{
    return new ModeSpecProperty(*this);
}



