add_library(rulessimulationscripts
        ScriptUtils.cpp
        )
target_link_libraries(rulessimulationscripts PUBLIC
        common
        pycxx
        )


add_library(rulessimulation
        BaseWorld.cpp
        Entity.cpp
        Thing.cpp
        World.cpp
        ExternalMind.cpp
        Movement.cpp
        LineProperty.cpp
        AreaProperty.cpp
        TerrainProperty.cpp
        TerrainEffectorProperty.cpp
        TerrainModProperty.cpp
        TerrainModTranslator.cpp
        CalendarProperty.cpp
        StatusProperty.cpp
        TransientProperty.cpp
        TasksProperty.cpp
        SpawnProperty.cpp
        VisibilityProperty.cpp
        Task.cpp
        SuspendedProperty.cpp
        DomainProperty.cpp
        PhysicalDomain.cpp
        VoidDomain.cpp
        InventoryDomain.cpp
        ModeProperty.cpp
        PropelProperty.cpp
        DensityProperty.cpp
        GeometryProperty.cpp
        AngularFactorProperty.cpp
        PhysicalWorld.cpp
        OgreMeshDeserializer.cpp
        PerceptionSightProperty.cpp
        UsagesProperty.cpp
        WorldTimeProperty.cpp
        UsageInstance.cpp
        MindsProperty.cpp
        AdminMind.cpp
        AdminProperty.cpp
        AttachmentsProperty.cpp
        EntityProperty.cpp
        ServerBBoxProperty.cpp
        StackableDomain.cpp
        AmountProperty.cpp
        SimulationSpeedProperty.cpp
        ModeDataProperty.cpp
        ContainerDomain.cpp
        ContainedVisibilityProperty.cpp
        ModifyProperty.cpp
        ModifiersProperty.cpp
        ModifySelfProperty.cpp
        CorePropertyManager.cpp
        WorldRouter.cpp
        VisibilityDistanceProperty.cpp
        ContainerAccessProperty.cpp
        ContainersActiveProperty.cpp
        TerrainPointsProperty.cpp
        FilterProperty.cpp
        AliasProperty.cpp
        ActionsProperty.cpp)

target_link_libraries(rulessimulation PUBLIC
        rulessimulationscripts
        common
        mercator::mercator
        ${BULLET_LIBRARIES}
        )

target_link_directories(rulessimulation PUBLIC
        ${BULLET_LIB_DIR}
        )

target_include_directories(rulessimulation PUBLIC
        ${BULLET_INCLUDE_DIR}
        )

add_subdirectory(python)
