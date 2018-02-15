/*
   backend.h - backend definitions of the psh

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

#include "../pshell.h"

extern int last_command_status;
extern int pipe_fd[2], in_fd, out_fd;

int prepare(void);
char *gethd(void);
char *gethdnam(char *);
char *getun(void);
char *pshgetcwd(void);
int pshgethostname(char *, size_t);
int do_run(struct parse_info *info);
int pshgetuid(void);
int pshchdir(char *);
