install(FILES
    basic/scripts/editor.py
    DESTINATION ${CMAKE_INSTALL_DATADIR}/cyphesis/scripts/cyphesis)
install(DIRECTORY
    deeds
    DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/cyphesis/rulesets)
install(DIRECTORY
    basic/scripts/mind
    DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/cyphesis/rulesets/basic/scripts)



