// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "EntityExerciser.h"

#include "common/BaseEntity.h"

#include "common/Chop.h"
#include "common/Cut.h"
#include "common/Eat.h"
#include "common/Burn.h"
#include "common/Generic.h"
#include "common/Nourish.h"
#include "common/Setup.h"
#include "common/Tick.h"

#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Disappearance.h>
#include <Atlas/Objects/Operation/Divide.h>
#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Feel.h>
#include <Atlas/Objects/Operation/Imaginary.h>
#include <Atlas/Objects/Operation/Listen.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Smell.h>
#include <Atlas/Objects/Operation/Sniff.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Touch.h>

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

    return 0;
}
