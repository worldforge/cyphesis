add_library(pythonbase
        PythonClass.cpp
        PythonContext.cpp
        Python_API.cpp
        WrapperBase.cpp
        PythonMalloc.cpp)

target_link_libraries(pythonbase common ${PYTHON_LIBRARIES})



