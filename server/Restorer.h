// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: Restorer.h,v 1.20 2006-10-26 00:48:15 alriddoch Exp $

#ifndef SERVER_RESTORER_H
#define SERVER_RESTORER_H

#include "common/Database.h"

#include <string>

class Entity;

template <class T>
class Persistor;

/// \brief Class template to handle restoring an entity of the parameter
/// class from the database.
///
/// This class should never ever be instantiated, so the constructor is private
/// and unimplemented. Instead the template should be instantiated with
/// T as the class to be restored, and a reference to the object being
/// restored is cast to a reference to this type, allowing code in
/// this class to write to the protected methods.
/// This class is probably going to want to directly decode data from the
/// database layer.
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

    void rEntity(DatabaseResult::const_iterator & dr);
    void rCharacter(DatabaseResult::const_iterator & dr);
    void rPlant(DatabaseResult::const_iterator & dr);
  public:
    static Persistor<T> m_persist;

    void populate(DatabaseResult::const_iterator & dr);

    static Entity * restore(const std::string & id, long intId,
                            DatabaseResult::const_iterator & dr);

};

#endif // SERVER_RESTORER_H
