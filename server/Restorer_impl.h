// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
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
    restoreString(dr.column("type"), type);
    restoreString(dr.column("name"), name);
    restoreFloat(dr.column("px"), location.m_pos.X());
    restoreFloat(dr.column("py"), location.m_pos.Y());
    restoreFloat(dr.column("pz"), location.m_pos.Z());
    location.m_pos.set();
    restoreFloat(dr.column("ox"), location.m_orientation.X());
    restoreFloat(dr.column("oy"), location.m_orientation.Y());
    restoreFloat(dr.column("oz"), location.m_orientation.Z());
    restoreFloat(dr.column("ow"), location.m_orientation.W());
    location.m_orientation.set();
    restoreFloat(dr.column("bnx"), location.m_bBox.nearPoint().X());
    restoreFloat(dr.column("bny"), location.m_bBox.nearPoint().Y());
    restoreFloat(dr.column("bnz"), location.m_bBox.nearPoint().Z());
    location.m_bBox.nearPoint().set();
    restoreFloat(dr.column("bfx"), location.m_bBox.farPoint().X());
    restoreFloat(dr.column("bfy"), location.m_bBox.farPoint().Y());
    restoreFloat(dr.column("bfz"), location.m_bBox.farPoint().Z());
    location.m_bBox.farPoint().set();
    restoreFloat(dr.column("status"), status);
    restoreFloat(dr.column("mass"), mass);
    restoreInt(dr.column("seq"), seq);
    restoreMap(dr.column("attributes"), attributes);
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
