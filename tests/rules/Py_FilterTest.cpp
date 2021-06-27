

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "../python_testers.h"

#include "pythonbase/Python_API.h"

#include <cassert>
#include <rules/entityfilter/python/CyPy_EntityFilter.h>
#include "pythonbase/PythonMalloc.h"

int main()
{
    setupPythonMalloc();
    {
        init_python_api({&CyPy_EntityFilter::init});

        run_python_string("import entity_filter");

        //Try creating a filter with a valid query
        run_python_string("test_filter=entity_filter.Filter('entity.burn_speed=0.3')");

        //Try creating an invalid filter
        expect_python_error("entity_filter.Filter('entity instance_of | types.foo')", PyExc_TypeError);

        //test deallocator
        run_python_string("assert(test_filter)");
        run_python_string("del(test_filter)");

//    //We need PyThing components to test in-python entity matching
//    run_python_string("from server import *");
//    run_python_string("from atlas import Operation");
//
//    run_python_string("le1 = Thing('1')");
//    run_python_string("le2 = Thing('2')");
//
//    run_python_string("f = entity_filter.Filter('entity.id=1')");
//    run_python_string("assert(f.match_entity(le1))");
//    run_python_string("assert(not f.match_entity(le2))");

    }
    shutdown_python_api();
    return 0;
}
