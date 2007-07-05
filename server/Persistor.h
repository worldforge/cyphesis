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

// $Id: Persistor.h,v 1.14 2007-07-05 17:51:41 alriddoch Exp $

#ifndef SERVER_PERSISTOR_H
#define SERVER_PERSISTOR_H

#include <sigc++/trackable.h>

#include <string>

class Entity;
class Character;
class Line;
class Area;
class Plant;

/// \brief Class template for handling persisting entity attributes in
/// the database
template <class T>
class Persistor : virtual public sigc::trackable {
  private:
    const std::string m_class;

    void uEntity(Entity &, std::string &);
    void uCharacter(Character &, std::string &);
    void uPlant(Plant &, std::string &);

    void cEntity(Entity &, std::string &, std::string &);
    void cCharacter(Character &, std::string &, std::string &);
    void cPlant(Plant &, std::string &, std::string &);
  public:
    // Register table with database
    explicit Persistor(bool temp = false);

    void hookup(T &);
    void persist(T &);
    void update(T *);
    void remove(T *);
};

#endif // SERVER_PERSISTOR_H
