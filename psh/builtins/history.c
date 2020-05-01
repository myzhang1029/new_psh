/*
   history.c - builtin history

   Copyright 2017 Zhang Maiyun.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <getopt.h>

#include "builtin.h"
#include "libpsh/xmalloc.h"
#include "libpsh/util.h"

#define USAGE()                                                                \
    OUT2E("history: usage: history [-c] [-d offset] [n] or history -awrn "     \
          "[filename] or history -ps arg [arg...]\n")
#define AFLAG 0x01
#define RFLAG 0x02
#define WFLAG 0x04
#define NFLAG 0x08
#define SFLAG 0x10
#define PFLAG 0x20
#define CFLAG 0x40
#define DFLAG 0x80

int builtin_history(ARGS)
{
#ifdef NO_HISTORY
    OUT2E("%s: libhistory not compiled!\n", bltin_argv[0]);
    return 2;
#else
    if (bltin_argv[1] != NULL)
    {
        int count, ch, flags = 0, n;
        char *filename = malloc(P_CS * MAXEACHARG);
        struct option longopts[] = {{"help", no_argument, NULL, 'h'},
                                    {NULL, 0, NULL, 0}};
        if (!filename)
        {
            OUT2E("%s: %s: malloc failed\n", argv0, bltin_argv[0]);
            return 2;
        }

        /*Get argc for getopt*/
        for (count = 0; bltin_argv[count]; count++)
            ;
        while ((ch = getopt_long(count, bltin_argv, ":a::w::r::n::p::s::cd:",
                                 longopts, NULL)) != -1)
        {
            switch (ch)
            {
                case 'a':
                    flags |= AFLAG;
                    if (optarg)
                        strncpy(filename, optarg,
                                sizeof(char) * MAXEACHARG - 1);
                    break;
                case 'r':
                    if (flags & AFLAG)
                    {
                        OUT2E("%s: %s: cannot use more "
                              "than one of -anrw\n",
                              argv0, bltin_argv[0]);
                        USAGE();
                        free(filename);
                        return 2;
                    }
                    flags |= RFLAG;
                    if (optarg)
                        strncpy(filename, optarg,
                                sizeof(char) * MAXEACHARG - 1);
                    break;
                case 'w':
                    if (flags & AFLAG || flags & RFLAG)
                    {
                        OUT2E("%s: %s: cannot use more "
                              "than one of -anrw\n",
                              argv0, bltin_argv[0]);
                        USAGE();
                        free(filename);
                        return 2;
                    }
                    if (optarg)
                        strncpy(filename, optarg,
                                sizeof(char) * MAXEACHARG - 1);
                    flags |= WFLAG;
                    break;
                case 'n':
                    if (flags & AFLAG || flags & RFLAG || flags & WFLAG)
                    {
                        OUT2E("%s: %s: cannot use more "
                              "than one of -anrw\n",
                              argv0, bltin_argv[0]);
                        USAGE();
                        free(filename);
                        return 2;
                    }
                    if (optarg)
                        strncpy(filename, optarg,
                                sizeof(char) * MAXEACHARG - 1);
                    flags |= NFLAG;
                    break;
                case 's':
                    flags |= SFLAG;
                    break;
                case 'p':
                    flags |= PFLAG;
                    break;
                case 'c':
                    flags |= CFLAG;
                    break;
                case 'd':
                    flags |= DFLAG;
                    n = atoi(bltin_argv[count]);
                    if (n < 0)
                    {
                        OUT2E("%s: %s: %d: invalid "
                              "option\n",
                              argv0, bltin_argv[0], n);
                        free(filename);
                        return 2;
                    }
                    if (!n)
                    {
                        int count2;
                        for (count2 = 0; bltin_argv[count][count2]; ++count2)
                            if (bltin_argv[count][count2] != '0' &&
                                (!isspace(bltin_argv[count][count2])))
                            {
                                OUT2E("%s: %s: "
                                      "%s: "
                                      "numeric "
                                      "argument "
                                      "required"
                                      "\n",
                                      argv0, bltin_argv[0], bltin_argv[count]);
                                free(filename);
                                return 2;
                            }
                    }
                    break;
                case '?':
                    OUT2E("%s: %s: invalid option '-%c'\n", argv0,
                          bltin_argv[0], optopt);
                    free(filename);
                    return 2;
                case ':':
                    OUT2E("%s: %s: -d: option requires an "
                          "argument\n",
                          argv0, bltin_argv[0]);
                    free(filename);
                    return 2;
            }
        }
        if (flags == 0)
        {
            free(filename);
            goto noopts;
        }
        else
        {
            /* TODO:Code here */
            free(filename);
        }
    }
    else
    noopts : {
        HIST_ENTRY **histlist;
        int i;

        histlist = history_list();
        if (histlist)
            for (i = 0; histlist[i]; i++)
                printf("    %d  %s\n", i + history_base, histlist[i]->line);
    }
        return 1;
#endif
}
