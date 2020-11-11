include(GNUInstallDirs)

add_library(libpsh STATIC path_searcher.c stringbuilder.c util.c xmalloc.c hash.c hasher.c)
# Name it libpsh here so that it won't collide with the executable
set_target_properties(libpsh PROPERTIES OUTPUT_NAME "psh")

install(DIRECTORY ../include/libpsh DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS libpsh DESTINATION ${CMAKE_INSTALL_LIBDIR})
