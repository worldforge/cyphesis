// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPyING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_RESTORER_IMPL_H
#define SERVER_RESTORER_IMPL_H

#include "Restorer.h"

#include "Persistor.h"

#include "common/Database.h"
#include "common/stringstream.h"

template <class T>
void Restorer<T>::restoreMap(const char * c, Atlas::Message::Object::MapType & ent)
{
    if (c == 0) { return; }
    Database::instance()->decodeObject(c, ent);
}

template <class T>
void Restorer<T>::rEntity(DatabaseResult::const_iterator & dr)
{
    restoreString(dr.column("type"), m_type);
    restoreString(dr.column("name"), m_name);
    restoreFloat(dr.column("px"), m_location.m_pos.x());
    restoreFloat(dr.column("py"), m_location.m_pos.y());
    restoreFloat(dr.column("pz"), m_location.m_pos.z());
    m_location.m_pos.setValid();
#warning FIXME Must be able to modify axisbox
    // restoreFloat(dr.column("ox"), m_location.m_orientation.vector().x());
    // restoreFloat(dr.column("oy"), m_location.m_orientation.vector().y());
    // restoreFloat(dr.column("oz"), m_location.m_orientation.vector().z());
    // restoreFloat(dr.column("ow"), m_location.m_orientation.scalar());
    // m_location.m_orientation.setValid();
#warning FIXME Must be able to modify axisbox
    // restoreFloat(dr.column("bnx"), m_location.m_bBox.lowCorner().x());
    // restoreFloat(dr.column("bny"), m_location.m_bBox.lowCorner().y());
    // restoreFloat(dr.column("bnz"), m_location.m_bBox.lowCorner().z());
    // m_location.m_bBox.lowCorner().setValid();
#warning FIXME Must be able to modify axisbox
    // restoreFloat(dr.column("bfx"), m_location.m_bBox.highCorner().x());
    // restoreFloat(dr.column("bfy"), m_location.m_bBox.highCorner().y());
    // restoreFloat(dr.column("bfz"), m_location.m_bBox.highCorner().z());
    // m_location.m_bBox.highCorner().setValid();
    restoreFloat(dr.column("status"), m_status);
    restoreFloat(dr.column("mass"), m_mass);
    restoreInt(dr.column("seq"), m_seq);
    restoreMap(dr.column("attributes"), m_attributes);
}

template <class T>
void Restorer<T>::populate(DatabaseResult::const_iterator & dr)
{
    rEntity(dr);
}

template <class T>
Entity * Restorer<T>::restore(const std::string & id, DatabaseResult::const_iterator & dr)
{
    T * t = new T(id);
    
    Restorer<T> * rt = (Restorer<T> *)t;

    rt->populate(dr);

    m_persist.hookup(*t);

    return t;
}

#endif // SERVER_RESTORER_IMPL_H
