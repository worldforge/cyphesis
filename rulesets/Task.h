// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef RULESETS_TASK_H
#define RULESETS_TASK_H

#include "common/types.h"

/// \brief Interface class for handling tasks which take a short while to
/// complete
class Task {
  protected:
    /// \brief Flag to indicate if this task is obsolete and should be removed
    bool m_obsolete;

    Task();
  private:
    /// \brief Private and un-implemented, to make sure slicing is impossible
    Task(const Task & t);
    /// \brief Private and un-implemented, to make sure slicing is impossible
    const Task & operator=(const Task &);
  public:
    virtual ~Task();

    /// \brief Handle a tick operation to perform the task
    ///
    /// A Task gets regular ticks which cause whatever actions this
    /// Task involves to be returned.
    virtual void TickOperation(const Operation & op, OpVector & res) = 0;

    void irrelevant();

    /// \brief Accessor to determine if this Task is obsolete
    const bool obsolete() { return m_obsolete; }
};

#endif // RULESETS_TASK_H
