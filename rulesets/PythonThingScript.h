// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_PYTHON_THING_SCRIPT_H
#define RULESETS_PYTHON_THING_SCRIPT_H

#include "PythonScript.h"

#include <Python.h>

class Thing;

class PythonThingScript : public PythonScript {
  private:
    Thing & thing;
  public:
    PythonThingScript(PyObject *, Thing &);
    virtual ~PythonThingScript();
    virtual bool Operation(const std::string &,
                           const Atlas::Objects::Operation::RootOperation &,
                           oplist &,
                      Atlas::Objects::Operation::RootOperation * sub_op=NULL);
    virtual void hook(const std::string &, Entity *);
};

#endif // RULESETS_PYTHON_THING_SCRIPT_H
