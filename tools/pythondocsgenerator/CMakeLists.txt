add_executable(pythondocgenerator
        PythonDocGenerator.cpp)


target_link_libraries(pythondocgenerator PUBLIC
        rulesai_python
        rulessimulation_python
        entityfilter_python
        rulesbase_python
        atlas_python
        common_python
        physics_python
        )
