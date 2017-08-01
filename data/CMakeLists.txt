configure_file(cyphesis.vconf.in cyphesis.vconf @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cyphesis.vconf DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/cyphesis)

macro(wf_install_ruleset RULESET)
    install(DIRECTORY ${RULESET}/ DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/cyphesis/${RULESET}.d)
endmacro()

wf_install_ruleset(acorn)
wf_install_ruleset(deeds)
wf_install_ruleset(mars)
wf_install_ruleset(mason)
wf_install_ruleset(minimal)
wf_install_ruleset(werewolf)
