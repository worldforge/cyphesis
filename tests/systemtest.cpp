// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "common/globals.h"
#include "common/system.h"

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int main()
{
    std::string hostname = get_hostname();
    assert(!hostname.empty());

    pid_t pid = getpid();
    assert(!exit_flag);

    // Verify that most of these flag shutdown, except SIGPIPE
    interactive_signals();

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

    // Verify that most of these are ignored, except SIGTERM
    daemon_signals();

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

    // Check the background mechanism

    daemon_flag = true;

    int child = daemonise();

    assert(child != -1);

    if (child == 0) {
        sleep(1);
        running();
        sleep(1);
        return 0;
    }

    std::string test_password("test_password");
    std::string test_hash;

    encrypt_password(test_password, test_hash);

    assert(!test_hash.empty());
    assert(test_hash != test_password);

    return 0;
}
