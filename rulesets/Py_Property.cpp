// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "Py_Property.h"

#include "Py_Statistics.h"
#include "StatisticsProperty.h"

#include <iostream>

PyObject * Property_asPyObject(PropertyBase * property, Entity * owner)
{
    StatisticsProperty * sp = dynamic_cast<StatisticsProperty *>(property);
    if (sp != 0) {
        PyStatistics * ps = newPyStatistics();
        if (ps == NULL) {
            return NULL;
        }
        ps->m_entity = owner;
        return (PyObject*)ps;
    }
    return 0;
}
