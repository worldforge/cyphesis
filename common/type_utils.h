// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_TYPE_UTILS_H
#define COMMON_TYPE_UTILS_H

#include "types.h"

inline void idListAsObject(const IdList & l,
                           Atlas::Message::Object::ListType & ol)
{
    ol.clear();
    for(IdList::const_iterator I = l.begin(); I != l.end(); I++) {
        ol.push_back(*I);
    }
}

inline void idListFromAtlas(const Atlas::Message::Object::ListType & l,
                            IdList & ol)
{
    ol.clear();
    Atlas::Message::Object::ListType::const_iterator I = l.begin();
    for (; I != l.end(); ++I) {
        ol.push_back(I->AsString());
    }
}

// This could probably be made into a template, as it will work
// on any type which implements asObject()
inline void coordListAsObject(const CoordList & l,
                              Atlas::Message::Object::ListType & ol)
{
    ol.clear();
    for(CoordList::const_iterator I = l.begin(); I != l.end(); I++) {
        ol.push_back(I->asObject());
    }
}

inline void coordListFromAtlas(const Atlas::Message::Object::ListType & l,
                               CoordList & ol)
{
    ol.clear();
    Atlas::Message::Object::ListType::const_iterator I = l.begin();
    for (; I != l.end(); ++I) {
        ol.push_back(Vector3D(I->AsList()));
    }
}

#endif // COMMON_TYPE_UTILS_H
