// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef THING_SHAPE_H
#define THING_SHAPE_H

#include <map>

#include <SOLID/solid.h>

#include "modules/Location.h"

class ThingShape {
  private:
    Vector3D box;
    Vector3D median;
    static std::map<string, ThingShape *> * dict;
    
    std::map<string, ThingShape *> & getDict() {
        if (dict == NULL) {
            dict = new std::map<string, ThingShape *>();
        }
        return *dict;
    }
  public:
    DtShapeRef solidShape;
    
    ThingShape(const string & type, double x, double y, double z) :
         box(x, y, z) {
        getDict()[type] = this;
    }

    ThingShape(const string & type, double x, double y, double z, Vector3D med):
         median(med), box(x, y, z) {
        getDict()[type] = this;
    }
};

#endif // THING_SHAPE_H
