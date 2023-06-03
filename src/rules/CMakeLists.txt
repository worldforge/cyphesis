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

target_link_libraries(rulesbase PUBLIC
        wfmath::wfmath
        Atlas::Atlas
        libsigcpp::sigc++
        common
        )


add_subdirectory(python)
add_subdirectory(entityfilter)
add_subdirectory(ai)
add_subdirectory(simulation)




