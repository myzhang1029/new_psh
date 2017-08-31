#include "builtin.h"

int builtin_echo(char *command, char **parameters)
{
    if(parameters[1] == NULL)
    {
        /* A blank line */
        printf("\n");
        return 1;
    }
    else if(parameters[1][0] == '-')
    {
        switch(parameters[1][1])
        {
            case 0:
                /* Another blank line */
                puts("");
                return 1;
            case 'n':
                if(parameters[2] == NULL)
                    /* No more blank line */
                    return 1;
                else
                {
                    int cnt=2;
                    printf("%s", parameters[cnt]);
                    while (parameters[++cnt] != NULL)
                    {
                        printf(" %s", parameters[cnt]);
                    }
                    return 1;
                }
            default:
            {
                int cnt=1;
                printf("%s", parameters[cnt]);
                while (parameters[++cnt] != NULL)
                {
                    printf(" %s", parameters[cnt]);
                    cnt++;
                }
                puts("");
                return 1;
            }
        }/* switch-case */
    }
    else
    {
        int cnt=1;
        printf("%s", parameters[cnt]);
        while (parameters[++cnt] != NULL)
        {
            printf(" %s", parameters[cnt]);
            cnt++;
        }
        puts("");
        return 1;
    }
}
