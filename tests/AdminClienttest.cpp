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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "tools/AdminClient.h"

#include "common/AtlasStreamClient.h"

#include <iostream>
#include <queue>

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

enum srvop { SRV_INFO, SRV_ERROR };

std::queue<enum srvop> stub_poll_sequence;

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

        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_INFO);
        ac.test_checkRule("test_this_rule");
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // new rule with no parents - fails
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_ERROR);
        int res = ac.uploadRule("new_rule", "test_rule_set", rule);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // new rule with non list parents - fails
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        rule["parents"] = "non_list_parents";
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_ERROR);
        int res = ac.uploadRule("new_rule", "test_rule_set", rule);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // new rule with empty parents - fails
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        rule["parents"] = Atlas::Message::ListType();
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_ERROR);
        int res = ac.uploadRule("new_rule", "test_rule_set", rule);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // new rule with parents - upload fails
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        rule["parents"] = Atlas::Message::ListType(1, "new_rule");
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_ERROR); // Check existence of new rule
        stub_poll_sequence.push(SRV_INFO); // Check existence of parent
        stub_poll_sequence.push(SRV_ERROR); // Create new rule
        int res = ac.uploadRule("new_rule", "test_rule_set", rule);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // new rule with parents - upload succeeds
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        rule["parents"] = Atlas::Message::ListType(1, "new_rule");
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_ERROR); // Check existence of new rule
        stub_poll_sequence.push(SRV_INFO); // Check existence of parent
        stub_poll_sequence.push(SRV_INFO); // Create new rule
        int res = ac.uploadRule("new_rule", "test_rule_set", rule);
        assert(res == 1);
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // new rule with parents - upload succeeds, and re-upload same should fail
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        rule["parents"] = Atlas::Message::ListType(1, "new_rule");
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_ERROR); // Check existence of new rule
        stub_poll_sequence.push(SRV_INFO); // Check existence of parent
        stub_poll_sequence.push(SRV_INFO); // Create new rule
        int res = ac.uploadRule("new_rule", "test_rule_set", rule);
        assert(res == 1);
        stub_poll_sequence = std::queue<enum srvop>();

        stub_poll_sequence.push(SRV_ERROR); // Check existence of new rule
        stub_poll_sequence.push(SRV_INFO); // Check existence of parent
        stub_poll_sequence.push(SRV_INFO); // Create new rule
        res = ac.uploadRule("new_rule", "test_rule_set", rule);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // new rule with parents - parent rule not uploaded yet
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        rule["parents"] = Atlas::Message::ListType(1, "new_rule");
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_ERROR); // Check existence of new rule
        stub_poll_sequence.push(SRV_ERROR); // Check existence of parent
        int res = ac.uploadRule("new_rule", "test_rule_set", rule);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // new rule with parents - parent rule not uploaded yet, and second
    //                         rule queueed on same parent
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        rule["parents"] = Atlas::Message::ListType(1, "new_rule");
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_ERROR); // Check existence of new rule
        stub_poll_sequence.push(SRV_ERROR); // Check existence of parent
        int res = ac.uploadRule("rule1", "test_rule_set", rule);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();
        stub_poll_sequence.push(SRV_ERROR); // Check existence of new rule
        stub_poll_sequence.push(SRV_ERROR); // Check existence of parent
        res = ac.uploadRule("rule2", "test_rule_set", rule);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // new rule with parents - parent rule not uploaded yet, and try same
    //                         rule twice
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        rule["parents"] = Atlas::Message::ListType(1, "new_rule");
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_ERROR); // Check existence of new rule
        stub_poll_sequence.push(SRV_ERROR); // Check existence of parent
        int res = ac.uploadRule("rule1", "test_rule_set", rule);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();
        stub_poll_sequence.push(SRV_ERROR); // Check existence of new rule
        stub_poll_sequence.push(SRV_ERROR); // Check existence of parent
        res = ac.uploadRule("rule1", "test_rule_set", rule);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // new rule with parents - parent rule not uploaded yet, and try same
    //                         rule twice
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule1;
        rule1["parents"] = Atlas::Message::ListType(1, "rule2");
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_ERROR); // Check existence of new rule
        stub_poll_sequence.push(SRV_ERROR); // Check existence of parent
        int res = ac.uploadRule("rule1", "test_rule_set", rule1);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();

        Atlas::Message::MapType rule2;
        rule2["parents"] = Atlas::Message::ListType(1, "old_rule");
        stub_poll_sequence.push(SRV_ERROR); // Check existence of new rule
        stub_poll_sequence.push(SRV_INFO); // Check existence of parent
        stub_poll_sequence.push(SRV_INFO); // Create rule2
        stub_poll_sequence.push(SRV_ERROR); // Check existence of waiting rule
        stub_poll_sequence.push(SRV_INFO); // Check existence of waiting rule parent
        stub_poll_sequence.push(SRV_INFO); // Create rule1
        res = ac.uploadRule("rule2", "test_rule_set", rule2);
        std::cerr << "res: " << res << std::endl;
        assert(res == 2);
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // Updating existing rule succeeds
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_INFO);
        stub_poll_sequence.push(SRV_INFO);
        int res = ac.uploadRule("new_rule", "test_rule_set", rule);
        assert(res == 0);
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // Updating existing rule fails
    {
        TestAdminClient ac;

        Atlas::Message::MapType rule;
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_INFO);
        stub_poll_sequence.push(SRV_ERROR);
        int res = ac.uploadRule("new_rule", "test_rule_set", rule);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();
    }

    // FIXME cover a bunch more of uploadRule()

    {
        AdminClient ac;

        ac.login();
    }

    {
        AdminClient ac;

        ac.report();
    }

    {
        AdminClient ac;

        Atlas::Message::MapType rule;
        rule["parents"] = Atlas::Message::ListType(1, "new_rule");
        assert(stub_poll_sequence.empty());
        stub_poll_sequence.push(SRV_ERROR); // Check existence of new rule
        stub_poll_sequence.push(SRV_ERROR); // Check existence of parent
        int res = ac.uploadRule("new_rule", "test_rule_set", rule);
        assert(res == -1);
        stub_poll_sequence = std::queue<enum srvop>();

        ac.report();
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
    reply_flag = false;
    error_flag = false;
}

int AtlasStreamClient::login(const std::string & username,
                             const std::string & password)
{
    return 0;
}

int AtlasStreamClient::poll(int timeOut, int msec)
{
    enum srvop stub_poll_returns = SRV_ERROR;
    if (!stub_poll_sequence.empty()) {
        stub_poll_returns = stub_poll_sequence.front();
        stub_poll_sequence.pop();
    }
    switch (stub_poll_returns) {
      case SRV_INFO:
        reply_flag = true;
        error_flag = false;
        break;
      case SRV_ERROR:
      default:
        reply_flag = true;
        error_flag = true;
        break;
    }
    return 0;
}
