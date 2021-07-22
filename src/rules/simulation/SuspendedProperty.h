// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Erik Hjortsberg
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


#ifndef RULESETS_SUSPENDEDPROPERTY_H_
#define RULESETS_SUSPENDEDPROPERTY_H_

#include "common/Property.h"
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>

/**
 * \brief Suspends either the world or an entity.
 *
 * When an entity is suspended it won't react to any Tick operations.
 * These are instead stored and sent again when the entity is resumed.
 *
 * When this property is applied on the world, i.e. the entity with id 0,
 * the effect is slightly different from when its applied to another entity.
 * When applied to the world it means that _all_ Tick operations in the whole system
 * are suspended.
 *
 * \ingroup PropertyClasses
 */
class SuspendedProperty : public BoolProperty
{
    public:
        static constexpr const char* property_name = "suspended";

        explicit SuspendedProperty() = default;

        SuspendedProperty* copy() const override;

        void apply(LocatedEntity&) override;

        void install(LocatedEntity&, const std::string&) override;

        void remove(LocatedEntity&, const std::string& name) override;

        HandlerResult operation(LocatedEntity& e,
                                const Operation& op, OpVector& res) override;

    protected:
        SuspendedProperty(const SuspendedProperty& rhs) = default;

        /**
         * \brief Store ops that needs to be sent again when the entity is resumed.
         */
        OpVector m_suspendedOps;

};

#endif /* RULESETS_SUSPENDEDPROPERTY_H_ */
