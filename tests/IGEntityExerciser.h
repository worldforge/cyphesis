// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef TESTS_IG_ENTITY_EXERCISER_H
#define TESTS_IG_ENTITY_EXERCISER_H

#include "EntityExerciser.h"

#include <Atlas/Message/Element.h>

template <class EntityType>
class IGEntityExerciser : public EntityExerciser<EntityType> {
  public:
    explicit IGEntityExerciser(EntityType & e) : EntityExerciser<EntityType>(e) { }

    bool checkAttributes(const std::set<std::string> & attr_names);
};

template <class EntityType>
inline bool IGEntityExerciser<EntityType>::checkAttributes(const std::set<std::string> & attr_names)
{
    Atlas::Message::Element null;
    std::set<std::string>::const_iterator I = attr_names.begin();
    std::set<std::string>::const_iterator Iend = attr_names.end();
    for (; I != Iend; ++I) {
        if (!this->m_ent.get(*I, null)) {
            std::cerr << "Entity does not have \"" << *I << "\" attribute."
                      << std::endl << std::flush;
            return false;
        }
    }
    return true;
}

#endif // TESTS_IG_ENTITY_EXERCISER_H
