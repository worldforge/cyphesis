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

    bool checkAttrbuites(const std::set<std::string> & attr_names);
};

#endif // TESTS_IG_ENTITY_EXERCISER_H
