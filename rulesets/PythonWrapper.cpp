// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "PythonWrapper.h"

PythonWrapper::PythonWrapper(PyObject * wrapper) : m_wrapper(wrapper)
{
}

PythonWrapper::~PythonWrapper()
{
}
