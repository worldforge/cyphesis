// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_RESTORER_IMPL_H
#define SERVER_RESTORER_IMPL_H

#include "Restorer.h"

#include "Persistor.h"

#include <common/Database.h>

template <class T>
void Restorer<T>::rEntity(DatabaseResult::const_iterator & dr)
{
    restoreString(dr.column("type"), type);
    restoreString(dr.column("name"), name);
    restoreFloat(dr.column("px"), location.coords.X());
    restoreFloat(dr.column("py"), location.coords.Y());
    restoreFloat(dr.column("pz"), location.coords.Z());
    location.coords.set();
    restoreFloat(dr.column("ox"), location.orientation.X());
    restoreFloat(dr.column("oy"), location.orientation.Y());
    restoreFloat(dr.column("oz"), location.orientation.Z());
    restoreFloat(dr.column("ow"), location.orientation.W());
    location.orientation.set();
    restoreFloat(dr.column("bnx"), location.bBox.nearPoint().X());
    restoreFloat(dr.column("bny"), location.bBox.nearPoint().Y());
    restoreFloat(dr.column("bnz"), location.bBox.nearPoint().Z());
    location.bBox.nearPoint().set();
    restoreFloat(dr.column("bfx"), location.bBox.farPoint().X());
    restoreFloat(dr.column("bfy"), location.bBox.farPoint().Y());
    restoreFloat(dr.column("bfz"), location.bBox.farPoint().Z());
    location.bBox.farPoint().set();
    restoreFloat(dr.column("status"), status);
    restoreFloat(dr.column("mass"), mass);
    restoreInt(dr.column("seq"), seq);
}

template <class T>
void Restorer<T>::populate(const std::string & id, DatabaseResult::const_iterator & dr)
{
    // setId(id);
    rEntity(dr);
}

template <class T>
Entity * Restorer<T>::restore(const std::string & id, DatabaseResult::const_iterator & dr)
{
    T * t = new T(id);
    
    Restorer<T> * rt = (Restorer<T> *)t;

    rt->populate(id, dr);

    m_persist.hookup(*t);

    return t;
}

#endif // SERVER_RESTORER_IMPL_H
