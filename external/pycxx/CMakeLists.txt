add_library(pycxx
        Src/cxx_exceptions.cxx
        Src/cxx_extensions.cxx
        Src/cxxextensions.c
        Src/cxxsupport.cxx
        Src/IndirectPythonInterface.cxx

        CXX/Config.hxx
        CXX/cxx_standard_exceptions.hxx
        CXX/CxxDebug.hxx
        CXX/Exception.hxx
        CXX/ExtensionModule.hxx
        CXX/ExtensionOldType.hxx
        CXX/Extensions.hxx
        CXX/ExtensionType.hxx
        CXX/ExtensionTypeBase.hxx
        CXX/IndirectPythonInterface.hxx
        CXX/Objects.hxx
        CXX/PythonType.hxx
        CXX/Version.hxx
        CXX/WrapPython.h
        )

target_include_directories(pycxx PRIVATE .)
target_link_libraries(pycxx PUBLIC
        Python3::Python
        )


