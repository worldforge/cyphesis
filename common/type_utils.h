// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_TYPE_UTILS_H
#define COMMON_TYPE_UTILS_H

#include "types.h"

#include <wfmath/atlasconv.h>

inline void idListasObject(const IdList & l, ListType & ol)
{
    ol.clear();
    for(IdList::const_iterator I = l.begin(); I != l.end(); I++) {
        ol.push_back(*I);
    }
}

inline void idListFromAtlas(const ListType & l, IdList & ol)
{
    ol.clear();
    ListType::const_iterator I = l.begin();
    for (; I != l.end(); ++I) {
        ol.push_back(I->asString());
    }
}

// This could probably be made into a template, as it will work
// on any type which implements asMessage()
template<typename List_T>
void objectListAsMessage(const List_T & l, ListType & ol)
{
    ol.clear();
    for(typename List_T::const_iterator I = l.begin(); I != l.end(); ++I) {
        ol.push_back(I->toAtlas());
    }
}

template<typename T, typename List_T>
inline void objectListFromMessage(const ListType & l, List_T & ol)
{
    ol.clear();
    ListType::const_iterator I = l.begin();
    for (; I != l.end(); ++I) {
        ol.push_back(T(I->asList()));
    }
}

#endif // COMMON_TYPE_UTILS_H
