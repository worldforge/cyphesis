// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef THING_SHAPE_H
#define THING_SHAPE_H

#include <map>

#include <SOLID/solid.h>

#include "modules/Location.h"

class ThingShape {
  private:
    shape_t shape;
    Vector3D box;
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
         shape(SH_C_BOX), box(x, y, z) {
        getDict()[type] = this;
    }
    ThingShape(const string & type, double rad, double height) :
         shape(SH_M_CYL), box(rad, rad, height) {
        getDict()[type] = this;
    }
};

#endif // THING_SHAPE_H
