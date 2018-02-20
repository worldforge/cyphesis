// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Alistair Riddoch
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


#ifndef RULESETS_DECAYS_PROPERTY_H
#define RULESETS_DECAYS_PROPERTY_H

#include "common/Property.h"

/**
 * \brief Specifies the type that will be created when an entity is destroyed.
 * A simple example would be a "skull" being left over when a character is destroyed.
 * \ingroup PropertyClasses
 */
class DecaysProperty : public Property<std::string>
{
    public:
        void install(LocatedEntity*, const std::string&) override;

        void remove(LocatedEntity*, const std::string&) override;

        HandlerResult operation(LocatedEntity*,
                                const Operation&,
                                OpVector&) override;

        DecaysProperty* copy() const override;

        HandlerResult del_handler(LocatedEntity* e,
                                  const Operation&,
                                  OpVector& res);
};

#endif // RULESETS_DECAYS_PROPERTY_H
