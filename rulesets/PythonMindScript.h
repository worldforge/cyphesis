// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef PYTHON_MIND_SCRIPT_H
#define PYTHON_MIND_SCRIPT_H

#include "PythonScript.h"

class BaseMind;

class PythonMindScript : public PythonScript {
  private:
    BaseMind & mind;
  public:
    PythonMindScript(PyObject *, BaseMind &);
    virtual ~PythonMindScript();
    virtual bool Operation(const string&,
                      const Atlas::Objects::Operation::RootOperation&, oplist&,
                      Atlas::Objects::Operation::RootOperation * sub_op=NULL);
    virtual void hook(const string &, Entity *);
};

#endif // PYTHON_MIND_SCRIPT_H
