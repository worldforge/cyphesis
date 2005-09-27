// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "EntityExerciser.h"
#include "allOperations.h"

#include "common/BaseEntity.h"

#include <cassert>

class BaseEntityTest : public BaseEntity {
  public:
    BaseEntityTest() : BaseEntity("testId") { }
};

int main()
{
    BaseEntityTest be;

    EntityExerciser<BaseEntity> ee(be);

    ee.runOperations();

    std::set<std::string> opNames;
    ee.addAllOperations(opNames);

    ee.runOperations();

    return 0;
}
