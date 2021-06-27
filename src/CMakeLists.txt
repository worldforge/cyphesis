add_compile_options(${_LIB_NAME} ${WF_WARNING_FLAGS})

add_subdirectory(physics)
add_subdirectory(common)
add_subdirectory(modules)
add_subdirectory(rules)
add_subdirectory(navigation)
add_subdirectory(metaserverapi)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(tools)
add_subdirectory(pythonbase)

