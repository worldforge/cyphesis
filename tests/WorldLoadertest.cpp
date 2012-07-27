// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "tools/WorldLoader.h"

#include "tools/ObjectContext.h"

#include "common/compose.hpp"

#include <Atlas/Objects/RootOperation.h>

#include <cassert>

static bool stub_isOpen = true;

const std::string data_path = TESTDATADIR;

class test_ObjectContext : public ObjectContext {
  public:
    std::string m_id;
    test_ObjectContext(const std::string & id) :
          ObjectContext(*(Interactive*)0), m_id(id) { }
    virtual bool accept(const Atlas::Objects::Operation::RootOperation&) const
    {
        return false;
    }

    virtual int dispatch(const Atlas::Objects::Operation::RootOperation&)
    {
        return 0;
    }

    virtual std::string repr() const
    {
        return "test_context";
    }

    virtual bool checkContextCommand(const struct command *)
    {
        return false;
    }

    virtual void setFromContext(const Atlas::Objects::Operation::RootOperation&op)
    {
        op->setFrom(m_id);
    }
};

using boost::shared_ptr;

int main()
{
    shared_ptr<ObjectContext> test_context(new test_ObjectContext("42"));

    WorldLoader * wl = new WorldLoader("23", test_context);
    delete wl;

    OpVector res;
    wl = new WorldLoader("23", test_context);
    stub_isOpen = true;
    wl->setup(String::compose("%1/no_such_file", data_path), res);
    delete wl;

    res.clear();
    wl = new WorldLoader("23", test_context);
    stub_isOpen = false;
    wl->setup(String::compose("%1/world.xml", data_path), res);
    delete wl;
}

// stubs

#include "common/AtlasFileLoader.h"
#include "common/log.h"

ObjectContext::~ObjectContext()
{
}

AtlasFileLoader::AtlasFileLoader(const std::string & filename,
      std::map<std::string, Atlas::Objects::Root> & m) : m_messages(m)
{
}

AtlasFileLoader::~AtlasFileLoader()
{
}

void AtlasFileLoader::objectArrived(const Atlas::Objects::Root & obj)
{
}

bool AtlasFileLoader::isOpen()
{
    return stub_isOpen;
}

/// Read input file to atlas codec.
void AtlasFileLoader::read()
{
}


void log(LogLevel lvl, const std::string & msg)
{
}
