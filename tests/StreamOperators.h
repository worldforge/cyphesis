/*
 Copyright (C) 2022 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef TESTS_STREAM_OPERATORS_H
#define TESTS_STREAM_OPERATORS_H

#include <Atlas/Message/Element.h>

#include <iostream>

//debug.cpp is linked to all tests, but we don't want to include debug.h since the "debug" macro creates conflicts.
template <typename T>
void debug_dump(const T & t, std::ostream &);

namespace Atlas
{
namespace Message
{
inline std::ostream& operator<<(std::ostream& os, const Atlas::Message::MapType& v)
{
    os << "[ATLAS_MAP]";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Atlas::Message::Element& e)
{
    debug_dump(e, os);
    return os;
}
}
}

#endif  // TESTS_STREAM_OPERATORS_H
