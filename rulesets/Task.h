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

#include "common/OperationRouter.h"

#include <Atlas/Message/Element.h>

#include <string>

#include <cassert>

class LocatedEntity;
class Script;

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

    /// \brief Progress towards task completion
    float m_progress;

    /// \brief Rate of progress towards task completion
    float m_rate;

    /// \brief Entity owning the task
    ///
    /// This doesn't handle the idea that there might be multiple actors
    /// involved.
    LocatedEntity & m_owner;

    /// \brief Additional task attributes
    Atlas::Message::MapType m_attr;

    /// \brief Name of task presented to client
    std::string m_name;

    /// \brief The language script that will handle this task
    Script * m_script;

  private:
    /// \brief Private deleted, to make sure slicing is impossible
    Task(const Task & t) = delete;
    /// \brief Private deleted, to make sure slicing is impossible
    const Task & operator=(const Task &) = delete;
  public:
    /// \brief Constructor
    explicit Task(LocatedEntity & owner);

    ~Task();

    /// \brief Flag this task as obsolete
    void irrelevant();

    /// \brief Handle the operation that instigates the task
    ///
    /// @param op The operation to be processed
    /// @param res The result of the operation is returned here.
    void initTask(const Operation & op, OpVector & res);

    /// \brief Handle an operation to perform the task
    ///
    /// A Task gets regular ticks which cause whatever actions this
    /// Task involves to be returned.
    /// @param op The operation to be processed
    /// @param res The result of the operation is returned here.
    void operation(const Operation & op, OpVector & res);

    /// \brief Create a new tick op for the next iteration of this task
    Operation nextTick(double interval);

    /// \brief Increment the reference count on this task
    void incRef() {
        ++m_refCount;
    }

    /// \brief Decrement the reference count on this task
    void decRef() {
        if (--m_refCount < 1) {
            assert(m_refCount == 0);
            delete this;
        }
    }

    /// \brief Return the number of entities involved in this task
    int count() const {
        return m_refCount;
    }

    /// \brief Return the number of the next to arrive at this task
    int serialno() const {
        return m_serialno;
    }

    /// \brief Return a new tick serial number.
    int newTick() {
        return ++m_serialno;
    }

    /// \brief Accessor for the script that handles this task
    Script * script() const {
        return m_script;
    }

    void setScript(Script * scrpt);

    /// \brief Accessor to determine if this Task is obsolete
    const bool obsolete() { return m_obsolete; }

    /// \brief Accessor for character that owns this Task.
    LocatedEntity & owner() const { return m_owner; }

    /// \brief Accessor for name of this task
    std::string & name() { return m_name; }

    /// \brief Accessor for progress towards completion
    float & progress() { return m_progress; }

    /// \brief Accessor for rate of progress towards completion
    float & rate() { return m_rate; }

    /// \brief Accessor for additional attributes
    int getAttr(const std::string & attr, Atlas::Message::Element & val) const;

    /// \brief Sets additional attribute
    void setAttr(const std::string & attr, const Atlas::Message::Element & val);

    friend class Tasktest;
};

#endif // RULESETS_TASK_H
