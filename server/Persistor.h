// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_PERSISTOR_H
#define SERVER_PERSISTOR_H

#include <string>

template <class T>
class Persistor {
  private:
    std::string m_class;
  public:
    // Register table with database
    Persistor();

    void persist(T &);
};

#endif // SERVER_PERSISTOR_H
