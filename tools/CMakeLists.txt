add_subdirectory(pythondocsgenerator)
configure_file(cyphesis.vconf.in cyphesis.vconf @ONLY)
configure_file(systemd/cyphesis.tmpl.service systemd/cyphesis.service @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cyphesis.vconf DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/cyphesis)