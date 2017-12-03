#include "../pshell.h"

extern pid_t BPTable[MAXPIDTABLE];
extern int status;
extern pid_t ChdPid, ChdPid2;
extern int pipe_fd[2], in_fd, out_fd;
