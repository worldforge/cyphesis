// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_RESTORATION_H
#define SERVER_RESTORATION_H

class ServerRouting;

class Restoration {
  private:

  public:
    ServerRouting & server;

    Restoration(ServerRouting & svr);

    void read();
};

#endif // SERVER_RESTORATION_H
