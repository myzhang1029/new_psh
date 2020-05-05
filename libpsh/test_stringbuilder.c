/* Test for stringbuilder, xmaloc and psh_strncpy
 * do `gcc -DNO_READLINE -Wall -Wextra -I. -g -fsanitize=address
 * libpsh/test_stringbuilder.c libpsh/util.c libpsh/stringbuilder.c
 * libpsh/xmalloc.c`
 */
#include <stdio.h>
#include <string.h>

#include "libpsh/stringbuilder.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"

int main()
{
    char str1[6] = "abcde";
    char *str2 = xmalloc(P_CS * 6);
    char intended[] = "ababcabcde";
    char *result;
    psh_stringbuilder *builder = psh_stringbuilder_create();
    psh_strncpy(str2, str1, 5);
    psh_stringbuilder_add_length(builder, str1, 2, 0);
    psh_stringbuilder_add_length(builder, str2, 3, 1);
    psh_stringbuilder_add(builder, str1, 0);
    result = psh_stringbuilder_yield(builder);
    psh_stringbuilder_free(builder);
    printf("%s: %s\n", result,
           strcmp(result, intended) == 0 ? "matches" : "doesn't match");
    xfree(result);
    return 0;
}
