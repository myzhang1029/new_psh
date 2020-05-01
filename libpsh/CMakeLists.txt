include(GNUInstallDirs)

add_library(libpsh STATIC stringbuilder.c util.c xmalloc.c hash.c hasher.c)

install(FILES stringbuilder.h util.h xmalloc.h hash.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libpsh)
