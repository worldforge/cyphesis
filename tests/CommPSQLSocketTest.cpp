// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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

#include "server/CommPSQLSocket.h"

#include <cassert>

int main()
{
    return 0;
}

// stubs

#include "common/log.h"
#include "common/Database.h"
#include "common/globals.h"

CommSocket::CommSocket(boost::asio::io_service & svr) : m_io_service(svr) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
}

void log(LogLevel, const std::string & msg)
{
}

int Database::runMaintainance(int command)
{
    return 0;
}

int Database::launchNewQuery()
{
    return 0;
}

int Database::initConnection()
{
    return 0;
}

void Database::queryResult(ExecStatusType status)
{
}

void Database::queryComplete()
{
}

void Database::reportError()
{
}

void Database::shutdownConnection()
{
}

bool exit_flag = false;
