cmake_minimum_required(VERSION 3.12)

project(Cyphesis)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include(GNUInstallDirs)
include(FindPkgConfig)
include(CheckFunctionExists)
include(CheckIncludeFiles)
include(GetGitRevisionDescription)
include(CMakeDependentOption)

# Version setup

set(CYPHESIS_VERSION_MAJOR 0)
set(CYPHESIS_VERSION_MINOR 7)
set(CYPHESIS_VERSION_PATCH 0)

set(VERSION ${CYPHESIS_VERSION_MAJOR}.${CYPHESIS_VERSION_MINOR}.${CYPHESIS_VERSION_PATCH})

# Set compiler flags
set(WF_WARNING_FLAGS -Wall -Winit-self -Wcast-qual -Wwrite-strings -Wextra -Wundef -Wmissing-declarations -Wno-unused-parameter -Wshadow -Wno-missing-field-initializers -Wno-long-long)

add_definitions(-DHAVE_CONFIG_H)
add_definitions(-DENABLE_BINRELOC)

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/external ${PROJECT_BINARY_DIR}/generated)

enable_testing()

# Meta data
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
set(DESCRIPTION "The Worldforge server.")
set(SOCKET_DIR ${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/tmp)
set(PACKAGE_BUGREPORT "erik@ogenvik.org")

check_function_exists(syslog HAVE_SYSLOG)

check_include_files(syslog.h HAVE_SYSLOG_H)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

#If we're using Conan this won't set the Python3_EXECUTABLE environment variables as expected, which is why we do that in conanfile.py
find_package(Python3 COMPONENTS Development Interpreter REQUIRED)

find_package(Atlas 0.7.0 REQUIRED)
find_package(varconf 1.0.3 REQUIRED)
find_package(mercator 0.4.0 REQUIRED)
find_package(wfmath 1.0.3 REQUIRED)
find_package(Squall 0.1.0 REQUIRED)

find_package(sigc++-3 3.0 REQUIRED)
find_package(libgcrypt 1.2 REQUIRED)
find_package(Bullet 2.81 REQUIRED)

find_package(Avahi)
if (AVAHI_FOUND)
    link_directories(${AVAHI_LIBRARY_DIRS})
    include_directories(${AVAHI_INCLUDE_DIR})
    set(HAVE_AVAHI 1)
endif (AVAHI_FOUND)

find_package(PostgreSQL 7.1)
cmake_dependent_option(CYPHESIS_USE_POSTGRES "Use PostgreSQL for storage" FALSE "PostgreSQL_FOUND" FALSE)

find_package(SQLite3 REQUIRED)

find_package(libxdg-basedir REQUIRED)

find_package(readline REQUIRED)
find_package(Boost
        1.46.0
        REQUIRED
        COMPONENTS filesystem system)

set(ASSETS_DIR assets-${VERSION})
set(MEDIAREPO_DIR ${PROJECT_SOURCE_DIR}/mediarepo)
add_compile_definitions("CYPHESIS_RAW_ASSETS_DIRECTORY=\"${MEDIAREPO_DIR}/trunk/assets\"")

find_program(SUBVERSION_CMD NAMES svn)
find_program(RSYNC_CMD NAMES rsync)

#It would be better if Bullet provided a header which specified how it was built...
OPTION(BT_USE_DOUBLE_PRECISION "Using double precision with Bullet. This should match what Bullet has been compiled with." OFF)

if (BT_USE_DOUBLE_PRECISION)
    message(STATUS "Using double precision with Bullet. Make sure that Bullet is compiled with this too.")
    add_definitions(-DBT_USE_DOUBLE_PRECISION=1)
endif ()

#You can specify an exernal installation of the Worlds though WORLDFORGE_WORLDS_PATH environment variable. If that's not specified,
#Worlds data will be installed as specified by the WORLDFORGE_WORLDS_SOURCE_PATH environment variable (which is set by Conan).
if (WORLDFORGE_WORLDS_PATH)
    message(STATUS "An installation of the Worldforge Worlds has been specified through the WORLDFORGE_WORLDS_PATH environment variable: ${WORLDFORGE_WORLDS_PATH}.")
else ()
    set(WORLDFORGE_WORLDS_PATH "${CMAKE_INSTALL_PREFIX}/share/cyphesis/worlds")

    if (WORLDFORGE_WORLDS_SOURCE_PATH)
        install(DIRECTORY ${WORLDFORGE_WORLDS_SOURCE_PATH}/ DESTINATION ${WORLDFORGE_WORLDS_PATH})
        message(STATUS "Added installation of the Worldforge Worlds from '${WORLDFORGE_WORLDS_SOURCE_PATH}' to the 'install' target.")
    else ()
        message(STATUS "No Worldforge Worlds installation has been specified (through the WORLDFORGE_WORLDS_SOURCE_PATH environment variable); we will work on the assumption that there's an installation already at '${WORLDFORGE_WORLDS_PATH}'.")
    endif ()
endif ()

#add_custom_target(assets-download)
#if (RSYNC_CMD)
#    add_custom_command(
#            TARGET assets-download
#            COMMAND ${CMAKE_COMMAND} -E echo "I will now use rsync to install the assets from amber.worldforge.org into ${ASSETS_DIR}."
#            COMMAND ${CMAKE_COMMAND} -E make_directory ${ASSETS_DIR}
#            #We'll for now only copy ".mesh" files.
#            COMMAND rsync -rtzu --include='*.mesh' --include='*/' --exclude='*' amber.worldforge.org::ember-media/ember-media-0.8.0/media/assets ${ASSETS_DIR}
#            COMMAND ${CMAKE_COMMAND} -E echo "Copying ${ASSETS_DIR} to ${CMAKE_INSTALL_FULL_DATADIR}/cyphesis/assets."
#            COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${CMAKE_INSTALL_FULL_DATADIR}/cyphesis/
#            COMMAND ${CMAKE_COMMAND} -E echo "Done."
#            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
#    )
#else (RSYNC_CMD)
#    add_custom_command(
#            TARGET assets-download
#            COMMAND ${CMAKE_COMMAND} -E echo "Could not find the Rsync command. The target 'assets-download' is disabled."
#    )
#endif (RSYNC_CMD)


add_custom_target(mediarepo-checkout)
if (SUBVERSION_CMD)
    add_custom_command(
            TARGET mediarepo-checkout
            COMMAND ${CMAKE_COMMAND} -E echo "Using Subversion to checkout https://svn.worldforge.org:886/svn/media/trunk to ${MEDIAREPO_DIR}/trunk."
            COMMAND ${CMAKE_COMMAND} -E make_directory ${MEDIAREPO_DIR}
            COMMAND ${SUBVERSION_CMD} co https://svn.worldforge.org:886/svn/media/trunk ${MEDIAREPO_DIR}/trunk
    )
else (SUBVERSION_CMD)
    add_custom_command(
            TARGET mediarepo-checkout
            COMMAND ${CMAKE_COMMAND} -E echo "Could not find the Subversion command 'svn'. The target 'mediarepo-checkout' is disabled."
    )
endif (SUBVERSION_CMD)

set(MEDIAREPO_PROCESSED_DIR ${CMAKE_INSTALL_PREFIX}/share/cyphesis/assets)
if (NOT MEDIA_PROCESS_TEXTURE_MAX_SIZE)
    set(MEDIA_PROCESS_TEXTURE_MAX_SIZE 512)
endif ()

add_custom_target(media-process-install)
if (Python3_EXECUTABLE)
    add_custom_command(
            TARGET media-process-install
            COMMAND ${CMAKE_COMMAND} -E echo "Processing media in '${MEDIAREPO_DIR}/trunk' and placing it in '${MEDIAREPO_PROCESSED_DIR}'."
            COMMAND ${CMAKE_COMMAND} -E echo "NOTE: this will make Cyphesis use the processed media in '${MEDIAREPO_PROCESSED_DIR}' instead of the raw media in '${MEDIAREPO_DIR}/trunk'."
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/scripts/make_dist_media.py ${MEDIAREPO_DIR}/trunk ${MEDIAREPO_PROCESSED_DIR} ${MEDIA_PROCESS_TEXTURE_MAX_SIZE}
    )
else (Python3_EXECUTABLE)
    add_custom_command(
            TARGET media-process-install
            COMMAND ${CMAKE_COMMAND} -E echo "Could not find the Python command 'python'. The target 'mediarepo-process' is disabled."
    )
endif (Python3_EXECUTABLE)


##TODO: check for binreloc?

#We'll use xmllint for validating schemas of some of our xml files.
find_program(XMLLINT xmllint)

set(STUB_HEADERS "")

macro(wf_generate_stubs DIRECTORY)

    file(GLOB_RECURSE H_FILES RELATIVE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/${DIRECTORY}/*.h")

    set(STUB_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/stubs)

    foreach (FILE ${H_FILES})
        set(STUB_FILE ${STUB_DIRECTORY}/stub${FILE}.h)
        set(STUB_CUSTOM_FILE ${STUB_DIRECTORY}/stub${FILE}.h)
        set(SOURCE_FILE "${CMAKE_SOURCE_DIR}/src/${FILE}")

        add_custom_command(TARGET stubs
                COMMAND ${CMAKE_COMMAND} -E echo "Generating stubs for ${FILE}\n"
                COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/scripts/generator/generate_stub.py ${FILE} ${STUB_DIRECTORY}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src
        )

        list(APPEND STUB_HEADERS ${STUB_FILE})
    endforeach (FILE)

endmacro()


# Add a "check" target, which builds and runs the tests.
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -E Benchmark)

# Add a "benchmark" target, which builds and runs the benchmarks.
add_custom_target(benchmark COMMAND ${CMAKE_CTEST_COMMAND} -R Benchmark)

configure_file(tools/Version.tmpl.h generated/Version.h @ONLY)
configure_file(tools/config.tmpl.h generated/config.h @ONLY)


add_subdirectory(src)
add_subdirectory(external)
add_subdirectory(data)
add_subdirectory(tools)
add_subdirectory(docs)
#Only build tests if the "check" target is used.
add_subdirectory(tests EXCLUDE_FROM_ALL)

configure_file(docs/Doxyfile.in Doxyfile @ONLY)

install(FILES README.md COPYING AUTHORS THANKS NEWS DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/cyphesis)
install(DIRECTORY DESTINATION ${SOCKET_DIR})

## Doxygen support, exports a "dox" target.

find_package(Doxygen)

if (DOXYGEN_FOUND)

    set(DOXYGEN_OUTPUT doc)

    add_custom_command(
            OUTPUT ${DOXYGEN_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-python
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-python
    )

    add_custom_target(dox DEPENDS ${DOXYGEN_OUTPUT})

endif (DOXYGEN_FOUND)

add_custom_command(
        OUTPUT ChangeLog
        COMMAND ${CMAKE_SOURCE_DIR}/tools/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR} cce66ef4a196a4871ebaebe82cd91c8dd733be13
)
add_custom_target(changelog DEPENDS ChangeLog)

# Packaging

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Worldforge")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${CYPHESIS_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CYPHESIS_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${CYPHESIS_VERSION_PATCH}")
#set(CPACK_INSTALL_SCRIPT "sh ${CMAKE_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CPACK_PACKAGE_INSTALL_DIRECTORY} 8bd480b053190ffde2afe33af66f484953036f5a")

set(CPACK_SOURCE_GENERATOR TBZ2 ZIP)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION}" CACHE INTERNAL "tarball basename")

set(CPACK_SOURCE_IGNORE_FILES
        # no hidden files
        "/\\\\..+$"
        "~$"
)

include(CPack)
