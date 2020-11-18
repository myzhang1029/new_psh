cmake_minimum_required(VERSION 3.12)

include(CheckIncludeFiles)
include(CheckTypeSize)
include(CheckSymbolExists)

project(psh VERSION 0.18.0
    LANGUAGES "C")
set(PROJECT_DESCRIPTION "P Shell")
#enable_testing()
include_directories(./include)

# checks
# Check for libreadline
find_file(HAVE_READLINE_READLINE_H readline/readline.h)
find_library(HAVE_READLINE_GNU NAMES readline)
find_library(HAVE_READLINE_ALTS NAMES edit)
if(HAVE_READLINE_ALTS)
	set(HAVE_READLINE ${HAVE_READLINE_ALTS})
endif()
if(HAVE_READLINE_GNU)
	set(HAVE_READLINE ${HAVE_READLINE_GNU})
endif()
if(HAVE_READLINE_READLINE_H)
    string(REPLACE "readline/readline.h" "" INCLUDE_READLINE ${HAVE_READLINE_READLINE_H})
	include_directories(${INCLUDE_READLINE})
endif()
if(NOT HAVE_READLINE)
    message(WARNING "No libreadline or libedit, disabling readline. If you think this is a mistake, try adding -DCMAKE_INCLUDE_PATH=$location/include -DCMAKE_LIBRARY_PATH=$location/lib to the cmake command")
endif()

# Check for libhistory
find_file(HAVE_READLINE_HISTORY_H readline/history.h)
find_library(HAVE_HISTORY NAMES history edit)
if(HAVE_READLINE_HISTORY_H)
    string(REPLACE "readline/history.h" "" INCLUDE_HISTORY ${HAVE_READLINE_HISTORY_H})
	include_directories(${INCLUDE_HISTORY})
endif()
# Check if libhistory has history_list()
if(HAVE_READLINE_HISTORY_H AND HAVE_HISTORY)
	set(CMAKE_REQUIRED_LIBRARIES "${HAVE_HISTORY}")
	set(CMAKE_REQUIRED_INCLUDES ${INCLUDE_HISTORY})
	check_symbol_exists("history_list" "stdio.h;readline/history.h" HAVE_WORKING_HISTORY)
endif()
if(NOT HAVE_WORKING_HISTORY)
    message(WARNING "No libhistory, disabling history. If you think this is a mistake, try adding -DCMAKE_INCLUDE_PATH=$location/include -DCMAKE_LIBRARY_PATH=$location/lib to the cmake command")
endif()

check_type_size(size_t SIZE_T)
check_type_size(intptr_t INTPTR_T)
if(NOT SIZE_T)
    set(size_t int)
endif()
if(NOT INTPTR_T)
    set(intptr_t long)
endif()

# pkg-config file
configure_file(psh.pc.in psh.pc @ONLY)

# config.h
configure_file(config.h.cm config.h)
set(CMAKE_C_FLAGS "-I${CMAKE_BINARY_DIR} -DHAVE_CONFIG_H ${CMAKE_C_FLAGS}")

add_subdirectory(lib)
add_subdirectory(src)

# distclean target
add_custom_target(distclean
    ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -D "subdirs=.\;src\;lib\;src/backends/posix2" -P
    ${CMAKE_SOURCE_DIR}/cmake/distclean.cmake)
# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake")


install(FILES ${CMAKE_BINARY_DIR}/psh.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
