// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "Task.h"

Task::Task() : m_obsolete(false)
{
}

Task::~Task()
{
}

void Task::irrelevant()
{
    m_obsolete = true;
}
