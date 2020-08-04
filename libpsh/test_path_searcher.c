/* Test for stringbuilder, xmaloc and psh_strncpy
 * do `gcc -Wall -Wextra -I. -g -fsanitize=address libpsh/test_path_searcher.c
 * libpsh/path_searcher.c libpsh/xmalloc.c`
 */
#include <stdio.h>
#include <string.h>

#include "libpsh/path_searcher.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"

static int chk_func(const char *s) { return !strcmp(s, "uuuu"); }

int main()
{
    char *res = psh_search_path("uuu:ppp", ':', "u", chk_func);
    printf("%s\n", res);
    xfree(res);
    return 0;
}