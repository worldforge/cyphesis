// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

#ifndef TESTS_IG_ENTITY_EXERCISER_H
#define TESTS_IG_ENTITY_EXERCISER_H

#include "EntityExerciser.h"
#include "TestPropertyManager.h"

#include <Atlas/Message/Element.h>

template <class EntityType>
class IGEntityExerciser : public EntityExerciser<EntityType> {
  public:
    explicit IGEntityExerciser(EntityType & e) : EntityExerciser<EntityType>(e) { new TestPropertyManager; }

    bool checkAttributes(const std::set<std::string> & attr_names);
    bool checkProperties(const std::set<std::string> & prop_names);
};

template <class EntityType>
inline bool IGEntityExerciser<EntityType>::checkAttributes(const std::set<std::string> & attr_names)
{
    Atlas::Message::Element null;
    std::set<std::string>::const_iterator I = attr_names.begin();
    std::set<std::string>::const_iterator Iend = attr_names.end();
    for (; I != Iend; ++I) {
        if (!this->m_ent.getAttr(*I, null)) {
            std::cerr << "Entity does not have \"" << *I << "\" attribute."
                      << std::endl << std::flush;
            return false;
        }
    }
    return true;
}

template <class EntityType>
inline bool IGEntityExerciser<EntityType>::checkProperties(const std::set<std::string> & prop_names)
{
    std::set<std::string>::const_iterator I = prop_names.begin();
    std::set<std::string>::const_iterator Iend = prop_names.end();
    for (; I != Iend; ++I) {
        if (this->m_ent.getProperty(*I) == NULL) {
            std::cerr << "Entity does not have \"" << *I << "\" property."
                      << std::endl << std::flush;
            return false;
        }
    }
    return true;
}

#endif // TESTS_IG_ENTITY_EXERCISER_H
