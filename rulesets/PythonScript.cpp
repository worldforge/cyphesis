// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "PythonScript.h"

PythonScript::PythonScript(PyObject * o, Entity & t) :
                 script_object(o), entity(t)
{
}

PythonScript::~PythonScript()
{
}
