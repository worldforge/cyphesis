add_library(rulessimulation_python
    CyPy_Entity.cpp
    CyPy_Server.cpp
    CyPy_Task.cpp
    CyPy_World.cpp
    CyPy_TerrainProperty.cpp
    CyPy_UsageInstance.cpp
    )

target_link_libraries(rulessimulation_python ${PYTHON_LIBRARIES} pycxx)



