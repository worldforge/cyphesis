// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_PYTHON_THING_SCRIPT_H
#define RULESETS_PYTHON_THING_SCRIPT_H

#include "PythonScript.h"

class Entity;

class PythonEntityScript : public PythonScript {
  private:
    Entity & m_entity;
  public:
    PythonEntityScript(PyObject *, Entity &);
    virtual ~PythonEntityScript();
    virtual bool Operation(const std::string &, const RootOperation &,
                           OpVector &, RootOperation * sub_op = 0);
    virtual void hook(const std::string &, Entity *);
};

#endif // RULESETS_PYTHON_THING_SCRIPT_H
