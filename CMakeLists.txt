cmake_minimum_required(VERSION 3.2)

project(Cyphesis)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 14)

include(GNUInstallDirs)
include(FindPkgConfig)
include(CheckFunctionExists)
include(CheckIncludeFiles)
include(GetGitRevisionDescription)

# Version setup

set(CYPHESIS_VERSION_MAJOR 0)
set(CYPHESIS_VERSION_MINOR 7)
set(CYPHESIS_VERSION_PATCH 0)

set(VERSION ${CYPHESIS_VERSION_MAJOR}.${CYPHESIS_VERSION_MINOR}.${CYPHESIS_VERSION_PATCH})

# Set compiler flags
# Prevent GCC from messing up Python pointer handling
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-strict-aliasing")
add_definitions(-DHAVE_CONFIG_H)

include_directories("${PROJECT_SOURCE_DIR};${PROJECT_BINARY_DIR}")

enable_testing()

# Meta data
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
set(DESCRIPTION "A complete server for the Worldforge system.")
set(SOCKET_DIR ${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/tmp)
set(PACKAGE_BUGREPORT "erik@ogenvik.org")

#Macro for adding a test
macro(wf_add_test _TEST_NAME)

    add_executable(${_TEST_NAME}_test ${ARGN})
    target_link_libraries(${_TEST_NAME}_test ${LIBNAME})
    add_test(NAME ${_TEST_NAME} COMMAND ${_TEST_NAME}_test)

endmacro()

check_function_exists(getuid HAVE_GETUID)
check_function_exists(nice HAVE_NICE)
check_function_exists(sigaction HAVE_SIGACTION)
check_function_exists(fork HAVE_FORK)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(uname HAVE_UNAME)
check_function_exists(localtime_r HAVE_LOCALTIME_R)
check_function_exists(syslog HAVE_SYSLOG)
check_function_exists(usleep HAVE_USLEEP)

check_include_files(syslog.h HAVE_SYSLOG_H)
check_include_files(sys/utsname.h HAVE_SYS_UTSNAME_H)
check_include_files(sys/un.h HAVE_SYS_UN_H)
check_include_files(sys/wait.h HAVE_SYS_WAIT_H)
check_include_files(io.h HAVE_IO_H)
check_include_files(getopt.h HAVE_GETOPT_H)
check_include_files(dirent.h HAVE_DIRENT_H)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

pkg_check_modules(WF REQUIRED varconf-1.0>=0.6.7 mercator-0.4>=0.4.0 atlascpp-0.7>=0.7.0 wfmath-1.0>=1.0.3)
link_directories(${WF_LIBRARY_DIRS})
include_directories(${WF_INCLUDE_DIRS})
link_libraries(${WF_LIBRARIES})

message(STATUS ${WF_INCLUDE_DIRS})

pkg_check_modules(SIGCPP REQUIRED sigc++-2.0>=2.0.0)
link_directories(${SIGCPP_LIBRARY_DIRS})
include_directories(${SIGCPP_INCLUDE_DIRS})
link_libraries(${SIGCPP_LIBRARIES})

find_package(PythonLibs 2.6 REQUIRED)
link_directories(${PYTHON_LIBRARY_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

find_package(PythonInterp 2.6 REQUIRED)

find_package(GCrypt 1.2 REQUIRED)
link_directories(${GCRYPT_LIBRARY_DIRS})
include_directories(${GCRYPT_INCLUDE_DIRS})

#The default FindBullet.cmake macro doesn't look in pkg-config first, so we'll do that ourselves.
pkg_check_modules(BULLET bullet>=2.81)
if (NOT BULLET_FOUND)
       find_package(Bullet 2.81 REQUIRED)
endif (NOT BULLET_FOUND)

link_directories(${BULLET_LIBRARY_DIRS})
include_directories(${BULLET_INCLUDE_DIRS})

find_package(Avahi)
if (AVAHI_FOUND)
    link_directories(${AVAHI_LIBRARY_DIRS})
    include_directories(${AVAHI_INCLUDE_DIR})
    set(HAVE_AVAHI 1)
endif (AVAHI_FOUND)


find_package(PostgreSQL 7.1 REQUIRED)
link_directories(${PostgreSQL_LIBRARY_DIRS})
include_directories(${PostgreSQL_INCLUDE_DIRS})

find_package(Readline REQUIRED)
link_directories(${READLINE_LIBRARY_DIRS})
include_directories(${READLINE_INCLUDE_DIRS})

find_package(Boost
        1.46.0
        REQUIRED
        COMPONENTS filesystem system)

link_directories(${Boost_LIBRARY_DIRS})
link_libraries(${Boost_LIBRARIES})
include_directories(${Boost_INCLUDE_DIRS})

##TODO: check for binreloc?

#We'll use xmllint for validating schemas of some of our xml files.
find_program(XMLLINT xmllint)

configure_file(config.h.in config.h)

set(STUB_HEADERS "")

macro(wf_generate_stubs DIRECTORY)

    file(GLOB_RECURSE H_FILES RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/${DIRECTORY}/*.h")

    set(STUB_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/stubs)

    foreach(FILE ${H_FILES})
        set(STUB_FILE ${STUB_DIRECTORY}/stub${FILE}.h)
        set(STUB_CUSTOM_FILE ${STUB_DIRECTORY}/stub${FILE}.h)
        set(SOURCE_FILE "${CMAKE_SOURCE_DIR}/${FILE}")

        add_custom_command(TARGET stubs
            COMMAND ${CMAKE_COMMAND} -E echo_append "Generating stubs for ${FILE}\n"
            COMMAND ${PROJECT_SOURCE_DIR}/scripts/generator/generate_stub.py ${FILE} ${STUB_DIRECTORY}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

        list(APPEND STUB_HEADERS ${STUB_FILE})
    endforeach(FILE)

endmacro()

add_custom_target(stubs)
wf_generate_stubs(common)
wf_generate_stubs(modules)
wf_generate_stubs(navigation)
wf_generate_stubs(physics)
wf_generate_stubs(rulesets)
wf_generate_stubs(server)


# Add a "check" target, which builds and runs the tests.
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -E Benchmark)

# Add a "benchmark" target, which builds and runs the benchmarks.
add_custom_target(benchmark COMMAND ${CMAKE_CTEST_COMMAND} -R Benchmark)

add_subdirectory(physics)
add_subdirectory(common)
add_subdirectory(modules)
add_subdirectory(rulesets)
add_subdirectory(navigation)
add_subdirectory(metaserverapi)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(aiclient)
add_subdirectory(data)
add_subdirectory(tools)
add_subdirectory(man)
#Only build tests if the "check" target is used.
add_subdirectory(tests EXCLUDE_FROM_ALL)

#TODO: Implement schema validation for tests only
#file(GLOB MODELDEFINITIONS_FILES data/dural/*.modeldef)
#foreach (FILE ${MODELDEFINITIONS_FILES})
#    execute_process(COMMAND "${XMLLINT}" --schema "${PROJECT_SOURCE_DIR}/data/modeldefinition.xsd" --nonet --noout "${FILE}" RESULT_VARIABLE CMDRESULT)
#    if (CMDRESULT)
#        message(FATAL_ERROR "xmllint failure: ${CMDRESULT}")
#    endif (CMDRESULT)
#endforeach (FILE MODELDEFINITIONS_FILES)
#
#

configure_file(Doxyfile.in Doxyfile @ONLY)
configure_file(Doxyfile-python.in Doxyfile-python @ONLY)

install(FILES README COPYING AUTHORS THANKS NEWS FIXME DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/cyphesis-${VERSION})
install(DIRECTORY DESTINATION ${SOCKET_DIR})

## Doxygen support, exports a "docs" target.

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

    add_custom_target(docs DEPENDS ${DOXYGEN_OUTPUT})

endif (DOXYGEN_FOUND)

#add_custom_command(
#        OUTPUT ChangeLog
#        COMMAND ${CMAKE_SOURCE_DIR}/scripts/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR} f12012e7616c191a8926432faf866c8e43854062
#)
#add_custom_target(changelog DEPENDS ChangeLog)


# Packaging

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Worldforge")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README")
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
