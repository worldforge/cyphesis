#ifndef EXTERNAL_MIND_H
#define EXTERNAL_MIND_H

#include "BaseMind.h"

class Connection;

class ExternalMind : public BaseMind {
  public:
    Connection * connection;

    ExternalMind(Connection * connection, string & id);

    virtual oplist message(const RootOperation & msg);
};

#endif /* EXTERNAL_MIND_H */
