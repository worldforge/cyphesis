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

#ifndef RULESETS_MODESPECPROPERTY_H_
#define RULESETS_MODESPECPROPERTY_H_

#include "common/Property.h"
#include "physics/Transform.h"

/**
 * \brief Specifies how the entity should behave when put into different modes.
 *
 * Currently we support transformations, as done through the TransformsProperty.
 *
 * \ingroup PropertyClasses
 */
class ModeSpecProperty : public Property<Atlas::Message::MapType>{
    public:
        ModeSpecProperty();

        virtual int get(Atlas::Message::Element & val) const;
        virtual void set(const Atlas::Message::Element &);

        const Transform& getTransform() const;

    private:

        /**
         * A transformation to apply to the entity when put into a specific mode.
         */
        Transform mTransform;


};

#endif /* RULESETS_MODESPECPROPERTY_H_ */
