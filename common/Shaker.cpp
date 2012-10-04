// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2012 Anthony Pesce
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Shaker.h"

#include "log.h"
#include "debug.h"
#include "globals.h"
#include "compose.hpp"

#include <wfmath/MersenneTwister.h>

#ifdef _WIN32
#undef DATADIR
#endif // _WIN32

#include <gcrypt.h>

#include <iostream>

#include <sys/stat.h>

#include <cassert>

extern "C" {
#ifdef HAVE_SYS_UTSNAME_H
    #include <sys/utsname.h>
#endif // HAVE_SYS_UTSNAME_H
    #include <sys/types.h>
#ifdef HAVE_SYS_WAIT_H
    #include <sys/wait.h>
#endif // HAVE_SYS_WAIT_H
    #include <signal.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdio.h>
}

static const char hex_table[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };


Shaker::Shaker(){
	
}

std::string Shaker::generate_salt(char * buffer[],size_t length){
	std::string salt;
	
	for (int i = 0; i < length; ++i) {
        unsigned char b = rng.randInt() & 0xff;
        salt.push_back(hex_table[b & 0xf]);
        salt.push_back(hex_table[(b & 0xf0) >> 4]);
    }
	
	
	return salt;
}


