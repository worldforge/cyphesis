// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_RESTORER_IMPL_H
#define SERVER_RESTORER_IMPL_H

#include "Restorer.h"

#include <common/Database.h>

template <class T>
void Restorer<T>::rEntity(DatabaseResult & dr)
{
    restoreString(dr.field(0, "type"), type);
    restoreString(dr.field(0, "name"), name);
    restoreFloat(dr.field(0, "px"), location.coords.X());
    restoreFloat(dr.field(0, "py"), location.coords.Y());
    restoreFloat(dr.field(0, "pz"), location.coords.Z());
    location.coords.set();
    restoreFloat(dr.field(0, "ox"), location.orientation.X());
    restoreFloat(dr.field(0, "oy"), location.orientation.Y());
    restoreFloat(dr.field(0, "oz"), location.orientation.Z());
    restoreFloat(dr.field(0, "ow"), location.orientation.W());
    location.orientation.set();
    restoreFloat(dr.field(0, "bnx"), location.bBox.nearPoint().X());
    restoreFloat(dr.field(0, "bny"), location.bBox.nearPoint().Y());
    restoreFloat(dr.field(0, "bnz"), location.bBox.nearPoint().Z());
    location.bBox.nearPoint().set();
    restoreFloat(dr.field(0, "bfx"), location.bBox.farPoint().X());
    restoreFloat(dr.field(0, "bfy"), location.bBox.farPoint().Y());
    restoreFloat(dr.field(0, "bfz"), location.bBox.farPoint().Z());
    location.bBox.farPoint().set();
    restoreFloat(dr.field(0, "status"), status);
    restoreFloat(dr.field(0, "mass"), mass);
    restoreInt(dr.field(0, "seq"), seq);
}

template <class T>
void Restorer<T>::populate(const std::string & id, DatabaseResult & dr)
{
    setId(id);
    rEntity(dr);
}

template <class T>
Entity * Restorer<T>::restore(const std::string & id, DatabaseResult & dr)
{
    T * t = new T();
    
    Restorer<T> * rt = (Restorer<T> *)t;

    rt->populate(id, dr);

    return t;
}

#endif // SERVER_RESTORER_IMPL_H
