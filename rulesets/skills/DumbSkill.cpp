// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "common/operations.h"

#include <Atlas/Objects/Operation/Sight.h>

#include "DumbSkill.h"

using Atlas::Message::Object;

oplist DumbSkill::action(const std::string & skill, const RootOperation & op)
{
    RootOperation * s = new Sight(Sight::Instantiate());
    s->SetArgs(Object::ListType(1,op.AsObject()));
    return oplist(1,s);
}
