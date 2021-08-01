// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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


#include "common/AtlasFileLoader.h"

#include "common/log.h"
#include "common/compose.hpp"
#include "common/debug.h"

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Factories.h>
#include <Atlas/Codecs/XML.h>

using Atlas::Objects::Root;

/// \brief Called from the base class when a complete message has been decoded
void AtlasFileLoader::objectArrived(Root obj)
{
    if (obj->isDefaultId()) {
        log(ERROR, String::compose("Object without ID read from file %1", m_filename));
        std::stringstream ss;
        debug_dump(obj, ss);
        log(ERROR, "Object: " + ss.str());
        return;
    }
    const std::string& id = obj->getId();
    if (m_messages.find(id) != m_messages.end()) {
        log(WARNING, String::compose("Duplicate object ID \"%1\" loaded from file %2.", id, m_filename));
    }
    m_messages[id] = std::move(obj);
    ++m_count;
}

/// \brief AtlasFileLoader constructor
///
/// @param filename Name of the file to be loaded
/// @param m Data store for the data loaded from the file
AtlasFileLoader::AtlasFileLoader(const Atlas::Objects::Factories& factories,
                                 const std::string& filename,
                                 std::map<std::string, Root>& m) :
    ObjectsDecoder(factories),
    m_file(filename.c_str(), std::ios::in),
    m_count(0), m_messages(m), m_filename(filename)
{
    m_codec = std::make_unique<Atlas::Codecs::XML>(m_file, m_file, *this);
}

AtlasFileLoader::~AtlasFileLoader() = default;

/// Indicate if the input file has been opened successfully
bool AtlasFileLoader::isOpen()
{
    return m_file.is_open();
}

/// Read input file to atlas codec.
void AtlasFileLoader::read()
{
    while (!m_file.eof()) {
        m_codec->poll();
    }
}

