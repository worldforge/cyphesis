add_library(rulessimulation_python
        CyPy_Entity.cpp
        CyPy_Server.cpp
        CyPy_Task.cpp
        CyPy_World.cpp
        CyPy_TerrainProperty.cpp
        CyPy_UsageInstance.cpp
        CyPy_EntityProps.cpp
        CyPy_Domain.cpp
        )

target_link_libraries(rulessimulation_python PUBLIC
        rulessimulation
        pythonbase)



