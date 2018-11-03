add_library(rulesbase
    LocatedEntity.cpp
    EntityProperties.cpp
    AtlasProperties.cpp
    Script.cpp
    Location.cpp
    EntityLocation.cpp
    QuaternionProperty.cpp
    Vector3Property.cpp
    SimpleTypeStore.cpp
    SolidProperty.cpp
    Domain.cpp
    SimpleProperty.cpp
    SimpleProperty.h)
add_subdirectory(python)
add_subdirectory(entityfilter)
add_subdirectory(ai)
add_subdirectory(simulation)




