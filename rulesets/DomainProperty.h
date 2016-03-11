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
#ifndef DOMAINPROPERTY_H_
#define DOMAINPROPERTY_H_

#include "common/Property.h"
#include "common/PropertyInstanceState.h"

class Domain;

/**
 * @brief Specifies that an entity contains a "Domain".
 *
 * This makes the entity responsible for physics and sight checking, effectively giving the entity
 * it's own space.
 * Normally you wouldn't use nested domains, since the effects (currently) are undefined.
 *
 * The data defines the kind of domain. The available options are:
 * * void: no movement or sight allowed
 * * physical: movement and sights behave like in the real world
 */
class DomainProperty: public Property<std::string> {
    public:

        explicit DomainProperty();
        explicit DomainProperty(const DomainProperty& rhs);

        virtual void install(LocatedEntity *, const std::string &);
        virtual void remove(LocatedEntity *, const std::string &);
        virtual DomainProperty * copy() const;

        virtual void apply(LocatedEntity *);

        Domain* getDomain(const LocatedEntity *) const;

        HandlerResult operation(LocatedEntity * e, const Operation & op, OpVector & res);

    private:

        static PropertyInstanceState<Domain> sInstanceState;

        void scheduleTick(LocatedEntity& entity, float secondsFromNow);
        HandlerResult tick_handler(LocatedEntity * e, const Operation & op, OpVector & res);

};

#endif /* DOMAINPROPERTY_H_ */
