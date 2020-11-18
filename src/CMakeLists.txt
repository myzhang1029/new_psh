include(GNUInstallDirs)

include("${CMAKE_SOURCE_DIR}/cmake/select_backend.cmake")

add_executable (psh builtins2.c command.c filpinfo.c input.c main.c parser.c prompts.c ${CMAKE_BINARY_DIR}/select_backend.c util.c variable.c builtins/builtin.c builtins/cd.c builtins/echo.c builtins/exit.c builtins/history.c builtins/pwd.c builtins/true.c)

include_directories(../include)
target_link_libraries(psh libpsh)
if(HAVE_READLINE)
    target_link_libraries(psh ${HAVE_READLINE})
endif()
if(HAVE_WORKING_HISTORY)
    target_link_libraries(psh ${HAVE_HISTORY})
endif()

install(TARGETS psh DESTINATION ${CMAKE_INSTALL_BINDIR})
