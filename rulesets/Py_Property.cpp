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

// $Id: Py_Property.cpp,v 1.6 2007-07-03 02:03:59 alriddoch Exp $

#include "Py_Property.h"

#include "Py_Statistics.h"
#include "StatisticsProperty.h"
#include "Statistics.h"
#include "TerrainProperty.h"
#include "PythonArithmeticScript.h"

#include "common/log.h"

#include <iostream>

PyObject * Property_asPyObject(PropertyBase * property, Entity * owner)
{
    StatisticsProperty * sp = dynamic_cast<StatisticsProperty *>(property);
    if (sp != 0) {
        PythonArithmeticScript * script = dynamic_cast<PythonArithmeticScript *>(sp->data().m_script);
        if (script != 0) {
            PyObject * o = script->script();
            Py_INCREF(o);
            return o;
        } else {
            log(WARNING, "Unexpected non-python Statistics script");
            PyStatistics * ps = newPyStatistics();
            if (ps == NULL) {
                return NULL;
            }
            ps->m_entity = owner;
            return (PyObject*)ps;
        }
    }
    TerrainProperty * tp = dynamic_cast<TerrainProperty *>(property);
    if (tp != 0) {
        std::cout << "It's a terrain property" << std::endl << std::flush;
        // Create a new python wrapper for this script, or even re-use an
        // existing wrapper if that is possible.
    }
    return 0;
}
