// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_RESTORER_IMPL_H
#define SERVER_RESTORER_IMPL_H

#include "Restorer.h"

template <class T>
void Restorer<T>::populate(int what_exactly)
{
    setId("ffo");
    type = "thing";
    seq = what_exactly;
}

template <class T>
Entity * Restorer<T>::restore(int what_exactly)
{
    T * t = new T();
    
    Restorer<T> * rt = (Restorer<T> *)t;

    rt->populate(what_exactly);

    return t;
}

#endif // SERVER_RESTORER_IMPL_H
