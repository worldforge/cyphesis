// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_CREATOR_H
#define RULESETS_CREATOR_H

#include "Character.h"

typedef Character Creator_parent;

class Creator : public Creator_parent {
  public:
    explicit Creator(const std::string & id);

    void sendExternalMind(const RootOperation & msg, OpVector &);

    virtual void operation(const RootOperation & op, OpVector &);
    virtual void externalOperation(const RootOperation & op, OpVector &);

    virtual void mindLookOperation(const Look & op, OpVector &);
};

#endif // RULESETS_CREATOR_H
