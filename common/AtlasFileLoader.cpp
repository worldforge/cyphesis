// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "common/AtlasFileLoader.h"

#include "common/log.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

void AtlasFileLoader::objectArrived(const Element & obj)
{
    if (!obj.isMap()) {
        log(WARNING, "Non-map object read from file");
        return;
    }
    MapType o = obj.asMap();
    MapType::iterator I = o.find("id");
    if (I == o.end()) {
        log(WARNING, "Object without ID read from file");
        return;
    }
    Element & id = I->second;
    if (!id.isString()) {
        log(WARNING, "Object without non-string ID read from file");
        return;
    }
    std::string objId = id.asString();
    o.erase(I);
    m_objects[objId] = o;
    ++m_count;
}

AtlasFileLoader::AtlasFileLoader(const std::string & filename,
                                 MapType & m) :
                m_file(filename.c_str(), std::ios::in),
                m_codec(m_file, this), m_count(0), m_objects(m)
{
}
