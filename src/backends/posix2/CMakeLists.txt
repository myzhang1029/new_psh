include(GNUInstallDirs)

add_library(psh_backend STATIC misc_impl.c builtin_exec.c run.c lifecycle.c)