include(GNUInstallDirs)

add_executable (psh show.c builtins.c util.c filpinfo.c select_backend.c builtins/history.c builtins/exec.c builtins/exit.c builtins/true.c builtins/pwd.c builtins/builtin.c builtins/cd.c builtins/misc.c builtins/echo.c input.c main.c parser.c)

include_directories(.)
target_link_libraries(psh libpsh)
target_link_libraries(psh readline)
target_link_libraries(psh history)

install(TARGETS psh DESTINATION ${CMAKE_INSTALL_BINDIR})
