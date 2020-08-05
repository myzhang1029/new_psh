// Do: gcc -g -fsanitize=address -Wall -Wextra -I.
// psh/test_prompts.c psh/prompts.c libpsh/stringbuilder.c libpsh/util.c
// libpsh/xmalloc.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *ps_expander(char *prompt);

int main()
{
    /* Say the date is Tue May 5 */
    char *prm1 = ps_expander("\\\\abxc\\b\\a\\d\\d\\d\\\\d");
    char intended1[] = "\\abxc\\b\aTue May 05Tue May 05Tue May 05\\d";
    char *prm2 = ps_expander("Okkkkkk");
    char intended2[] = "Okkkkkk";
    printf("1st: %d\n\t%s\n\t%s\n 2nd: %d\n\t%s\n\t%s\n",
           strcmp(prm1, intended1), prm1, intended1, strcmp(prm2, intended2),
           prm2, intended2);
    free(prm2);
    free(prm1);
    return 0;
}

char *pshstrdup(char *p) { return strdup(p); }
