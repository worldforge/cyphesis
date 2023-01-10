add_library(rulesbase_python
        PythonScriptFactory.cpp
        PythonWrapper.cpp
        CoordHelper.h
        ScriptsProperty.cpp)

target_link_libraries(rulesbase_python PUBLIC
        pythonbase)

add_library(atlas_python
        CyPy_Atlas.cpp
        CyPy_Operation.cpp
        CyPy_Element.cpp
        CyPy_RootEntity.cpp
        CyPy_Oplist.cpp
        CyPy_Root.cpp)

target_link_libraries(atlas_python PUBLIC
        pythonbase)

add_library(physics_python
        CyPy_Point3D.cpp
        CyPy_Vector3D.cpp
        CyPy_Quaternion.cpp
        CyPy_Axisbox.cpp
        CyPy_Physics.cpp
        CyPy_Ball.cpp)

target_link_libraries(physics_python PUBLIC
        atlas_python
        physics
        pythonbase)

add_library(common_python
        CyPy_LocatedEntity.cpp
        CyPy_Common.cpp
        CyPy_Location.cpp
        CyPy_EntityLocation.cpp
        CyPy_Props.cpp
        CyPy_Rules.cpp)

target_link_libraries(common_python PUBLIC
        physics_python
        pythonbase)
