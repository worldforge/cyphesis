

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "python_testers.h"

#include "rulesets/Python_API.h"
#include "rulesets/Py_Filter.h"

#include <cassert>

int main()
{
    init_python_api("");

    run_python_string("import entity_filter");

    //Try creating a filter with a valid query
    run_python_string("test_filter=entity_filter.get_filter('entity.burn_speed=0.3')");

    //Try creating an invalid filter
    expect_python_error("entity_filter.get_filter('foobar')", PyExc_TypeError);

    //test deallocator
    run_python_string("assert(test_filter)");
    run_python_string("del(test_filter)");

    shutdown_python_api();
    return 0;
}
