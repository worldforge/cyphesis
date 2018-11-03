add_library(rulesbase_python
    PythonClass.cpp
    PythonContext.cpp
    PythonWrapper.cpp
    PythonArithmeticScript.cpp
    PythonArithmeticFactory.cpp
    PythonScriptFactory.cpp
    PythonScriptFactory_impl.h
    Python_API.cpp
    CoordHelper.h
    WrapperBase.cpp
    ScriptsProperty.cpp
    CyPy_LocatedEntity.cpp
    CyPy_Common.cpp
    CyPy_Atlas.cpp
    CyPy_Operation.cpp
    CyPy_Element.cpp
    CyPy_RootEntity.cpp
    CyPy_Oplist.cpp
    CyPy_Location.cpp
    CyPy_Point3D.cpp
    CyPy_Vector3D.cpp
    CyPy_Quaternion.cpp
    CyPy_Axisbox.cpp
    CyPy_Physics.cpp
    CyPy_EntityLocation.cpp
    CyPy_Props.cpp
    CyPy_WorldTime.cpp
    CyPy_Rules.cpp CyPy_Rules.h)

target_link_libraries(rulesbase_python ${PYTHON_LIBRARIES} pycxx)



