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

// $Id$

#include "tools/AdminClient.h"

#include "common/AtlasStreamClient.h"

#include <cassert>

class TestAdminClient : public AdminClient
{
  public:
    int test_checkRule(const std::string & id) {
        return checkRule(id);
    }

    void test_setErrorFlag(bool v = true) {
        error_flag = v;
    }
};

int main()
{
    {
        AdminClient ac;
    }

    {
        TestAdminClient ac;

        ac.test_checkRule("test_this_rule");
    }

    // check rule
    {
        TestAdminClient ac;

        ac.test_setErrorFlag();
        ac.test_checkRule("test_this_rule");
    }

    // new rule
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        ac.uploadRule("new_rule", "test_rule_set", rule);
    }

    // duplicate
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        ac.uploadRule("new_rule", "test_rule_set", rule);
        ac.uploadRule("new_rule", "test_rule_set", rule);
    }

    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        ac.test_setErrorFlag();
        ac.uploadRule("new_rule", "test_rule_set", rule);
    }

    return 0;
}

// stubs

using Atlas::Message::Element;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Root;

int AtlasStreamClient::authenticateLocal()
{
    return 0;
}

int AtlasStreamClient::linger()
{
    return 0;
}

void AtlasStreamClient::output(const Element & item, int depth) const
{
}

void AtlasStreamClient::objectArrived(const Root & obj)
{
}

void AtlasStreamClient::operation(const RootOperation & op)
{
}

void AtlasStreamClient::infoArrived(const RootOperation & op)
{
}

void AtlasStreamClient::appearanceArrived(const RootOperation & op)
{
}

void AtlasStreamClient::disappearanceArrived(const RootOperation & op)
{
}

void AtlasStreamClient::sightArrived(const RootOperation & op)
{
}

void AtlasStreamClient::soundArrived(const RootOperation & op)
{
}

/// \brief Called when an Error operation arrives
///
/// @param op Operation to be processed
void AtlasStreamClient::errorArrived(const RootOperation & op)
{
}

AtlasStreamClient::AtlasStreamClient() : reply_flag(false), error_flag(false),
                                         serialNo(512), m_fd(-1), m_encoder(0),
                                         m_codec(0), m_ios(0), m_currentTask(0)
{
}

AtlasStreamClient::~AtlasStreamClient()
{
}

void AtlasStreamClient::send(const RootOperation & op)
{
}

int AtlasStreamClient::login(const std::string & username,
                             const std::string & password)
{
    return 0;
}

int AtlasStreamClient::poll(int timeOut, int msec)
{
    return 0;
}
