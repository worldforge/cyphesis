// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_EXTERNAL_MIND_H
#define RULESETS_EXTERNAL_MIND_H

#include <rulesets/BaseMind.h>

class Connection;

class ExternalMind : public BaseMind {
  public:
    Connection & connection;

    ExternalMind(Connection & connection, const std::string & id, const std::string & name);

    virtual OpVector message(const RootOperation & op);
};

#endif // RULESETS_EXTERNAL_MIND_H
