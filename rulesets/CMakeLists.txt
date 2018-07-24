add_library(rulesetbase
    LocatedEntity.cpp
    EntityProperties.cpp
    AtlasProperties.cpp
    Script.cpp
    Location.cpp)

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
    Vector3Property.cpp
    PhysicalWorld.cpp PhysicalWorld.h
    OgreMeshDeserializer.cpp OgreMeshDeserializer.h
    PerceptionSightProperty.cpp PerceptionSightProperty.h
    ScaleProperty.cpp ScaleProperty.h
    ScriptsProperty.cpp ScriptsProperty.h
    UsagesProperty.cpp UsagesProperty.h)

target_link_libraries(rulesetentity ${BULLET_LIBRARIES})

add_library(entityfilter
    entityfilter/Filter.cpp
    entityfilter/Providers.cpp
    entityfilter/Predicates.cpp
    entityfilter/ParserDefinitions.h python/CyPy_Props.cpp python/CyPy_Props.h)

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
#    Py_Message.cpp
#    Py_Operation.cpp
#    Py_RootEntity.cpp
#    Py_Oplist.cpp
#    Py_Vector3D.cpp
#    Py_Point3D.cpp
#    Py_Thing.cpp
#    Py_Filter.cpp
#    Py_MemMap.cpp
#    Py_Quaternion.cpp
#    Py_BBox.cpp
#    Py_WorldTime.cpp
#    Py_World.cpp
#    Py_Location.cpp
#    Py_Task.cpp
#    Py_Shape.cpp
#    Py_Property.cpp
#    Py_TerrainModProperty.cpp
#    Py_TerrainProperty.cpp
    Python_API.cpp
    PythonClass.cpp
    PythonContext.cpp
    PythonWrapper.cpp
    PythonEntityScript.cpp
    PythonArithmeticScript.cpp
    PythonArithmeticFactory.cpp
    PythonScriptFactory.cpp
    PythonScriptFactory_impl.h
    CyPy_Props.cpp CyPy_Props.h

    python/CyPy_EntityFilter.cpp python/CyPy_EntityFilter.h
    python/CyPy_LocatedEntity.cpp python/CyPy_LocatedEntity.h
    python/CyPy_Entity.cpp python/CyPy_Entity.h
    python/CyPy_Common.cpp python/CyPy_Common.h
    python/CyPy_Atlas.cpp python/CyPy_Atlas.h
    python/CyPy_Operation.cpp python/CyPy_Operation.h
    python/CyPy_Element.cpp python/CyPy_Element.h
    python/CyPy_RootEntity.cpp python/CyPy_RootEntity.h
    python/CyPy_Oplist.cpp python/CyPy_Oplist.h
    python/CyPy_Location.cpp python/CyPy_Location.h
    python/CyPy_Point3D.cpp python/CyPy_Point3D.h
    python/CyPy_Vector3D.cpp python/CyPy_Vector3D.h
    python/CyPy_Quaternion.cpp python/CyPy_Quaternion.h
    python/CyPy_Axisbox.cpp python/CyPy_Axisbox.h
    python/CyPy_Physics.cpp python/CyPy_Physics.h
    python/CoordHelper.cpp python/CoordHelper.h
    python/WrapperBase.cpp python/WrapperBase.h
    python/CyPy_Server.cpp python/CyPy_Server.h
    python/CyPy_MemMap.cpp python/CyPy_MemMap.h
    python/CyPy_MemEntity.cpp python/CyPy_MemEntity.h
    python/CyPy_Character.cpp python/CyPy_Character.h
    python/CyPy_BaseMind.cpp python/CyPy_BaseMind.h
    python/CyPy_Task.cpp python/CyPy_Task.h
    python/CyPy_WorldTime.cpp python/CyPy_WorldTime.h
    python/CyPy_World.cpp python/CyPy_World.h
    python/CyPy_TerrainProperty.cpp python/CyPy_TerrainProperty.h)

target_link_libraries(scriptpython ${PYTHON_LIBRARIES} pycxx)
# Prevent GCC from messing up Python pointer handling
#target_compile_options(scriptpython PUBLIC "-fno-strict-aliasing")

install(FILES
    basic/scripts/world/probability.py
    basic/scripts/editor.py
    DESTINATION ${CMAKE_INSTALL_DATADIR}/cyphesis/scripts/cyphesis)
install(DIRECTORY
    deeds
    DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/cyphesis/rulesets)
install(DIRECTORY
    basic/scripts/mind
    DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/cyphesis/rulesets/basic/scripts)

#moduledir = $(datadir)/cyphesis/scripts/cyphesis



