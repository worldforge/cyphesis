// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_RESTORER_H
#define SERVER_RESTORER_H

#include "common/Database.h"

#include <string>

class Entity;

template <class T>
class Persistor;

// This class should never ever be instantiated, so the constructor is private
// and unimplemented. Instead the template should be instantiated with
// T as the class to be restored, and a reference to the object being
// restored is cast to a reference to this type, allowing code in
// this class to write to the protected methods.
// This class is probably going to want to directly decode data from the
// database layer.

template <class T>
class Restorer : public T {
  private:
    Restorer(); // DO NOT IMPLEMENT THIS

    bool checkBool(const char * c) {
        if (c == 0) { return false; }
        if (*c == 't') {
            return true;
        } else {
            return false;
        }
    }

    void restoreInt(const char * c, int & i) {
        if (c == 0) { return; }
        i = strtol(c, 0, 10);
    }
    void restoreFloat(const char * c, double & d) {
        if (c == 0) { return; }
        d = strtod(c, 0);
    }
    void restoreFloat(const char * c, float & f) {
        if (c == 0) { return; }
        f = strtof(c, 0);
    }
    void restoreString(const char * c, std::string & s) {
        if (c == 0) { return; }
        s = c;
    }
    void restoreMap(const char * c, Atlas::Message::Object::MapType &);

    void rEntity(DatabaseResult::const_iterator & dr);
    void rCharacter(DatabaseResult::const_iterator & dr);
  public:
    static Persistor<T> m_persist;

    void populate(DatabaseResult::const_iterator & dr);

    static Entity * restore(const std::string & id,
                            DatabaseResult::const_iterator & dr);

};

#endif // SERVER_RESTORER_H
