// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_SYSTEM_H
#define COMMON_SYSTEM_H

#include <cstdlib>

// These two will not be transmitted to our parent, so we don't need to
// convery any data
#define EXIT_CONFIG_ERROR     (EXIT_FAILURE)
#define EXIT_FORK_ERROR       (EXIT_FAILURE)

// These exit status values might be passed back to our waiting parent, so we
// can embed information about the nature of the error.
#define EXIT_DATABASE_ERROR   (EXIT_FAILURE | 1 << 1)
#define EXIT_SOCKET_ERROR     (EXIT_FAILURE | 2 << 1)

#include <string>

const std::string get_hostname();
void interactive_signals();
void daemon_signals();
int daemonise();
void running();
void encrypt_password(const std::string & pwd, std::string & hash);

#endif // COMMON_SYSTEM_H
