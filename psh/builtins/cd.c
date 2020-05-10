/*
   cd.c - builtin cd

   Copyright 2013 wuyve.
   Copyright 2017 Zhang Maiyun.
*/

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "builtin.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"

static int create_new_pwd(char **cd_dir)
{
    if (!cd_dir || !(*cd_dir))
        return 1;
    if ((*cd_dir)[0] == '/') /* cd_dir is an abs path */
    {
        char *delim;
        while ((delim = strstr((*cd_dir), "/.")))
        {
            if (*(delim + 2) == 0) /* /. */
            {
                *delim = 0;
            }
            else if (*(delim + 2) == '/') /* /./ */
            {
                memmove(delim, delim + 2, strlen(delim + 2) + 1);
            }
            else if (delim == *cd_dir) /* root/.. */
            {
                if (*(delim + 3) == 0)
                    *(delim + 1) = 0;
                else
                    memmove(delim, delim + 3, strlen(delim + 3) + 1);
            }
            else if (*(delim + 3) == 0) /* /..\0 */
            {
                char *lastnode;
                *delim = 0; /* Terminate the string at the
                           current / */

                lastnode = strrchr(*cd_dir, '/');
                if (lastnode == *cd_dir) /* Root node reached */
                    *(lastnode + 1) = 0;
                else
                    *lastnode = 0; /* Then terminate the
                              string at the last /
                            */
            }
            else /* /../ */
            {
                char oldval = *delim;
                char *lastnode;
                *delim = 0;
                lastnode = strrchr(*cd_dir, '/');
                *delim = oldval;
                memmove(lastnode, delim + 3, strlen(delim + 3) + 1);
            }
        } /* while */
    }
    else
    {
        char *oldpwd = getenv("PWD");
        *cd_dir = realloc(*cd_dir, P_CS * (strlen(*cd_dir) + 1 /*\0*/ +
                                           strlen(oldpwd) + 1 /*'/'*/));
        if (!cd_dir)
        {
            OUT2E("%s: create_new_pwd: realloc failed\n", argv0);
            return 1;
        }
        {
            int count = strlen(*cd_dir) + 1 /*\0*/;
            char *d = (*cd_dir) + strlen(oldpwd) + 1 /*'/'*/ + count - 1;
            char *s = (*cd_dir) + count - 1;
            while (count--)
                *d-- = *s--;
        }
        memmove(*cd_dir, oldpwd, strlen(oldpwd));
        (*cd_dir)[strlen(oldpwd)] = '/';
    }
    return 0;
}

int builtin_cd(ARGS)
{
    char *cd_path = NULL;
    int argc = get_argc(bltin_argv);
    if (argc == 1) /* 'cd', the same as cd $HOME */
    {
        char *homedir = getenv("HOME");
        if (!homedir)
        {
            OUT2E("%s: %s: HOME not set\n", argv0, bltin_argv[0]);
            return 2;
        }
        cd_path = malloc(P_CS * (strlen(homedir) + 1));
        if (cd_path == NULL)
        {
            OUT2E("%s: malloc failed: %s\n", bltin_argv[0], strerror(errno));
            return 2;
        }
        strcpy(cd_path, homedir);
    }
    else if (strcmp(bltin_argv[1], "-") ==
             0) /* 'cd -', the same as cd $OLDPWD*/
    {
        char *oldpwd = getenv("OLDPWD");
        if (!oldpwd)
        {
            OUT2E("%s: %s: OLDPWD not set\n", argv0, bltin_argv[0]);
            return 2;
        }
        cd_path = malloc(P_CS * (strlen(oldpwd) + 1));
        if (cd_path == NULL)
        {
            OUT2E("%s: malloc failed: %s\n", bltin_argv[0], strerror(errno));
            return 2;
        }
        puts(oldpwd);
        strcpy(cd_path, oldpwd);
    }
    else
    {
        cd_path = malloc(P_CS * (strlen(bltin_argv[1]) + 1));
        if (cd_path == NULL)
        {
            OUT2E("%s: malloc failed: %s\n", bltin_argv[0], strerror(errno));
            return 2;
        }
        psh_strncpy(cd_path, bltin_argv[1], strlen(bltin_argv[1]));
    }

    create_new_pwd(&cd_path);

    if (pshchdir(cd_path) != 0)
        OUT2E("%s: %s: %s\n", bltin_argv[0], strerror(errno), cd_path);
    else
    {
        pshsetenv("OLDPWD", getenv("PWD"), 1);
        pshsetenv("PWD", cd_path, 1);
    }
    free(cd_path);
    return 1;
}
