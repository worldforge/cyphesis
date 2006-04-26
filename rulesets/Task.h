// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004-2005 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef RULESETS_TASK_H
#define RULESETS_TASK_H

#include "common/types.h"

#include <cassert>

class Character;

/// \brief Interface class for handling tasks which take a short while to
/// complete
class Task {
  private:

    /// \brief Count of references held by entities involved in this task
    int m_refCount;
  protected:

    /// \brief Serial number of the tick due to arrive next at this task.
    int m_serialno;

    /// \brief Flag to indicate if this task is obsolete and should be removed
    bool m_obsolete;

    /// \brief Character performing the task
    ///
    /// This doesn't handle the idea that there might be multiple actors
    /// involved.
    Character & m_character;

    /// \brief Protected constructor to be called by classes which inherit
    explicit Task(Character & chr);
  private:
    /// \brief Private and un-implemented, to make sure slicing is impossible
    Task(const Task & t);
    /// \brief Private and un-implemented, to make sure slicing is impossible
    const Task & operator=(const Task &);
  public:
    virtual ~Task();

    /// \brief Flag this task as obsolete
    virtual void irrelevant();

    /// \brief Handle the operation that instigates the task
    ///
    /// @param op The operation to be processed
    /// @param res The result of the operation is returned here.
    virtual void initTask(const Operation & op, OpVector & res) = 0;

    /// \brief Handle a tick operation to perform the task
    ///
    /// A Task gets regular ticks which cause whatever actions this
    /// Task involves to be returned.
    /// @param op The operation to be processed
    /// @param res The result of the operation is returned here.
    virtual void TickOperation(const Operation & op, OpVector & res) = 0;

    void incRef() {
        ++m_refCount;
    }

    void decRef() {
        if (m_refCount <= 1) {
            assert(m_refCount == 1);
            delete this;
        } else {
            --m_refCount;
        }
    }

    /// \brief Return the number of entities involved in this task
    int count() {
        return m_refCount;
    }

    /// \brief Return the number of the next to arrive at this task
    int serialno() {
        return m_serialno;
    }

    /// \brief Accessor to determine if this Task is obsolete
    const bool obsolete() { return m_obsolete; }

    /// \brief Accessor for character that owns this Task.
    Character & character() const { return m_character; }
};

#endif // RULESETS_TASK_H
