#ifndef EXTERNAL_MIND_H
#define EXTERNAL_MIND_H

#include "NPCMind.h"

class ExternalMind : public NPCMind {
    Connection * connection;
  public:
    ExternalMind(Connection * connection);
    virtual RootOperation * message(const RootOperation & msg);
};

#endif /* EXTERNAL_MIND_H */
