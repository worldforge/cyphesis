// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "common/ScriptKit.h"

class LocatedEntity;

class TestScriptKit : public ScriptKit<LocatedEntity>
{
  public:
    std::string m_package;
    virtual const std::string & package() const { return m_package; }
    virtual int addScript(LocatedEntity & entity) const { return 0; }
    virtual int refreshClass() { return 0; }
};

int main()
{
    // The is no code in ScriptKit.cpp to execute, but we need coverage.
    {
        ScriptKit<LocatedEntity> * sk = new TestScriptKit;

        delete sk;
    }
    return 0;
}
