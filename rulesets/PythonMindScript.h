// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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

// $Id: PythonMindScript.h,v 1.17 2007-12-02 23:49:07 alriddoch Exp $

#error This file has been removed from the build
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef RULESETS_PYTHON_MIND_SCRIPT_H
#define RULESETS_PYTHON_MIND_SCRIPT_H

#include "PythonScript.h"

/// \brief Script class for Python scripts attached to a character Mind
class PythonMindScript : public PythonScript {
  public:
    explicit PythonMindScript(PyObject *, PyObject *);
    virtual ~PythonMindScript();

    virtual bool operation(const std::string & opname,
                           const Atlas::Objects::Operation::RootOperation & op,
                           OpVector & res);
    virtual void hook(const std::string & function, LocatedEntity * entity);
};

#endif // RULESETS_PYTHON_MIND_SCRIPT_H
