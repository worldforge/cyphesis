

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

    run_python_string("import filter");

    //Try creating a filter with a valid query
    run_python_string("filter.get_filter('entity.type=types.bear')");

    //Try creating an invalid filter
    expect_python_error("filter.get_filter('foobar')", PyExc_TypeError);

    shutdown_python_api();
    return 0;
}
