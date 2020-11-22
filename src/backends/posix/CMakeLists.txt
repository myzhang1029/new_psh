include(GNUInstallDirs)

add_library(psh_backend STATIC posix.c ../posix2/misc_impl.c)
