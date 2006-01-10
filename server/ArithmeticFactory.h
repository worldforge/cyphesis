// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Alistair Riddoch

#ifndef SERVER_ARITHMETIC_FACTORY_H
#define SERVER_ARITHMETIC_FACTORY_H

#include <Python.h>

#include <string>

class ArithmeticScript;
class Character;

/// \brief Base class for for factories for creating arithmetic scripts
class ArithmeticFactory {
  public:
    virtual ~ArithmeticFactory();

    virtual ArithmeticScript * newScript(Character & chr) = 0;
};

class PythonArithmeticFactory : public ArithmeticFactory {
  public:
    PyObject * m_module;
    PyObject * m_class;

    std::string m_package;
    std::string m_name;

    int getClass();
    int addScript();
  public:
    PythonArithmeticFactory(const std::string & package,
                            const std::string & name);
    virtual ~PythonArithmeticFactory();

    virtual ArithmeticScript * newScript(Character & chr);
};

#endif // SERVER_ARITHMETIC_FACTORY_H
