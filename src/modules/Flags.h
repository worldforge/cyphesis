/*
 Copyright (C) 2018 Erik Ogenvik

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

#ifndef CYPHESIS_FLAGS_H
#define CYPHESIS_FLAGS_H

#include <cinttypes>

/**
 * Simple struct for handling binary flags.
 */
struct Flags
{
    std::uint32_t m_flags;

    explicit Flags(std::uint32_t flags) : m_flags(flags)
    {}

    void addFlags(std::uint32_t flags)
    {
        m_flags |= flags;
    }

    void removeFlags(std::uint32_t flags)
    {
        m_flags &= ~flags;
    }

    bool hasFlags(std::uint32_t flags) const
    {
        return (m_flags & flags) != 0;
    }
};

#endif //CYPHESIS_FLAGS_H
