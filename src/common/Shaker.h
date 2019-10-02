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


#ifndef COMMON_SHAKER_H
#define COMMON_SHAKER_H

#include <wfmath/MersenneTwister.h>

#include <string>

/// \brief This is a class to generate a salt for a hash
class Shaker
{
  public:
    Shaker();
    /// \brief Function to generate salt
    std::string generateSalt(size_t length);	
  private:
    /// \brief MTRand Random Number Generator
    WFMath::MTRand rng;	
};

#endif //COMMON_SHAKER_H
