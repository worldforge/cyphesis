// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004-2005 Alistair Riddoch

#include "Task.h"

/// \brief Task constructor for classes which inherit from Task
Task::Task(Character & chr) : m_refCount(0), m_obsolete(false), m_character(chr)
{
}

/// \brief Task destructor
Task::~Task()
{
}

/// \brief Set the obsolete flag indicating that this task is obsolete
///
/// The task should be checked to see if it is irrelevant before any
/// processing is passed to it. A typical example is the Task may
/// contain references to entities which are deleted. irrelevant() could
/// be connected to the destroyed signal of the entities, so that it
/// never de-references the pointers to deleted entities.
void Task::irrelevant()
{
    m_obsolete = true;
}
