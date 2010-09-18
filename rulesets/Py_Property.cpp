// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#include "Py_Property.h"

#include "StatisticsProperty.h"
#include "TerrainModProperty.h"
#include "TerrainProperty.h"
#include "PythonArithmeticScript.h"

#include "common/log.h"

#include <iostream>

PyObject * Property_asPyObject(PropertyBase * property, Entity * owner)
{
    StatisticsProperty * sp = dynamic_cast<StatisticsProperty *>(property);
    if (sp != 0) {
        PythonArithmeticScript * script = dynamic_cast<PythonArithmeticScript *>(sp->script());
        if (script != 0) {
            PyObject * o = script->script();
            Py_INCREF(o);
            return o;
        } else {
            log(ERROR, "Unexpected non-python Statistics script");
            // FIXME Do we need PyStatisticsProperty for this kind of thing?
            // PyStatistics * ps = newPyStatistics();
            // if (ps == NULL) {
                // return NULL;
            // }
            // ps->m_entity = owner;
            // return (PyObject*)ps;
            Py_INCREF(Py_None);
            return Py_None;
        }
    }
    TerrainProperty * tp = dynamic_cast<TerrainProperty *>(property);
    if (tp != 0) {
        // Create a new python wrapper for this property.
        PyTerrainProperty * prop = newPyTerrainProperty();
        if (prop != NULL) {
            prop->m_entity = owner;
            prop->m_property = tp;
        }
        return (PyObject*)prop;
    }
    TerrainModProperty * tm = dynamic_cast<TerrainModProperty *>(property);
    if (tm != 0) {
        // Create a new python wrapper for this property
        PyTerrainModProperty * prop = newPyTerrainModProperty();
        if (prop != NULL) {
            prop->m_entity = owner;
            prop->m_property = tm;
        }
        return (PyObject*)prop;
    }
    return 0;
}
