// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Sight.h>
#include <common/operations.h>

#include "DumbSkill.h"

using Atlas::Message::Object;

oplist DumbSkill::action(const std::string & skill, const RootOperation & op)
{
    RootOperation * s = new Sight(Sight::Instantiate());
    Object::ListType args(1,op.AsObject());
    s->SetArgs(args);
    return oplist(1,s);
}
