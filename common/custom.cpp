// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/RootOperation.h>
#include "Chop.h"
#include "Cut.h"
#include "Eat.h"
#include "Fire.h"
#include "Generic.h"
#include "Load.h"
#include "Nourish.h"
#include "Save.h"
#include "Setup.h"
#include "Tick.h"

#include "inheritance.h"
#include "operations.h"

void installCustomOperations()
{
    Inheritance & i = Inheritance::instance();

    i.addChild("action", new Chop());
    i.addChild("action", new Cut());
    i.addChild("action", new Eat());
    i.addChild("action", new Fire());
    i.addChild("set", new Load());
    i.addChild("action", new Nourish());
    i.addChild("get", new Save());
    i.addChild("root_operation", new Setup());
    i.addChild("root_operation", new Tick());
}
