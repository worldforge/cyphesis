// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_EXTERNAL_MIND_H
#define SERVER_EXTERNAL_MIND_H

#include "rulesets/BaseMind.h"

class Connection;

class ExternalMind : public BaseMind {
  public:
    Connection & connection;

    ExternalMind(Connection & connection, const std::string & id, const std::string & name);
    virtual ~ExternalMind();

    virtual OpVector message(const RootOperation & op);
};

#endif // SERVER_EXTERNAL_MIND_H
