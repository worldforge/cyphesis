// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPyING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_RESTORER_IMPL_H
#define SERVER_RESTORER_IMPL_H

#include "Restorer.h"

#include "Persistor.h"

#include "common/Database.h"
#include "common/stringstream.h"

#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/quaternion.h>

template <class T>
void Restorer<T>::restoreMap(const char * c, Atlas::Message::Element::MapType & ent)
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
    typename WFMath::CoordType x, y, z, w;
    restoreFloat(dr.column("ox"), x);
    restoreFloat(dr.column("oy"), y);
    restoreFloat(dr.column("oz"), z);
    restoreFloat(dr.column("ow"), w);
    m_location.m_orientation = WFMath::Quaternion(w, x, y, z);
    if (checkBool(dr.column("hasBox"))) {
        WFMath::Point<3> & lc = (WFMath::Point<3>&)m_location.m_bBox.lowCorner();
        restoreFloat(dr.column("bnx"), lc.x());
        restoreFloat(dr.column("bny"), lc.y());
        restoreFloat(dr.column("bnz"), lc.z());
        lc.setValid();
        WFMath::Point<3> & hc = (WFMath::Point<3>&)m_location.m_bBox.highCorner();
        restoreFloat(dr.column("bfx"), hc.x());
        restoreFloat(dr.column("bfy"), hc.y());
        restoreFloat(dr.column("bfz"), hc.z());
        hc.setValid();
    }
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
