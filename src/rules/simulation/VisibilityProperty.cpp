// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "VisibilityProperty.h"

#include "rules/LocatedEntity.h"

VisibilityProperty::VisibilityProperty()
    : mData(Visibility::Public)
{
}

VisibilityProperty* VisibilityProperty::copy() const
{
    return new VisibilityProperty(*this);
}

int VisibilityProperty::get(Atlas::Message::Element& e) const
{
    switch (mData) {
        case Visibility::Public: {
            // no-op
        } break;
        case Visibility::Protected: {
            e = "protected";
        } break;
        case Visibility::Private: {
            e = "private";
        } break;
    }
    return 0;
}

void VisibilityProperty::set(const Atlas::Message::Element& e)
{
    if (e.isString()) {
        if (e.String() == "protected") {
            mData = Visibility::Protected;
        } else if (e.String() == "private") {
            mData = Visibility::Private;
        }
    }
}

void VisibilityProperty::apply(LocatedEntity& entity)
{
    switch (mData) {
        case Visibility::Public: {
            entity.removeFlags(entity_visibility_private);
            entity.removeFlags(entity_visibility_protected);
        } break;
        case Visibility::Protected: {
            entity.removeFlags(entity_visibility_private);
            entity.addFlags(entity_visibility_protected);
        } break;
        case Visibility::Private: {
            entity.addFlags(entity_visibility_private);
            entity.removeFlags(entity_visibility_protected);
        } break;
    }
}
