// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_EXTERNAL_MIND_H
#define SERVER_EXTERNAL_MIND_H

#include "common/BaseEntity.h"

class Connection;

/// \brief This class connects in-game entities to the Connection of the client
/// controlling it.
///
/// Essentially used to relay in-game operations that pass to the mind on
/// to the client.
class ExternalMind : public BaseEntity {
  public:
    Connection & m_connection;

    ExternalMind(Connection & connection, const std::string & id);
    virtual ~ExternalMind();

    virtual OpVector operation(const RootOperation & op);
};

#endif // SERVER_EXTERNAL_MIND_H
