// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_TYPE_UTILS_H
#define COMMON_TYPE_UTILS_H

#include "types.h"

inline Atlas::Message::Object::ListType idListAsObject(const IdList &l)
{
    Atlas::Message::Object::ListType ret;
    for(IdList::const_iterator I = l.begin(); I != l.end(); I++) {
        ret.push_back(I);
    }
    return ret;
}

inline IdList idListFromAtlas(const Atlas::Message::Object & o)
{
    IdList ret;
    const Atlas::Message::Object::ListType & l = o.AsList();
    Atlas::Message::Object::ListType::const_iterator I = l.begin();
    for (; I != l.end(); ++I) {
        ret.push_back(I->AsString());
    }
    return ret;
}

// This could probably be made into a template, as it will work
// on any type which implements asObject()
inline Atlas::Message::Object::ListType coordListAsObject(const CoordList &l)
{
    Atlas::Message::Object::ListType ret;
    for(CoordList::const_iterator I = l.begin(); I != l.end(); I++) {
        ret.push_back(I->asObject());
    }
    return ret;
}

inline CoordList coordListFromAtlas(const Atlas::Message::Object & o)
{
    CoordList ret;
    const Atlas::Message::Object::ListType & l = o.AsList();
    Atlas::Message::Object::ListType::const_iterator I = l.begin();
    for (; I != l.end(); ++I) {
        ret.push_back(Vector3D(I->AsList()));
    }
    return ret;
}

#endif // COMMON_TYPE_UTILS_H
