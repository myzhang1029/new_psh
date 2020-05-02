/*
   builtins.c - builtins caller file of the psh

   Copyright 2013 wuyve.
   Copyright 2017 Zhang Maiyun.
*/

#include <stdlib.h>

#include "builtins/builtin.h"
#include "pshell.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"

#define cmdis(cmd) (strcmp(info->parameters[0], cmd) == 0)

extern int last_command_status;

int get_argc(char **argv)
{
    int argc = 0;
    for (; argv[argc]; ++argc)
        ;
    return argc;
}

int builtin_unsupported(ARGS)
{
    OUT2E("%s: %s: Not supported, coming soon\n", argv0, bltin_argv[0]);
    return 2;
}

int compare_builtin(const void *key, const void *cur)
{
    const struct builtin *bkey = (const struct builtin *)key;
    const struct builtin *builtin = (const struct builtin *)cur;
    return strcmp(bkey->name, builtin->name);
}

builtin_function find_builtin(char *name)
{
    struct builtin *key = xmalloc(sizeof(struct builtin));
    struct builtin *result;
    key->name = name;
    result = (struct builtin *)bsearch(
        key, builtins, 61, sizeof(struct builtin), &compare_builtin);
    xfree(key);
    return result != NULL ? result->proc : (builtin_function)0;
}

/* List of all builtins, sorted by name */
const struct builtin builtins[] = {{".", &builtin_unsupported},
                                   {":", &builtin_true},
                                   {"alias", &builtin_unsupported},
                                   {"bg", &builtin_unsupported},
                                   {"bind", &builtin_unsupported},
                                   {"break", &builtin_unsupported},
                                   {"builtin", &builtin_builtin},
                                   {"case", &builtin_unsupported},
                                   {"cd", &builtin_cd},
                                   {"chdir", &builtin_cd},
                                   {"command", &builtin_unsupported},
                                   {"continue", &builtin_unsupported},
                                   {"declare", &builtin_unsupported},
                                   {"do", &builtin_unsupported},
                                   {"done", &builtin_unsupported},
                                   {"echo", &builtin_echo},
                                   {"elif", &builtin_unsupported},
                                   {"else", &builtin_unsupported},
                                   {"esac", &builtin_unsupported},
                                   {"eval", &builtin_unsupported},
                                   {"exec", &builtin_exec},
                                   {"exit", &builtin_exit},
                                   {"export", &builtin_unsupported},
                                   {"false", &builtin_false},
                                   {"fc", &builtin_unsupported},
                                   {"fg", &builtin_unsupported},
                                   {"fi", &builtin_unsupported},
                                   {"for", &builtin_unsupported},
                                   {"getopts", &builtin_unsupported},
                                   {"hash", &builtin_unsupported},
                                   {"history", &builtin_history},
                                   {"if", &builtin_unsupported},
                                   {"jobid", &builtin_unsupported},
                                   {"jobs", &builtin_unsupported},
                                   {"local", &builtin_unsupported},
                                   {"logout", &builtin_exit},
                                   {"popd", &builtin_unsupported},
                                   {"pushd", &builtin_unsupported},
                                   {"pwd", &builtin_pwd},
                                   {"quit", &builtin_exit},
                                   {"read", &builtin_unsupported},
                                   {"readonly", &builtin_unsupported},
                                   {"return", &builtin_unsupported},
                                   {"set", &builtin_unsupported},
                                   {"setvar", &builtin_unsupported},
                                   {"shift", &builtin_unsupported},
                                   {"source", &builtin_unsupported},
                                   {"test", &builtin_unsupported},
                                   {"then", &builtin_unsupported},
                                   {"times", &builtin_unsupported},
                                   {"trap", &builtin_unsupported},
                                   {"true", &builtin_true},
                                   {"type", &builtin_unsupported},
                                   {"ulimit", &builtin_unsupported},
                                   {"umask", &builtin_unsupported},
                                   {"unalias", &builtin_unsupported},
                                   {"unset", &builtin_unsupported},
                                   {"until", &builtin_unsupported},
                                   {"wait", &builtin_unsupported},
                                   {"which", &builtin_unsupported},
                                   {"while", &builtin_unsupported}};

int run_builtin(struct command *info)
{
    if (cmdis("getstat"))
        return printf("%d\n", last_command_status), 1;
    else if (cmdis("about"))
    {
        printf("psh is a not fully implemented shell in UNIX.\n");
        return 1;
    }
    else
    {
        builtin_function proc = find_builtin(info->parameters[0]);
        if (proc == 0)
            return 0;
        return ((*proc)(info));
    }
}
