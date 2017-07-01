add_library(rulesetbase
        LocatedEntity.cpp
        EntityProperties.cpp
        AtlasProperties.cpp
        Container.cpp
        Script.cpp)

add_library(rulesetentity
        Entity.cpp
        Thing.cpp
        World.cpp
        Character.cpp
        Creator.cpp
        Plant.cpp
        Stackable.cpp
        Domain.cpp
        ExternalMind.cpp
        Movement.cpp
        Pedestrian.cpp
        EntityProperty.cpp
        OutfitProperty.cpp
        LineProperty.cpp
        AreaProperty.cpp
        TerrainProperty.cpp
        TerrainEffectorProperty.cpp
        TerrainModProperty.cpp
        TerrainModTranslator.cpp
        CalendarProperty.cpp
        SolidProperty.cpp
        StatusProperty.cpp
        BBoxProperty.cpp
        TransientProperty.cpp
        InternalProperties.cpp
        StatisticsProperty.cpp
        TasksProperty.cpp
        SpawnProperty.cpp
        VisibilityProperty.cpp
        ExternalProperty.cpp
        BiomassProperty.cpp
        BurnSpeedProperty.cpp
        DecaysProperty.cpp
        Task.cpp
        ArithmeticScript.cpp
        ArithmeticFactory.cpp
        SuspendedProperty.cpp
        SpawnerProperty.cpp
        ImmortalProperty.cpp
        RespawningProperty.cpp
        DefaultLocationProperty.cpp
        DomainProperty.cpp
        LimboProperty.cpp
        PhysicalDomain.cpp
        VoidDomain.cpp
        ProxyMind.cpp
        InventoryDomain.cpp
        ModeProperty.cpp
        PropelProperty.cpp
        DensityProperty.cpp
        GeometryProperty.cpp
        AngularFactorProperty.cpp
        QuaternionProperty.cpp
        Vector3Property.cpp PhysicalWorld.cpp PhysicalWorld.h)

target_link_libraries(rulesetentity ${BULLET_LIBRARIES})

add_library(entityfilter
        entityfilter/Filter.cpp
        entityfilter/Providers.cpp
        entityfilter/Predicates.cpp)

add_library(rulesetmind
        BaseMind.cpp
        MindFactory.cpp
        MindProperty.cpp
        MemEntity.cpp
        MemMap.cpp
        mind/AwareMind.cpp mind/AwareMindFactory.cpp
        mind/AwarenessStore.cpp mind/AwarenessStoreProvider.cpp
        mind/SharedTerrain.cpp)

add_library(scriptpython
        Py_Message.cpp
        Py_Operation.cpp
        Py_RootEntity.cpp
        Py_Oplist.cpp
        Py_Vector3D.cpp
        Py_Point3D.cpp
        Py_Thing.cpp
        Py_Filter.cpp
        Py_Map.cpp
        Py_Quaternion.cpp
        Py_BBox.cpp
        Py_WorldTime.cpp
        Py_World.cpp
        Py_Location.cpp
        Py_Task.cpp
        Py_Shape.cpp
        Py_Property.cpp
        Py_TerrainModProperty.cpp
        Py_TerrainProperty.cpp
        Python_API.cpp
        PythonClass.cpp
        PythonContext.cpp
        PythonWrapper.cpp
        PythonEntityScript.cpp
        PythonArithmeticScript.cpp
        PythonArithmeticFactory.cpp
        PythonScriptFactory.cpp)

target_link_libraries(scriptpython ${PYTHON_LIBRARIES})

install(FILES
        basic/world/objects/Thing.py
        basic/world/probability.py
        basic/world/objects/__init__.py
        basic/editor.py
        DESTINATION ${CMAKE_INSTALL_DATADIR}/cyphesis/scripts/cyphesis)
install(DIRECTORY
        mason minimal mars deeds
        DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/cyphesis/rulesets)
install(DIRECTORY
        basic/mind
        DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/cyphesis/rulesets/basic)

#moduledir = $(datadir)/cyphesis/scripts/cyphesis



