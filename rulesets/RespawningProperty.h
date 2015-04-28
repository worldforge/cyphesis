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

#ifndef RULESETS_RESPAWNINGPROPERTY_H_
#define RULESETS_RESPAWNINGPROPERTY_H_

#include "common/Property.h"
#include "common/PropertyInstanceState.h"
#include <sigc++/connection.h>



/// \brief Will make entities respawn at a spawn point when deleted, or to "limbo".
///
/// This property will intercept the Delete op, and instead of deleting the entity it
/// will respawn it at the specified spawn point. This should for example be applied
/// to player entities, which never should be deleted but instead respawn when killed.
///
/// The property also has the additional feature of putting entities that dies without
/// having any externally controlling client into "limbo". While in the "limbo" state
/// nothing happens to the entity. First when the entity again becomes externally controlled
/// is it moved back to the spawn.
/// The reason for doing this is that we don't want those entities/characters that have
/// been abandoned to litter the world. This way, if a character has been abandoned it
/// will end up in limbo, but not bother the rest of the world.
/// Entities that are put in limbo will be suspended, through the "suspended" property.
///
/// The property accepts a string, which is the name of a spawn point.
///
/// \ingroup PropertyClasses
class RespawningProperty : public Property<std::string>
{
    public:
        explicit RespawningProperty();
        virtual ~RespawningProperty();

        virtual void install(LocatedEntity *, const std::string &);
        virtual void remove(LocatedEntity *, const std::string &);
        virtual void apply(LocatedEntity *);
        virtual HandlerResult operation(LocatedEntity *,
                                        const Operation &,
                                        OpVector &);
        virtual RespawningProperty * copy() const;


        HandlerResult delete_handler(LocatedEntity * e,
                                  const Operation & op,
                                  OpVector & res);

    private:


        /// \brief Keeps a connection to the signal which is emitted when the entity
        /// gets externally controlled. This is used when the entity has been put in
        /// limbo, for which we'll have to respawn the entity when it regains external
        /// control.
        static PropertyInstanceState<sigc::connection> sInstanceState;

        /// \brief Called when a character that is in limbo gets externally controlled.
        ///
        /// The character should then return to the world.
        void entity_externalLinkChanged(LocatedEntity* entity);


};

#endif /* RULESETS_RESPAWNINGPROPERTY_H_ */
