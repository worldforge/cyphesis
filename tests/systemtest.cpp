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

// $Id: systemtest.cpp,v 1.6 2006-11-03 18:55:41 alriddoch Exp $

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "common/globals.h"
#include "common/system.h"

#include <skstream/sksocket.h>

#include <iostream>

#include <cassert>

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main()
{
    std::string hostname = get_hostname();
    assert(!hostname.empty());

    pid_t pid = getpid();
    assert(!exit_flag);

    // Verify that most of these flag shutdown, except SIGPIPE
    interactive_signals();

#ifdef HAVE_KILL
    exit_flag = false;
    kill(pid, SIGINT);
    assert(exit_flag);

    exit_flag = false;
    kill(pid, SIGTERM);
    assert(exit_flag);

    exit_flag = false;
    kill(pid, SIGQUIT);
    assert(exit_flag);

    exit_flag = false;
    kill(pid, SIGHUP);
    assert(exit_flag);

    exit_flag = false;
    kill(pid, SIGPIPE);
    assert(!exit_flag);
#endif // HAVE_KILL

    // Verify that most of these are ignored, except SIGTERM
    daemon_signals();

#ifdef HAVE_KILL
    exit_flag = false;
    kill(pid, SIGINT);
    assert(!exit_flag);

    exit_flag = false;
    kill(pid, SIGTERM);
    assert(exit_flag);

    exit_flag = false;
    kill(pid, SIGQUIT);
    assert(!exit_flag);

    exit_flag = false;
    kill(pid, SIGHUP);
    assert(!exit_flag);

    exit_flag = false;
    kill(pid, SIGPIPE);
    assert(!exit_flag);
#endif // HAVE_KILL

    // Check the background mechanism

#ifdef HAVE_FORK
    daemon_flag = true;

    int child = daemonise();

    assert(child != -1);

    if (child == 0) {
        sleep(1);
        running();
        sleep(1);
        return 0;
    }
#endif

    const std::string test_password("test_password");
    std::string test_hash;

    encrypt_password(test_password, test_hash);

    assert(!test_hash.empty());
    assert(test_hash != test_password);

    assert(check_password(test_password, test_hash) == 0);
    assert(check_password(test_password, "61CEE1BB10EF20ED9D7B5D44D7D3CF56") == 0);
    assert(check_password("zjvspoehrgopes", "247E9405E40979403510799CBBFF88BD") == 0);
    assert(check_password("foobarbaz", test_hash) != 0);

#ifndef HAVE_GETTIMEOFDAY
#warning you got it

    struct timeval time;
    assert(gettimeofday(&time, 0) == 0);

    std::cout << time.tv_sec << "." << time.tv_usec << std::endl << std::flush;

    return 0;
#endif // HAVE_GETTIMEOFDAY
}
