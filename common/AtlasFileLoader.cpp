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

// $Id: AtlasFileLoader.cpp,v 1.11 2007-11-15 02:07:04 alriddoch Exp $

#include "common/AtlasFileLoader.h"

#include "common/log.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Message/Element.h>
#include <Atlas/Codecs/XML.h>

using Atlas::Objects::Root;
using Atlas::Message::Element;
using Atlas::Message::MapType;

/// \brief Called from the base class when a complete message has been decoded
void AtlasFileLoader::objectArrived(const Root & obj)
{
    if (obj->isDefaultId()) {
        log(ERROR, "Object without ID read from file");
        return;
    }
    const std::string & id = obj->getId();
    if (m_messages.find(id) != m_messages.end()) {
        log(WARNING, String::compose("Duplicate object ID \"%1\" loaded.", id));
    }
    m_messages[id] = obj;
    ++m_count;
}

/// \brief AtlasFileLoader constructor
///
/// @param filename Name of the file to be loaded
/// @param m Data store for the data loaded from the file
AtlasFileLoader::AtlasFileLoader(const std::string & filename,
                                 std::map<std::string, Root> & m) :
                m_file(filename.c_str(), std::ios::in),
                m_count(0), m_messages(m)
{
    m_codec = new Atlas::Codecs::XML(m_file, *this);
}

AtlasFileLoader::~AtlasFileLoader()
{
    delete m_codec;
}
