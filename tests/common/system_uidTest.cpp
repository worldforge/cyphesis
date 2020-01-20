// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "common/globals.h"
#include "common/system.h"
#include "common/log.h"

#include <iostream>

#include <cassert>

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

static const uid_t SAFE_UID = 17;

static uid_t test_uid = SAFE_UID;
static uid_t test_euid = SAFE_UID;

static bool test_error_logged = false;

int main()
{
    unsigned magic_res = security_check();
    assert(magic_res == SECURITY_OKAY);
    assert(test_error_logged == false);

    test_uid = 0;
    magic_res = security_check();
    assert(magic_res != SECURITY_OKAY);
    assert(test_error_logged == true);
    test_uid = SAFE_UID;


    test_error_logged = false;
    test_euid = 0;
    magic_res = security_check();
    assert(magic_res != SECURITY_OKAY);
    assert(test_error_logged == true);

    std::string session_name = create_session_username();
    assert(!session_name.empty());
}

extern "C" uid_t getuid()
{
    return test_uid;
}

extern "C" uid_t geteuid()
{
    return test_euid;
}

void log(LogLevel lvl, const std::string & msg)
{
    if (lvl == CYLOG_ERROR) {
        test_error_logged = true;
    }
}

void rotateLogger()
{
}