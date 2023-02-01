add_library(physics
        Vector3D.cpp
        BBox.cpp
        BBox.h
        Course.cpp
        Quaternion.cpp
        Shape.cpp
        Transform.cpp
        Convert.h
        Course_impl.h
        Shape_impl.h)

target_link_libraries(physics PUBLIC
        wfmath::wfmath
        Atlas::Atlas
        )

