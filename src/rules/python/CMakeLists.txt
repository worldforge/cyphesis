add_library(rulesbase_python
        PythonScriptFactory.cpp
        PythonScriptFactory_impl.h
        PythonWrapper.cpp
        CoordHelper.h
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
        CyPy_Rules.cpp
        CyPy_Root.cpp
        CyPy_Ball.cpp)

target_link_libraries(rulesbase_python PUBLIC pythonbase)



