#ifndef EXTERNAL_MIND_H
#define EXTERNAL_MIND_H

#include "BaseMind.h"

class Connection;

class ExternalMind : public BaseMind {
  public:
    Connection * connection;

    ExternalMind(Connection * connection, string & id, string & name);

    virtual oplist message(const RootOperation & op);
};

#endif /* EXTERNAL_MIND_H */
