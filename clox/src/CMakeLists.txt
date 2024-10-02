add_executable(clox)
file(GLOB _sources CONFIGURE_DEPENDS *.c)
target_sources(clox PRIVATE ${_sources})
