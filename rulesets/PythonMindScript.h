// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_PYTHON_MIND_SCRIPT_H
#define RULESETS_PYTHON_MIND_SCRIPT_H

#include "PythonScript.h"

class BaseMind;

class PythonMindScript : public PythonScript {
  private:
    BaseMind & mind;
  public:
    PythonMindScript(PyObject *, BaseMind &);
    virtual ~PythonMindScript();
    virtual bool Operation(const std::string &,
                           const Atlas::Objects::Operation::RootOperation &,
                           OpVector &,
                           Atlas::Objects::Operation::RootOperation * = 0);
    virtual void hook(const std::string &, Entity *);
};

#endif // RULESETS_PYTHON_MIND_SCRIPT_H
