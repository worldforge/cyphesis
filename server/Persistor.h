// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_PERSISTOR_H
#define SERVER_PERSISTOR_H

#include <sigc++/object.h>

#include <string>

class Entity;
class Character;
class Line;
class Area;
class Plant;

template <class T>
class Persistor : virtual public SigC::Object {
  private:
    const std::string m_class;

    void uEntity(Entity &, std::string &);
    void uCharacter(Character &, std::string &);
    void uLine(Line &, std::string &);
    void uArea(Area &, std::string &);
    void uPlant(Plant &, std::string &);

    void cEntity(Entity &, std::string &, std::string &);
    void cCharacter(Character &, std::string &, std::string &);
  public:
    // Register table with database
    explicit Persistor(bool temp = false);

    void hookup(T &);
    void persist(T &);
    void update(T *);
    void remove(T *);
};

#endif // SERVER_PERSISTOR_H
