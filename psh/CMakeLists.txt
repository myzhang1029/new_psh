include(GNUInstallDirs)

include("${CMAKE_SOURCE_DIR}/cmake/select_backend.cmake")

add_executable (psh builtins.c command.c filpinfo.c input2.c main2.c parser.c prompts.c ${CMAKE_BINARY_DIR}/select_backend.c util.c variable.c builtins/builtin.c builtins/cd2.c builtins/echo.c builtins/exit.c builtins/history.c builtins/pwd.c builtins/true.c)

include_directories(.)
target_link_libraries(psh libpsh)
if(HAVE_READLINE)
    target_link_libraries(psh ${HAVE_READLINE})
endif()
if(HAVE_WORKING_HISTORY)
    target_link_libraries(psh ${HAVE_HISTORY})
endif()

install(TARGETS psh DESTINATION ${CMAKE_INSTALL_BINDIR})
