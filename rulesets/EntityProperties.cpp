// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "Entity.h"

#include "common/Property_impl.h"
#include "common/type_utils.h"

#include "physics/BBox.h"

#include <wfmath/atlasconv.h>

template class Property<BBox>;
template class Property<IdList>;

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

void Property<IdList>::get(Atlas::Message::Element & e)
{
    e = ListType();
    idListasObject(m_data, e.asList());
}

void Property<IdList>::set(const Atlas::Message::Element & e)
{
    if (e.isList()) {
        idListFromAtlas(e.asList(), m_data);
    }
}

void Property<IdList>::add(const std::string & s, Atlas::Message::MapType & ent)
{
    if (!m_data.empty()) {
        get(ent["s"]);
    }
}

void ImmutableProperty<EntitySet>::get(Atlas::Message::Element & e)
{
    e = ListType();
    ListType & contlist = e.asList();
    EntitySet::const_iterator Iend = m_data.end();
    for (EntitySet::const_iterator I = m_data.begin(); I != Iend; ++I) {
        contlist.push_back((*I)->getId());
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
