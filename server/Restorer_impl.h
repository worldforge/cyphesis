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
    restoreString(dr.field("type"), type);
    restoreString(dr.field("name"), name);
    restoreFloat(dr.field("px"), location.coords.X());
    restoreFloat(dr.field("py"), location.coords.Y());
    restoreFloat(dr.field("pz"), location.coords.Z());
    location.coords.set();
    restoreFloat(dr.field("ox"), location.orientation.X());
    restoreFloat(dr.field("oy"), location.orientation.Y());
    restoreFloat(dr.field("oz"), location.orientation.Z());
    restoreFloat(dr.field("ow"), location.orientation.W());
    location.orientation.set();
    restoreFloat(dr.field("bnx"), location.bBox.nearPoint().X());
    restoreFloat(dr.field("bny"), location.bBox.nearPoint().Y());
    restoreFloat(dr.field("bnz"), location.bBox.nearPoint().Z());
    location.bBox.nearPoint().set();
    restoreFloat(dr.field("bfx"), location.bBox.farPoint().X());
    restoreFloat(dr.field("bfy"), location.bBox.farPoint().Y());
    restoreFloat(dr.field("bfz"), location.bBox.farPoint().Z());
    location.bBox.farPoint().set();
    restoreFloat(dr.field("status"), status);
    restoreFloat(dr.field("mass"), mass);
    restoreInt(dr.field("seq"), seq);
}

template <class T>
void Restorer<T>::populate(const std::string & id, DatabaseResult & dr)
{
    // setId(id);
    rEntity(dr);
}

template <class T>
Entity * Restorer<T>::restore(const std::string & id, DatabaseResult & dr)
{
    T * t = new T(id);
    
    Restorer<T> * rt = (Restorer<T> *)t;

    rt->populate(id, dr);

    return t;
}

#endif // SERVER_RESTORER_IMPL_H
