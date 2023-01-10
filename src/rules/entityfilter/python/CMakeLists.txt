add_library(entityfilter_python
        CyPy_EntityFilter.cpp
        )

target_link_libraries(entityfilter_python PUBLIC
        pythonbase)



