#include "backend.h"
#include "libpsh/util.h"

/* Set variable thats globally avaliable */
int verbose = 0;
extern int optopt;
extern char *argv0;

void parse_shell_args(int argc, char **argv)
{
    int arg;
    const char *optstring = ":v";

    /* Parse shell options */
    while ((arg = psh_backend_getopt(argc, argv, optstring)) != -1)
    {
        switch (arg)
        {
            /* Verbose flag */
            case 'v':
                verbose = 1;
                break;
            case ':':
                OUT2E("%s: option requires an argument\n", argv0);
                break;
            case '?':
            default:
                OUT2E("%s: unknown option -%c\n", argv0, optopt);
                break;
        }
    }
}
