// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_PERSISTOR_H
#define SERVER_PERSISTOR_H

#include <sigc++/object.h>

#include <string>

template <class T>
class Persistor : public SigC::Object {
  private:
    std::string m_class;

    void uEntity(T &, std::string &);
    void uCharacter(T &, std::string &);
    void uLine(T &, std::string &);
    void uArea(T &, std::string &);
    void uPlant(T &, std::string &);
  public:
    // Register table with database
    Persistor();

    void persist(T &);
    void update(T &);
    void remove(T &);
};

#endif // SERVER_PERSISTOR_H
