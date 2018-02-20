/*
 Cyphesis Online RPG Server and AI Engine
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

#ifndef RULESETS_IMMORTALPROPERTY_H_
#define RULESETS_IMMORTALPROPERTY_H_

#include "common/Property.h"

/// \brief Property allowing for immortal entities
///
/// When this property is attached to an entity and set to 1 it intercepts any
/// Delete operations and ignores them, thus making the entity immortal.
/// \ingroup PropertyClasses
class ImmortalProperty : public Property<int>
{
    public:

        void install(LocatedEntity *, const std::string &) override;

        void remove(LocatedEntity *, const std::string &) override;

        void apply(LocatedEntity *) override;

        HandlerResult operation(LocatedEntity *,
                                        const Operation &,
                                        OpVector &) override;

        ImmortalProperty * copy() const override;


        HandlerResult delete_handler(LocatedEntity * e,
                                  const Operation & op,
                                  OpVector & res);

    private:

};

#endif /* RULESETS_IMMORTALPROPERTY_H_ */
