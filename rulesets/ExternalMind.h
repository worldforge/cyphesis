// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#error This file has been removed from the build

#ifndef RULESETS_EXTERNAL_MIND_H
#define RULESETS_EXTERNAL_MIND_H

#include "BaseMind.h"

class Connection;

class ExternalMind : public BaseMind {
  public:
    Connection & connection;

    ExternalMind(Connection & connection, std::string & id, std::string & name);

    virtual oplist message(const RootOperation & op);
};

#endif // RULESETS_EXTERNAL_MIND_H
