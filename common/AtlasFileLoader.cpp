// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "common/AtlasFileLoader.h"

#include "common/log.h"

#include <Atlas/Message/Element.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

void AtlasFileLoader::messageArrived(const MapType & msg)
{
    MapType o = msg;
    MapType::iterator I = o.find("id");
    if (I == o.end()) {
        log(WARNING, "Message without ID read from file");
        return;
    }
    Element & id = I->second;
    if (!id.isString()) {
        log(WARNING, "Message without non-string ID read from file");
        return;
    }
    std::string msg_id = id.asString();
    o.erase(I);
    m_messages[msg_id] = o;
    ++m_count;
}

AtlasFileLoader::AtlasFileLoader(const std::string & filename,
                                 MapType & m) :
                m_file(filename.c_str(), std::ios::in),
                m_codec(m_file, *this), m_count(0), m_messages(m)
{
}
