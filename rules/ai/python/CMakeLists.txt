add_library(rulesmind_python
    CyPy_BaseMind.cpp
    CyPy_MemEntity.cpp
    CyPy_MemMap.cpp
    CyPy_Ai.cpp CyPy_Ai.h)

target_link_libraries(rulesmind_python ${PYTHON_LIBRARIES} pycxx)



