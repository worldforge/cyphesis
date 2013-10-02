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

#ifndef RULESETS_SPAWNERPROPERTY_H_
#define RULESETS_SPAWNERPROPERTY_H_

#include "common/Property.h"

/// \brief Class to handle automatic spawning behaviour.
///
/// When this property is attached to an entity it causes that entity to become
/// a "spawner". This makes it create new entities when it determines they are
/// needed. This allows for game play mechanics where some items or creatures
/// always are available.
///
/// This property is of "map" type.
/// These values are available:
/// type: a string specifying the type of entity to create
/// minamount: the desired minimum amount of entities (optionally within a radius)
///            if the actual number of entities dips below new ones are created
/// radius: an optional radius around the entity to consider when checking minamount
/// \ingroup PropertyClasses
class SpawnerProperty : public PropertyBase
{
    public:
        explicit SpawnerProperty();
        virtual ~SpawnerProperty();

        virtual void install(LocatedEntity *, const std::string &);
        virtual void apply(LocatedEntity *);
        virtual int get(Atlas::Message::Element &) const;
        virtual void set(const Atlas::Message::Element &);
        virtual HandlerResult operation(LocatedEntity *,
                                        const Operation &,
                                        OpVector &);
        virtual SpawnerProperty * copy() const;


        HandlerResult tick_handler(LocatedEntity * e,
                                  const Operation & op,
                                  OpVector & res);

    private:
        /**
         * @brief An optional radius to check within.
         */
        float m_radius;

        /**
         * @brief The minimum amount of entities.
         */
        int m_minamount;

        /**
         * @brief The type of entity.
         */
        std::string m_type;

        Atlas::Objects::Operation::RootOperation createTickOp();

        /**
         * Handle one of our own ticks.
         * @param e
         * @param op
         * @param res
         */
        void handleTick(LocatedEntity * e,
                const Operation & op,
                OpVector & res);

        /**
         * Create a new entity.
         * @param e
         * @param op
         * @param res
         */
        void createNewEntity(LocatedEntity * e,
                const Operation & op,
                OpVector & res);
};

#endif /* RULESETS_SPAWNERPROPERTY_H_ */
