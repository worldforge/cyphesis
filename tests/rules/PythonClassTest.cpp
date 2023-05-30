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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "../python_testers.h"

#include "pythonbase/PythonClass.h"
#include "pycxx/CXX/Extensions.hxx"
#include <cassert>
#include "pythonbase/PythonMalloc.h"
#include "pythonbase/Python_API.h"

Py::Object* stub_Get_PyClass_return = nullptr;
Py::Module* stub_Get_PyModule_return = nullptr;


class TestPythonClass : public PythonClass
{
    public:
        TestPythonClass(const std::string& p, const std::string& t) :
            PythonClass(p, t)
        {}

        virtual ~TestPythonClass()
        {}

        virtual int check() const
        { return 0; }

        int test_getClass(Py::Module o)
        { return getClass(o); }

        int test_load()
        { return load(); }

        int test_refresh()
        { return refresh(); }

        const std::string& access_package()
        { return m_package; }

        const std::string& access_type()
        { return m_type; }

        Py::Module& access_module()
        { return m_module; }

        boost::optional<Py::Callable>& access_class()
        { return m_class; }
};

struct TestMod : public Py::ExtensionModule<TestMod>
{
    TestMod() : ExtensionModule("testmod")
    {
        initialize("testmod");
    }
};


int main()
{
#if defined(PYTHONHOME)
    setenv("PYTHONHOME", PYTHONHOME, 1);
#endif
    setupPythonMalloc();
    PyImport_AppendInittab("testmod", []() {
        static TestMod testMod;
        return testMod.module().ptr();
    });

    Py_Initialize();

    {
        run_python_string("import testmod");

        run_python_string("class BadClass:\n"
                          " pass\n");
        run_python_string("class GoodClass(object):\n"
                          " pass\n");
        run_python_string("testmod.BadClass=BadClass");
        run_python_string("testmod.GoodClass=GoodClass");

        Py::Module testmod("testmod");
        auto good_class = Py::Callable(testmod.getAttr("GoodClass"));

        {
            const char* package = "acfd44fd-dccb-4a63-98c3-6facd580ca5f";
            const char* type = "3265e96a-28a0-417c-ad30-2970c1777c50";

            TestPythonClass* pc = new TestPythonClass(package, type);

            assert(pc != nullptr);

            assert(pc->access_package() == package);
            assert(pc->access_type() == type);

            assert(pc->access_module().isNull());
            assert(!pc->access_class());

            delete pc;
        }

        {
            const char* package = "acfd44fd-dccb-4a63-98c3-6facd580ca5f";
            const char* type = "3265e96a-28a0-417c-ad30-2970c1777c50";

            TestPythonClass* pc = new TestPythonClass(package, type);

            stub_Get_PyModule_return = nullptr;
            stub_Get_PyClass_return = nullptr;

            int ret = pc->test_load();

            assert(ret == -1);
            delete pc;
        }

        {
            const char* package = "acfd44fd-dccb-4a63-98c3-6facd580ca5f";
            const char* type = "3265e96a-28a0-417c-ad30-2970c1777c50";

            TestPythonClass* pc = new TestPythonClass(package, type);

            stub_Get_PyModule_return = &testmod;
            stub_Get_PyClass_return = nullptr;

            int ret = pc->test_load();

            assert(ret == -1);
            delete pc;
        }

        {
            const char* package = "acfd44fd-dccb-4a63-98c3-6facd580ca5f";
            const char* type = "3265e96a-28a0-417c-ad30-2970c1777c50";

            TestPythonClass* pc = new TestPythonClass(package, type);

            stub_Get_PyModule_return = &testmod;
            stub_Get_PyClass_return = &good_class;

            int ret = pc->test_load();

            assert(ret == 0);
            delete pc;
        }
    }
    Py_Finalize();

    return 0;
}

// stubs

#include "common/log.h"

#include "pythonbase/Python_Script_Utils.h"
#include "../stubs/common/stublog.h"


Py::Object Get_PyClass(const Py::Module& module,
                       const std::string & package,
                       const std::string & type)
{
    if (stub_Get_PyClass_return) {
        return *stub_Get_PyClass_return;
    }
    return Py::Null();
}

Py::Module Get_PyModule(const std::string& package)
{
    if (stub_Get_PyModule_return) {
        return *stub_Get_PyModule_return;
    }
    return Py::Module(nullptr);
}
