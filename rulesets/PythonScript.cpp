// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "PythonScript.h"

PythonScript::PythonScript(PyObject * o, Entity & t) :
                 scriptObject(o), entity(t)
{
}

PythonScript::~PythonScript()
{
}
