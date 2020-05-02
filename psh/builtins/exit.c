/*
   exit.c - builtin exit

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

#include <stdlib.h>

#include "builtin.h"

extern int last_command_status;

int builtin_exit(ARGS)
{
    if (info->parameters[1] == NULL)
    {
        exit_psh(last_command_status);
    }
    else
    {
        int i = atoi(bltin_argv[1]);
        exit_psh(i);
    }
    return 2; /* Noreturn */
}
