// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2003 Alistair Riddoch
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


#ifndef COMMON_SYSTEM_H
#define COMMON_SYSTEM_H

#include <cstdlib>

// These two will not be transmitted to our parent, so we don't need to
// convery any data
#define EXIT_CONFIG_ERROR     (EXIT_FAILURE)
#define EXIT_FORK_ERROR       (EXIT_FAILURE)
#define EXIT_SECURITY_ERROR   (EXIT_FAILURE)

// These exit status values might be passed back to our waiting parent, so we
// can embed information about the nature of the error.
#define EXIT_DATABASE_ERROR   (EXIT_FAILURE | 1 << 1)
#define EXIT_SOCKET_ERROR     (EXIT_FAILURE | 1 << 2)
#define EXIT_PORT_ERROR       (EXIT_FAILURE | 1 << 3)

// Magic number returned by the security check if everything is okay.
#define SECURITY_OKAY         (0xff7a64e1)

#include <string>

const std::string get_hostname();

unsigned int security_init();
unsigned int security_check();
unsigned int security_setup();
void reduce_priority(int);
void interactive_signals();
void daemon_signals();
int daemonise();
void running();

std::string create_session_username();

void hash_password(const std::string & pwd, const std::string & salt,
                   std::string & hash);

void encrypt_password(const std::string & pwd, std::string & hash);
int check_password(const std::string & pwd, const std::string & hash);

int getfiletime(const std::string & filename, time_t & t);

void getinstallprefix();

#ifdef _WIN32

// int gettimeofday(struct timeval * tv, struct timezone * tz);

#else // _WIN32

#include <unistd.h>

#endif // _WIN32

#endif // COMMON_SYSTEM_H
