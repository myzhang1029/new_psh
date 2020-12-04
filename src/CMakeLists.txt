include(GNUInstallDirs)

include("${CMAKE_SOURCE_DIR}/cmake/select_backend.cmake")

add_executable (psh args.c builtins.c command.c expansion.c filpinfo.c input.c main.c parser.c prompts.c util.c variable.c builtins/builtin.c builtins/cd.c builtins/echo.c builtins/exit.c builtins/hash.c builtins/help.c builtins/history.c builtins/pwd.c builtins/true.c)

include_directories(../include)
target_link_libraries(psh libpsh)
target_link_libraries(psh psh_backend)
if(HAVE_READLINE)
    target_link_libraries(psh ${HAVE_READLINE})
endif()
if(HAVE_WORKING_HISTORY)
    target_link_libraries(psh ${HAVE_HISTORY})
endif()

install(TARGETS psh DESTINATION ${CMAKE_INSTALL_BINDIR})
