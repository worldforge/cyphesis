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


target_link_directories(physics PUBLIC
        ${wfmath_LIBRARY_DIR}
        ${Atlas_LIBARY_DIR}
        )
target_link_libraries(physics PUBLIC
        ${wfmath_LIBRARIES}
        ${Atlas_LIBRARIES}
        )

target_include_directories(physics PUBLIC
        ${wfmath_INCLUDE_DIR}
        ${Atlas_INCLUDE_DIR}
        )

