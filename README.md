FORK OF https://github.com/myzhang1029/psh

IT COMPILES AND RUNS BUT STILL SOME WARNINGS ABOUT UNUSED PARAMETERS SHOW UP


## Description
Psh is a shell implementation. It tries to mimic most of bash's
and dash's behaviours.  

Psh started out as a vvy/wshell fork, but now all original contents
are to-be-removed, and I expect to implement the shell completely
by myself.  

Psh aims to be able to compile on as many C99 systems as possible,
providing a highly emulated POSIX environment.
To reach this goal, psh juices out those platform-dependent parts, for
example, subprocess management and user data extraction, into
_backends_.  

Psh is now my UWC FP _free summer_ project.

