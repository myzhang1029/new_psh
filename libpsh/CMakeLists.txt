include(GNUInstallDirs)

add_library(libpsh STATIC stringbuilder.c util.c xmalloc.c hash.c hasher.c)
# Name it libpsh here so that it won't collide with the executable
set_target_properties(libpsh PROPERTIES OUTPUT_NAME "psh")

install(FILES stringbuilder.h util.h xmalloc.h hash.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libpsh)
install(TARGETS libpsh DESTINATION ${CMAKE_INSTALL_LIBDIR})