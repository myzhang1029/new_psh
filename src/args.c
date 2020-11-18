#include <unistd.h>

#include "libpsh/util.h"
#include "backend.h"

/* Set variable thats globally avaliable */
int verbose = 0;

void setargs(int argc, char **argv, const char *optstring, char *argv0)
{

    int arg;

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
