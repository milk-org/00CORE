project(lib_00CORE_project)

include_directories ("${PROJECT_SOURCE_DIR}/src")
include_directories ("${PROJECT_SOURCE_DIR}/..")
add_library(00CORE SHARED 00CORE.c)

install(TARGETS 00CORE DESTINATION lib)
install(FILES 00CORE.h DESTINATION include)

