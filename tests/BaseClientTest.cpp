// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "client/BaseClient.h"

#include <Atlas/Objects/RootOperation.h>

#include <cassert>

using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;

Atlas::Objects::Factories factories;

class TestBaseClient : public BaseClient
{
  public:
    TestBaseClient(boost::asio::io_context& io_context) : BaseClient(io_context, factories) { }

    virtual void idle() { }
};

int main()
{
    boost::asio::io_context io_context;
    {
        BaseClient * bc = new TestBaseClient{io_context};

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient{io_context};

        bc->createAccount("8e7e4452-f666-11df-8027-00269e5444b3", "84abee0c-f666-11df-8f7e-00269e5444b3");

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient{io_context};

        bc->createSystemAccount();

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient{io_context};

        bc->createCharacter("9e7f4004-f666-11df-a327-00269e5444b3");

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient{io_context};

        bc->logout();

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient{io_context};

        bc->handleNet();

        delete bc;
    }

    return 0;
}

// stubs

#include "client/CreatorClient.h"

#include "common/log.h"

#include <cstdlib>

#include "stubs/rules/ai/stubBaseMind.h"
#include "stubs/client/stubCreatorClient.h"

#define STUB_ClientConnection_pop
RootOperation ClientConnection::pop()
{
    return RootOperation(nullptr);
}

#include "stubs/client/stubClientConnection.h"
#include "stubs/client/stubCharacterClient.h"
#include "stubs/rules/ai/stubMemMap.h"
#include "stubs/common/stubAtlasStreamClient.h"

void log(LogLevel lvl, const std::string & msg)
{
}

std::string create_session_username()
{
    return "admin_test";
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}




#include "stubs/rules/ai/stubMemEntity.h"
#include "stubs/rules/stubLocatedEntity.h"
#include "stubs/common/stubRouter.h"
#include "stubs/rules/stubLocation.h"

void WorldTime::initTimeInfo()
{
}

DateTime::DateTime(int t)
{
}
