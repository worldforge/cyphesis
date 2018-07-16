add_library(pycxx
    pycxx/Src/cxx_exceptions.cxx
    pycxx/Src/cxx_extensions.cxx
    pycxx/Src/cxxextensions.c
    pycxx/Src/cxxsupport.cxx
    pycxx/Src/IndirectPythonInterface.cxx
)

target_include_directories(pycxx PRIVATE pycxx)


