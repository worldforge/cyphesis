// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MEM_ENTITY_H
#define MEM_ENTITY_H

#include "rulesets/Entity.h"

class MemEntity : public Entity {
  public:
    explicit MemEntity(const std::string & id);
    virtual ~MemEntity();
};

#endif // MEM_ENTITY_H
