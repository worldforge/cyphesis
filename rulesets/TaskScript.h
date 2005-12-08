// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_TASK_SCRIPT_H
#define RULESETS_TASK_SCRIPT_H

#include "rulesets/Task.h"

class Script;

/// \brief Class for handling tasks which are implemented as scripts
class TaskScript : public Task {
  protected:
    Script * m_script;
  private:
    /// \brief Private and un-implemented, to make sure slicing is impossible
    TaskScript(const TaskScript & t);
    /// \brief Private and un-implemented, to make sure slicing is impossible
    const TaskScript & operator=(const TaskScript &);
  public:
    explicit TaskScript(Character & chr);
    virtual ~TaskScript();

    void setScript(Script * scrpt);

    virtual void setup(OpVector & res);

    virtual void irrelevant();

    virtual void TickOperation(const Operation & op, OpVector & res);
};

#endif // RULESETS_TASK_SCRIPT_H
