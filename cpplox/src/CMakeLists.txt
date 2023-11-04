add_executable(cpplox)
file(GLOB _sources CONFIGURE_DEPENDS *.cpp)
target_sources(cpplox PRIVATE ${_sources})

