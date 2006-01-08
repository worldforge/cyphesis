// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_PYTHON_ARITHMETIC_SCRIPT_H
#define RULESETS_PYTHON_ARITHMETIC_SCRIPT_H

#include "ArithmeticScript.h"

/// \brief Base class for script objects which handle statistics for entities.
///
/// This base class allows scripts to override attribute requests
class PythonArithmeticScript : public ArithmeticScript {
  protected:
    struct _object * m_script;
  public:
    PythonArithmeticScript(struct _object *);
    virtual ~PythonArithmeticScript();

    virtual int attribute(const std::string &, float &);
};

#endif // RULESETS_PYTHON_ARITHMETIC_SCRIPT_H
