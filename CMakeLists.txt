cmake_minimum_required(VERSION 3.0)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckTypeSize)
include(CheckLibraryExists)

project(psh VERSION 0.17.0
    LANGUAGES "C")
set(PROJECT_DESCRIPTION "P Shell")
#enable_testing()
include_directories(.)

# checks
check_include_files(readline/readline.h HAVE_READLINE_H)
check_include_files(readline/history.h HAVE_HISTORY_H)
check_library_exists(readline readline "" HAVE_READLINE)
check_library_exists(history add_history "" HAVE_HISTORY)
check_library_exists(edit readline "" HAVE_LIBEDIT)
check_type_size(size_t SIZE_T)
check_type_size(intptr_t INTPTR_T)
if(NOT HAVE_READLINE AND NOT HAVE_LIBEDIT)
    set(CMAKE_C_FLAGS "-DNO_READLINE ${CMAKE_C_FLAGS}")
    message(WARNING "No libreadline or libedit, disabling readline. If you think this is a mistake, try adding -DCMAKE_C_FLAGS_INIT=-I$location/include -DCMAKE_EXE_LD_FLAGS_INIT=-L$location/lib to the cmake command")
endif()
if(NOT HAVE_HISTORY_H OR NOT HAVE_HISTORY)
    set(CMAKE_C_FLAGS "-DNO_HISTORY ${CMAKE_C_FLAGS}")
    message(WARNING "No libhistory, disabling history. If you think this is a mistake, try adding -DCMAKE_C_FLAGS_INIT=-I$location/include -DCMAKE_EXE_LD_FLAGS_INIT=-L$location/lib to the cmake command")
endif()
if(NOT SIZE_T)
    set(size_t int)
endif()
if(NOT INTPTR_T)
    set(intptr_t long)
endif()

add_subdirectory(libpsh)
add_subdirectory(psh)

# pkg-config file
configure_file(libpsh.pc.in libpsh.pc @ONLY)

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


install(FILES ${CMAKE_BINARY_DIR}/libpsh.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
