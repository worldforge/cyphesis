add_library(rulesbase
        LocatedEntity.cpp
        EntityProperties.cpp
        AtlasProperties.cpp
        Script.cpp
        Location.cpp
        EntityLocation.cpp
        QuaternionProperty.cpp
        Vector3Property.cpp
        Point3Property.cpp
        SimpleTypeStore.cpp
        SolidProperty.cpp
        Domain.cpp
        BBoxProperty.cpp
        ScaleProperty.cpp
        Modifier.cpp
        PhysicalProperties.cpp)

target_link_directories(rulesbase PUBLIC
        ${wfmath_LIBRARY_DIR}
        ${Atlas_LIBARY_DIR}
        ${SigC++_LIBRARY_DIR}
        )
target_link_libraries(rulesbase PUBLIC
        ${wfmath_LIBRARIES}
        ${Atlas_LIBRARIES}
        ${SigC++_LIBRARIES}
        common
        )

target_include_directories(rulesbase PUBLIC
        ${wfmath_INCLUDE_DIR}
        ${Atlas_INCLUDE_DIR}
        ${SigC++_INCLUDE_DIR}
        )


add_subdirectory(python)
add_subdirectory(entityfilter)
add_subdirectory(ai)
add_subdirectory(simulation)




