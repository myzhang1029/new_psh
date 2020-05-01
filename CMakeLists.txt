cmake_minimum_required(VERSION 3.0)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckTypeSize)

project(psh VERSION 0.14.0
    LANGUAGES "C")
set(PROJECT_DESCRIPTION "P Shell")
#enable_testing()
include_directories(.)
add_subdirectory(libpsh)
add_subdirectory(psh)

# checks
check_include_files(readline/readline.h HAVE_READLINE_H)
check_type_size(size_t SIZE_T)
if(NOT SIZE_T)
    set(size_t int)
endif()

# pkg-config file
#configure_file(sbl.pc.in sbl.pc @ONLY)

# config.h
#configure_file(config.h.in config.h)
#set(CMAKE_CPP_FLAGS "-DHAVE_CONFIG_H")

# distclean target
#add_custom_target(distclean
#    COMMAND ${CMAKE_BUILD_TOOL} clean
#    COMMAND ${CMAKE_COMMAND} -P
#    ${CMAKE_SOURCE_DIR}/cmake/distclean.cmake
#    COMMAND ${CMAKE_COMMAND} -DRUNNING_DIR=test -P
#    ${CMAKE_SOURCE_DIR}/cmake/distclean.cmake
#    COMMAND ${CMAKE_COMMAND} -DRUNNING_DIR=src -P
#    ${CMAKE_SOURCE_DIR}/cmake/distclean.cmake)
# uninstall target
#configure_file(
#    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
#    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
#    IMMEDIATE @ONLY)
#add_custom_target(uninstall
#    COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake")


install(FILES include/slib.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY include/slib DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
#install(FILES ${CMAKE_BINARY_DIR}/libpsh.pc
#    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
