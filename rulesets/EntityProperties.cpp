// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "common/Property_impl.h"
#include "common/types.h"

#include "physics/BBox.h"

#include <wfmath/atlasconv.h>

template class Property<BBox>;

template class ImmutableProperty<EntitySet>;

void Property<BBox>::get(Atlas::Message::Element & e)
{
    e = m_data.toAtlas();
}

void Property<BBox>::set(const Atlas::Message::Element & e)
{
    if (e.isList() && (e.asList().size() > 2)) {
        m_data.fromAtlas(e.asList());
    }
}

void Property<BBox>::add(const std::string & s, Atlas::Message::MapType & ent)
{
    if (m_data.isValid()) {
        ent[s] = m_data.toAtlas();
    }
}

void ImmutableProperty<EntitySet>::get(Atlas::Message::Element & e)
{
    e = ListType();
    ListType & contlist = e.asList();
    for (EntitySet::const_iterator I = m_data.begin();
         I != m_data.end();
         I++) {
        contlist.push_back(*I);
    }
}

void ImmutableProperty<EntitySet>::set(const Atlas::Message::Element & e)
{
    return;
}

void ImmutableProperty<EntitySet>::add(const std::string & s,
                                       Atlas::Message::MapType & ent)
{
    if (!m_data.empty()) {
        get(ent[s]);
    }
}
