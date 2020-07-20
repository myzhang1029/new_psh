cmake_minimum_required(VERSION 3.12)

include(CheckIncludeFiles)
include(CheckTypeSize)
include(CheckSymbolExists)

project(psh VERSION 0.18.0
    LANGUAGES "C")
set(PROJECT_DESCRIPTION "P Shell")
#enable_testing()
include_directories(.)

# checks
find_file(HAVE_READLINE_H readline/readline.h)
find_file(HAVE_HISTORY_H readline/history.h)
find_library(LIBREADLINE NAMES readline edit)
find_library(LIBHISTORY NAMES history edit)
string(REPLACE "readline/readline.h" "" INCLUDE_READLINE ${HAVE_READLINE_H})
string(REPLACE "readline/history.h" "" INCLUDE_HISTORY ${HAVE_HISTORY_H})
include_directories(${INCLUDE_READLINE} ${INCLUDE_HISTORY})
# Check if libhistory has history_list()
if(HAVE_HISTORY_H AND LIBHISTORY)
	set(CMAKE_REQUIRED_LIBRARIES "${LIBHISTORY}")
	set(CMAKE_REQUIRED_INCLUDES ${INCLUDE_READLINE} ${INCLUDE_HISTORY})
	check_symbol_exists("history_list" "stdio.h;readline/history.h" LIBHISTORY_WORKS)
endif()
check_type_size(size_t SIZE_T)
check_type_size(intptr_t INTPTR_T)
if(NOT LIBREADLINE)
    set(CMAKE_C_FLAGS "-DNO_READLINE ${CMAKE_C_FLAGS}")
    message(WARNING "No libreadline or libedit, disabling readline. If you think this is a mistake, try adding -DCMAKE_INCLUDE_PATH=$location/include -DCMAKE_LIBRARY_PATH=$location/lib to the cmake command")
endif()
if(NOT LIBHISTORY_WORKS)
    set(CMAKE_C_FLAGS "-DNO_HISTORY ${CMAKE_C_FLAGS}")
    message(WARNING "No libhistory, disabling history. If you think this is a mistake, try adding -DCMAKE_INCLUDE_PATH=$location/include -DCMAKE_LIBRARY_PATH=$location/lib to the cmake command")
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
configure_file(config.h.in config.h)
set(CMAKE_CPP_FLAGS "-DHAVE_CONFIG_H")

# distclean target
add_custom_target(distclean
    ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -D "subdirs=.\;psh\;libpsh" -P
    ${CMAKE_SOURCE_DIR}/cmake/distclean.cmake)
# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake")


install(FILES ${CMAKE_BINARY_DIR}/libpsh.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
