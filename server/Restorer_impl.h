// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPyING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_RESTORER_IMPL_H
#define SERVER_RESTORER_IMPL_H

#include "Restorer.h"

#include "Persistor.h"

#include "common/Database.h"

#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/quaternion.h>

#include <sstream>

template <class T>
void Restorer<T>::rEntity(DatabaseResult::const_iterator & dr)
{
    dr.readColumn("type", this->m_type);
    dr.readColumn("name", this->m_name);
    dr.readColumn("px", this->m_location.m_pos.x());
    dr.readColumn("py", this->m_location.m_pos.y());
    dr.readColumn("pz", this->m_location.m_pos.z());
    this->m_location.m_pos.setValid();
    WFMath::CoordType x, y, z, w;
    dr.readColumn("ox", x);
    dr.readColumn("oy", y);
    dr.readColumn("oz", z);
    dr.readColumn("ow", w);
    this->m_location.m_orientation = WFMath::Quaternion(w, x, y, z);
    if (checkBool(dr.column("hasBox"))) {
        WFMath::Point<3> & lc = (WFMath::Point<3>&)this->m_location.bBox().lowCorner();
        dr.readColumn("bnx", lc.x());
        dr.readColumn("bny", lc.y());
        dr.readColumn("bnz", lc.z());
        lc.setValid();
        WFMath::Point<3> & hc = (WFMath::Point<3>&)this->m_location.bBox().highCorner();
        dr.readColumn("bfx", hc.x());
        dr.readColumn("bfy", hc.y());
        dr.readColumn("bfz", hc.z());
        hc.setValid();
        this->m_location.modifyBBox();
    }
    dr.readColumn("status", this->m_status);
    dr.readColumn("mass", this->m_mass);
    dr.readColumn("seq", this->m_seq);
    dr.readColumn("attributes", this->m_attributes);
}

template <class T>
void Restorer<T>::populate(DatabaseResult::const_iterator & dr)
{
    rEntity(dr);
}

template <class T>
Entity * Restorer<T>::restore(const std::string & id, long intId, 
                              DatabaseResult::const_iterator & dr)
{
    T * t = new T(id, intId);
    
    Restorer<T> * rt = (Restorer<T> *)t;

    rt->populate(dr);

    m_persist.hookup(*t);

    return t;
}

#endif // SERVER_RESTORER_IMPL_H
