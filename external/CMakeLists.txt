add_library(pycxx
    pycxx/Src/cxx_exceptions.cxx
    pycxx/Src/cxx_extensions.cxx
    pycxx/Src/cxxextensions.c
    pycxx/Src/cxxsupport.cxx
    pycxx/Src/IndirectPythonInterface.cxx

    pycxx/CXX/Config.hxx
    pycxx/CXX/cxx_standard_exceptions.hxx
    pycxx/CXX/CxxDebug.hxx
    pycxx/CXX/Exception.hxx
    pycxx/CXX/ExtensionModule.hxx
    pycxx/CXX/ExtensionOldType.hxx
    pycxx/CXX/Extensions.hxx
    pycxx/CXX/ExtensionType.hxx
    pycxx/CXX/ExtensionTypeBase.hxx
    pycxx/CXX/IndirectPythonInterface.hxx
    pycxx/CXX/Objects.hxx
    pycxx/CXX/PythonType.hxx
    pycxx/CXX/Version.hxx
    pycxx/CXX/WrapPython.h
)

target_include_directories(pycxx PRIVATE pycxx)


