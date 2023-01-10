add_executable(pythondocgenerator
        PythonDocGenerator.cpp)


target_link_libraries(pythondocgenerator
        rulesai_python
        rulessimulation_python
        entityfilter_python
        rulesbase_python
        rulesbase_python
        )
