add_library(pythonbase
        PythonClass.cpp
        PythonContext.cpp
        Python_API.cpp
        WrapperBase.cpp
        PythonMalloc.cpp
        PythonDebug.cpp)

target_link_libraries(pythonbase PUBLIC
        common
        pycxx)



