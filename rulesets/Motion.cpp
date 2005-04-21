// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "Motion.h"

Motion::Motion(Entity & body) : m_entity(body)
{
}

Motion::~Motion()
{
}

void Motion::setMode(const std::string & mode)
{
    m_mode = mode;
    // FIXME Re-configure stuff, and possible schedule an update?
}

void Motion::adjustPostion()
{
}

Operation * Motion::genUpdateOperation()
{
}

Operation * Motion::genMoveOperation()
{
}
