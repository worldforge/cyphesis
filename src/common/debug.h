// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2001 Alistair Riddoch
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


#ifndef COMMON_DEBUG_H
#define COMMON_DEBUG_H

#include <iostream>
#include <string>

namespace Atlas { namespace Message { class Element; } }

#define debug(prg) { if (debug_flag) { prg } }

#define debug_func() {\
    if (debug_flag) { \
        std::cerr << __PRETTY_FUNCTION__ << std::endl << std::flush;\
    }\
}

/**
 * Prints a string to standard err output.
 *
 * The text will be passed to std::cerr, so it's possible to chain the string.
 * An example:
 *
 * debug_print("some text" << someValue << "some more text);
 *
 * @param text The text.
 */
#define debug_print(text) { if (debug_flag) { \
        std::cerr << text << std::endl << std::flush;\
} }

/**
 * Prints a string to standard err output, appending with the calling function.
 *
 * The text will be passed to std::cerr, so it's possible to chain the string.
 * An example:
 *
 * debug_print("some text" << someValue << "some more text);
 *
 * @param text The text.
 */
#define debug_print_pretty(text) { if (debug_flag) { \
        std::cerr << text << "     :: " << __PRETTY_FUNCTION__ << std::endl << std::flush;\
} }

void output_element(std::ostream & out,
                    const Atlas::Message::Element & item,
                    size_t depth);

template <typename T>
void debug_dump(const T & t, std::ostream &);

template <typename T>
std::string debug_tostring(const T & t);

#endif // COMMON_DEBUG_H
