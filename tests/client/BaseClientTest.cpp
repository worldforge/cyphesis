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

#include "client/cyclient/BaseClient.h"
#include "../NullPropertyManager.h"
#include "rules/SimpleTypeStore.h"

#include <Atlas/Objects/RootOperation.h>

#include <cassert>

using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;

Atlas::Objects::Factories factories;

class TestBaseClient : public BaseClient
{
  public:
    explicit TestBaseClient(boost::asio::io_context& io_context, TypeStore& typeStore) : BaseClient(io_context, factories, typeStore) { }

    void idle() override { }
};

int main()
{
    NullPropertyManager propertyManager;
    SimpleTypeStore typeStore(propertyManager);
    boost::asio::io_context io_context;
    {
        BaseClient * bc = new TestBaseClient{io_context, typeStore};

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient{io_context, typeStore};

        bc->createAccount("8e7e4452-f666-11df-8027-00269e5444b3", "84abee0c-f666-11df-8f7e-00269e5444b3");

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient{io_context, typeStore};

        bc->createSystemAccount();

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient{io_context, typeStore};

        bc->createCharacter("9e7f4004-f666-11df-a327-00269e5444b3");

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient{io_context, typeStore};

        bc->logout();

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient{io_context, typeStore};

        bc->handleNet();

        delete bc;
    }

    return 0;
}

// stubs

#include "client/cyclient/CreatorClient.h"

#include "common/log.h"

#include <cstdlib>

#include "../stubs/rules/ai/stubBaseMind.h"
#include "../stubs/rules/stubSimpleTypeStore.h"
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/client/cyclient/stubCreatorClient.h"

#define STUB_ClientConnection_pop
RootOperation ClientConnection::pop()
{
    return RootOperation(nullptr);
}

#include "../stubs/client/cyclient/stubClientConnection.h"
#include "../stubs/client/cyclient/stubCharacterClient.h"
#include "../stubs/rules/ai/stubMemMap.h"
#include "../stubs/common/stubAtlasStreamClient.h"
#include "../stubs/common/stublog.h"
#include "../stubs/common/stubProperty.h"
#include "../stubs/common/stubPropertyManager.h"

std::string create_session_username()
{
    return "admin_test";
}

#include "../stubs/common/stubid.h"
#include "../stubs/rules/ai/stubMemEntity.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/common/stubcustom.h"
#include "../stubs/rules/stubLocation.h"
