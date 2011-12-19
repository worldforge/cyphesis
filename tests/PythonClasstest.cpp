// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "python_testers.h"

#include "rulesets/PythonClass.h"

#include <cassert>

bool stub_Get_PyClass_return = false;
bool stub_Get_PyModule_return = false;

class TestPythonClass : public PythonClass {
  public:
    TestPythonClass(const std::string & p, const std::string & t) :
          PythonClass(p, t, &PyBaseObject_Type) { }

    virtual ~TestPythonClass() { }

    virtual int check() const { return 0; }

    int test_getClass(struct _object * o) { return getClass(o); }
    int test_load() { return load(); }
    int test_refresh() { return refresh(); }

    const std::string & access_package() { return m_package; }
    const std::string & access_type() { return m_type; }

    struct _object * access_module() { return m_module; }
    struct _object * access_class() { return m_class; }
};

static PyMethodDef no_methods[] = {
    {NULL,          NULL}                       /* Sentinel */
};

int main()
{
    Py_Initialize();

    Py_InitModule("testmod", no_methods);

    run_python_string("import testmod");

    run_python_string("class BadClass:\n"
                      " pass\n");
    run_python_string("class GoodClass(object):\n"
                      " pass\n");
    run_python_string("testmod.BadClass=BadClass");

    {
        const char * package = "acfd44fd-dccb-4a63-98c3-6facd580ca5f";
        const char * type = "3265e96a-28a0-417c-ad30-2970c1777c50";

        TestPythonClass * pc = new TestPythonClass(package, type);

        assert(pc != 0);

        assert(pc->access_package() == package);
        assert(pc->access_type() == type);

        assert(pc->access_module() == 0);
        assert(pc->access_class() == 0);

        delete pc;
    }

    {
        const char * package = "acfd44fd-dccb-4a63-98c3-6facd580ca5f";
        const char * type = "3265e96a-28a0-417c-ad30-2970c1777c50";

        TestPythonClass * pc = new TestPythonClass(package, type);

        stub_Get_PyModule_return = false;
        stub_Get_PyClass_return = false;

        int ret = pc->test_load();

        assert(ret == -1);
    }

    {
        const char * package = "acfd44fd-dccb-4a63-98c3-6facd580ca5f";
        const char * type = "3265e96a-28a0-417c-ad30-2970c1777c50";

        TestPythonClass * pc = new TestPythonClass(package, type);

        stub_Get_PyModule_return = true;
        stub_Get_PyClass_return = false;

        int ret = pc->test_load();

        assert(ret == -1);
    }

    {
        const char * package = "acfd44fd-dccb-4a63-98c3-6facd580ca5f";
        const char * type = "3265e96a-28a0-417c-ad30-2970c1777c50";

        TestPythonClass * pc = new TestPythonClass(package, type);

        stub_Get_PyModule_return = true;
        stub_Get_PyClass_return = true;

        int ret = pc->test_load();

        assert(ret == 0);
    }

    return 0;
}

// stubs

#include "common/log.h"

#include "rulesets/Python_Script_Utils.h"

void log(LogLevel lvl, const std::string & msg)
{
}

struct _object * Get_PyClass(struct _object * module,
                             const std::string & package,
                             const std::string & type)
{
    if (stub_Get_PyClass_return) {
        return new PyObject;
    }
    return 0;
}

struct _object * Get_PyModule(const std::string & package)
{
    if (stub_Get_PyModule_return) {
        return new PyObject;
    }
    return 0;
}
