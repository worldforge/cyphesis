add_library(rulesai_python
    CyPy_BaseMind.cpp
    CyPy_MemMap.cpp
    CyPy_Ai.cpp
    CyPy_Steering.cpp)

target_link_libraries(rulesai_python ${PYTHON_LIBRARIES} pycxx)



