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

#include "tools/Interactive.h"
#include "tools/AvatarContext.h"

#include "common/compose.hpp"

#include <Atlas/Objects/RootOperation.h>

#include <cassert>

using boost::shared_ptr;

const std::string data_path = TESTDATADIR;

int main()
{
    Interactive client;
    shared_ptr<ObjectContext> avatar_context(new AvatarContext(client, "42")); 

    WorldLoader * wl = new WorldLoader("23", avatar_context);
    delete wl;

    OpVector res;
    wl = new WorldLoader("23", avatar_context);
    wl->setup(String::compose("%1/no_such_file", data_path), res);
    assert(wl->isComplete());
    assert(res.empty());
    delete wl;

    res.clear();
    wl = new WorldLoader("23", avatar_context);
    wl->setup(String::compose("%1/world.xml", data_path), res);
    assert(!wl->isComplete());
    assert(!res.empty());
    delete wl;
}

// stubs

namespace Atlas { namespace Objects { namespace Operation {
int CONNECT_NO = -1;
int MONITOR_NO = -1;
} } }

#include "tools/ConnectionContext.h"
#include "tools/Flusher.h"
#include "tools/JunctureContext.h"
#include "tools/OperationMonitor.h"
#include "tools/WorldDumper.h"

#include "common/log.h"

using Atlas::Message::Element;
using Atlas::Objects::Root;

AtlasStreamClient::AtlasStreamClient() : reply_flag(false), error_flag(false),
                                         serialNo(512), m_fd(-1), m_encoder(0),
                                         m_codec(0), m_ios(0), m_currentTask(0),
                                         m_spacing(2)
{
}

AtlasStreamClient::~AtlasStreamClient()
{
}

void AtlasStreamClient::output(const Element & item, int depth) const
{
}

void AtlasStreamClient::output(const Root & ent) const
{
}

void AtlasStreamClient::objectArrived(const Root & obj)
{
}

void AtlasStreamClient::operation(const Operation & op)
{
}

void AtlasStreamClient::infoArrived(const Operation & op)
{
}

void AtlasStreamClient::appearanceArrived(const Operation & op)
{
}

void AtlasStreamClient::disappearanceArrived(const Operation & op)
{
}

void AtlasStreamClient::sightArrived(const Operation & op)
{
}

void AtlasStreamClient::soundArrived(const Operation & op)
{
}

void AtlasStreamClient::loginSuccess(const Atlas::Objects::Root & arg)
{
}

void AtlasStreamClient::errorArrived(const Operation & op)
{
}

void AtlasStreamClient::send(const Operation & op)
{
}

int AtlasStreamClient::runTask(ClientTask * task, const std::string & arg)
{
    return 0;
}

int AtlasStreamClient::endTask()
{
    return 0;
}

int AtlasStreamClient::cleanDisconnect()
{
    return 0;
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

ConnectionContext::ConnectionContext(Interactive & i) : ObjectContext(i),
                                                        m_refNo(0L)
{
}

bool ConnectionContext::accept(const Operation& op) const
{
    return false;
}

int ConnectionContext::dispatch(const Operation & op)
{
    return 0;
}

std::string ConnectionContext::repr() const
{
    return "";
}

bool ConnectionContext::checkContextCommand(const struct command *)
{
    return false;
}

void ConnectionContext::setFromContext(const Operation & op)
{
}

Flusher::Flusher(const shared_ptr<ObjectContext> & c) : m_context(c)
{
}

Flusher::~Flusher()
{
}

void Flusher::setup(const std::string & arg, OpVector & ret)
{
}

void Flusher::operation(const Operation & op, OpVector & res)
{
}

JunctureContext::JunctureContext(Interactive & i,
                                 const std::string & id) : IdContext(i, id)
{
}

bool JunctureContext::accept(const Operation& op) const
{
    return false;
}

int JunctureContext::dispatch(const Operation & op)
{
    return 0;
}

std::string JunctureContext::repr() const
{
    return "junc";
}

bool JunctureContext::checkContextCommand(const struct command *)
{
    return false;
}

OperationMonitor::~OperationMonitor()
{
}

void OperationMonitor::setup(const std::string & arg, OpVector &)
{
}

void OperationMonitor::operation(const Operation & op, OpVector &)
{
}

WorldDumper::WorldDumper(const std::string & accountId) : m_account(accountId),
                                                          m_lastSerialNo(-1),
                                                          m_count(0),
                                                          m_codec(0),
                                                          m_encoder(0),
                                                          m_formatter(0)
{
}

WorldDumper::~WorldDumper()
{
}

void WorldDumper::setup(const std::string & arg, OpVector & res)
{
}

void WorldDumper::operation(const Operation & op, OpVector & res)
{
}

void log(LogLevel lvl, const std::string & msg)
{
}

void tokenize(const std::string& str,
              std::vector<std::string>& tokens,
              const std::string& delimiters)
{
}
