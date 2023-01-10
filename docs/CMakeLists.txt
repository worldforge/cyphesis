add_subdirectory(man)

#Add a custom target which regenerates the Python stubs in the "dox/python" directory based on the annotations we put in our C++ bindings.
#This target isn't run automatically because we don't want anything that alters the source code to do so.
#That means that you need to don't forget to run this yourself whenever you update the bindings.
add_custom_target(GeneratePythonDocs
        COMMENT "Generating Python docs stubs."
        COMMAND pythondocgenerator atlas > atlas.py
        COMMAND pythondocgenerator server > server.py
        COMMAND pythondocgenerator entity_filter > entity_filter.py
        COMMAND pythondocgenerator common > common.py
        COMMAND pythondocgenerator ai > ai.py
        COMMAND pythondocgenerator physics > physics.py
        COMMAND pythondocgenerator rules > rules.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/python
        DEPENDS pythondocgenerator rulesai_python rulessimulation_python entityfilter_python rulesbase_python
        VERBATIM
        )

