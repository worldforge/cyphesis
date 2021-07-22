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

#ifndef RULESETS_VISIBILITY_PROPERTY_H
#define RULESETS_VISIBILITY_PROPERTY_H

#include "common/Property.h"

/**
 * \brief Allows setting visibility of entities to either "public", "protected" or "private".
 * "public" means that normal visibility rules applies.
 * "protected" means that only parent entities can see it.
 * "private" means that only the simulation or admins can see it.
 */
class VisibilityProperty : public PropertyBase
{
  public:
    static constexpr const char* property_name = "visibility";
    static constexpr const char* property_atlastype = "string";

    VisibilityProperty();

    ~VisibilityProperty() override = default;

    VisibilityProperty* copy() const override;

    int get(Atlas::Message::Element&) const override;

    void set(const Atlas::Message::Element&) override;

    void apply(LocatedEntity& entity) override;

  protected:
    VisibilityProperty(const VisibilityProperty& rhs) = default;
    enum class Visibility { Public, Protected, Private };

    Visibility mData;
};

#endif  // RULESETS_VISIBILITY_PROPERTY_H
