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


#ifndef RULESETS_MIND_FACTORY_H
#define RULESETS_MIND_FACTORY_H

#include <string>

class BaseMind;

template<class T>
class ScriptKit;

class MindKit {
  protected:
    MindKit();

  public:
    ScriptKit<BaseMind> * m_scriptFactory;

    virtual ~MindKit() = 0;

    virtual BaseMind * newMind(const std::string & id, long) const = 0;
};

/// \brief Factory class for creating minds
///
/// A mind consists of a C++ base class and an associated script object
class MindFactory : public MindKit {
  public:
    virtual ~MindFactory();

    BaseMind * newMind(const std::string &, long) const;
};

#endif // RULESETS_MIND_FACTORY_H
