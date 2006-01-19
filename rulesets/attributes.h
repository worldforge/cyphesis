// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2002 Alistair Riddoch
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

#ifndef RULESETS_ATTRIBUTES_H
#define RULESETS_ATTRIBUTES_H

#include <inttypes.h>

static const uint32_t a_loc	= 1 << 0,	// Entity
                      a_pos	= 1 << 1,	// Entity
                      a_orient	= 1 << 2,	// Entity
                      a_bbox	= 1 << 3,	// Entity
                      a_status	= 1 << 4,	// Entity
                      a_type	= 1 << 5,	// Entity
                      a_cont	= 1 << 6,	// Entity
                      a_attr	= 1 << 7,	// Entity
                      a_name	= 1 << 8,	// Entity
                      a_mass	= 1 << 9,	// Entity
                      a_drunk	= 1 << 10,	// Character
                      a_sex	= 1 << 11,	// Character
                      a_food	= 1 << 12,	// Character
                      a_rwield	= 1 << 13,	// Character
                      a_line	= 1 << 14,	// Line
                      a_area	= 1 << 15,	// Area
                      a_fruit	= 1 << 16,	// Plant
                      a_terrain	= 1 << 17;	// World

static const int attr_count = 16;

#endif // RULESETS_ATTRIBUTES_H
