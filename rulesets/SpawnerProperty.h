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
/// entity: an optional entity declaration, to be sent as argument in a Create op
/// interval: an optional numeric value specifying the interval, in seconds, between
///           ticks. If omitted, a default value will be used.
/// \ingroup PropertyClasses
class SpawnerProperty : public Property<Atlas::Message::MapType>
{
    public:
        explicit SpawnerProperty();
        virtual ~SpawnerProperty();

        virtual void install(LocatedEntity *, const std::string &);
        virtual void apply(LocatedEntity *);
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

        /**
         * @brief An optional entity definition.
         *
         * This can be anything which can be created through a Create op, but
         * most probably either an entity or an archetype.
         *
         * If absent, an entity of the m_type will be created automatically.
         */
        Atlas::Message::MapType m_entity;

        /**
         * @brief The tick interval.
         *
         * If set to 0 a default value will be used.
         */
        int m_interval;

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
