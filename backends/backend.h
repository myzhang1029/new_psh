#include "../pshell.h"

extern pid_t BPTable[MAXPIDTABLE];
extern int status;
extern pid_t ChdPid, ChdPid2;
extern int pipe_fd[2], in_fd, out_fd;

char *gethd(void);
char *getun(void);
int do_run(char *command, char **parameters, struct parse_info info);
