// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#ifndef RULESETS_ATTRIBUTES_H
#define RULESETS_ATTRIBUTES_H

#include <inttypes.h>

static const uint32_t a_loc	= 1 << 0,	// Entity
                      a_pos	= 1 << 1,	// Entity
                      a_orient	= 1 << 2,	// Entity
                      a_bbox	= 1 << 3,	// Entity
                      a_status	= 1 << 4,	// Entity
                      a_type	= 1 << 5,	// Entity
                      a_attr	= 1 << 6,	// Entity
                      a_name	= 1 << 7,	// Entity
                      a_mass	= 1 << 8,	// Entity
                      a_drunk	= 1 << 9,	// Character
                      a_sex	= 1 << 10,	// Character
                      a_food	= 1 << 11,	// Character
                      a_line	= 1 << 12,	// Line
                      a_area	= 1 << 13,	// Area
                      a_fruit	= 1 << 14;	// Plant

static const int attr_count = 15;

#endif // RULESETS_ATTRIBUTES_H
