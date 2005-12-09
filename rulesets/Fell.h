// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef RULESETS_FELL_H
#define RULESETS_FELL_H

#include "rulesets/Task.h"

/// \brief Task class for felling trees
class Fell : public Task {
  public:
    explicit Fell(Character & chr, Entity & tool, Entity & target);
    virtual ~Fell();

    virtual void initTask(const Operation & op, OpVector & res);
    virtual void TickOperation(const Operation & op, OpVector & res);
};

#endif // RULESETS_FELL_H
