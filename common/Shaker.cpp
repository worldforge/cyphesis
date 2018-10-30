// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Anthony Pesce
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


#include "Shaker.h"

static const char hex_table[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
Shaker::Shaker() = default;

/// \brief Generates salt by using 
/// a Random Number Generator of type WFMath::MTRand
/// @param length The amount to iterate to generate the salt. 
std::string Shaker::generateSalt(size_t length)
{
    std::string salt;
	
    for (size_t i = 0; i < length; ++i) 
    {
        auto b = static_cast<unsigned char>(rng.randInt() & 0xff);
        salt.push_back(hex_table[b & 0xf]);
        salt.push_back(hex_table[(b & 0xf0) >> 4]);
    }
    return salt;
}
