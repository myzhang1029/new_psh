/*
   show.c - Prompt generator

   Copyright 2013 wuyve.
   Copyright 2017 Zhang Maiyun.
*/

#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "pshell.h"

const int max_name_len = 256;
const int max_path_len = 1024;

void type_prompt(char *prompt)
{
    char hostname[max_name_len];
    char *pathname = pshgetcwd_dm();
    int length;
    char *hdir = gethd(), *username = getun();

    if (pshgethostname(hostname, max_name_len) == 0)
        sprintf(prompt, "%s@%s:", username, strtok(hostname, "."));
    else
        sprintf(prompt, "%s@unknown:", username);
#ifdef DEBUG
    printf("pathname: %s,length:%d\npw_dir:%s,length:%d\n", pathname,
           strlen(pathname), pwd->pw_dir, strlen(pwd->pw_dir));
#endif
    length = strlen(prompt);
    if (strlen(pathname) < strlen(hdir) ||
        strncmp(pathname, hdir, strlen(hdir)) != 0)
        sprintf(prompt + length, "%s", pathname);
    else
        sprintf(prompt + length, "~%s", pathname + strlen(hdir));
    length = strlen(prompt);
    if (pshgetuid() == 0)
        sprintf(prompt + length, "# ");
    else
        sprintf(prompt + length, "$ ");
    free(pathname);
    return;
}
