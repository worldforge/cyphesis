// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef TESTS_IG_ENTITY_EXERCISER_H
#define TESTS_IG_ENTITY_EXERCISER_H

#include "EntityExerciser.h"

template <class EntityType>
class IGEntityExerciser : public EntityExerciser<EntityType> {
  public:
    explicit IGEntityExerciser(EntityType & e) : EntityExerciser<EntityType>(e) { }

    bool checkAttributes(const std::set<std::string> & attr_names);
};

template <class EntityType>
inline bool IGEntityExerciser<EntityType>::checkAttributes(const std::set<std::string> & attr_names)
{
    Element null;
    std::set<std::string>::const_iterator I = attr_names.begin();
    for(; I != attr_names.end(); ++I) {
        if (!this->m_ent.get(*I, null)) {
            return false;
        }
    }
    return true;
}

#endif // TESTS_IG_ENTITY_EXERCISER_H
