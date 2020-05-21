include(GNUInstallDirs)

add_executable (psh prompts.c builtins.c util.c filpinfo.c select_backend.c builtins/history.c builtins/exit.c builtins/true.c builtins/pwd.c builtins/builtin.c builtins/cd.c builtins/misc.c builtins/echo.c input2.c main2.c parser.c)

include_directories(.)
target_link_libraries(psh libpsh)
if(HAVE_READLINE)
    target_link_libraries(psh readline)
elseif(HAVE_LIBEDIT)
    target_link_libraries(psh edit)
endif()
if(HAVE_HISTORY_H AND HAVE_HISTORY)
    target_link_libraries(psh history)
endif()

install(TARGETS psh DESTINATION ${CMAKE_INSTALL_BINDIR})
