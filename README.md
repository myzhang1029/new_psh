# psh
[![Build Status](https://travis-ci.com/myzhang1029/psh.svg)](https://travis-ci.com/myzhang1029/psh)
[![Gitpod Ready-to-Code](https://img.shields.io/badge/Gitpod-Ready--to--Code-blue?logo=gitpod)](https://gitpod.io/#https://github.com/myzhang1029/psh) 

## Description
Psh is a shell implementation. It tries to mimic most of bash's
and dash's behaviors.  

Psh started out as a vvy/wshell fork, but now all original contents
have been removed and re-implemented without referencing the original code,
and the shell is being worked on completely by me and other nice contributors.  

Psh aims to be able to compile on as many C99 systems as possible,
providing a highly emulated POSIX environment.
To reach this goal, psh juices out those platform-dependent parts, for
example, subprocess management and user data extraction, into
_backends_.  

Psh is now my UWC FP _free summer_ project.

## Usage
Psh is equipped with CMake build system as well as GNU autotools support.
```
mkdir build
cd build
cmake ..
make
```

or
```
autoreconf -fi
./configure
make
```

Now just run:
```
src/psh
```

## Future Plans
Have a look at the GitHub projects
[Roadmap](https://github.com/myzhang1029/psh/projects/2) kanban!


## Contributing
Issues and pull requests are always welcomed! However, please do take a note on
[licensing](https://www.gnu.org/licenses/#GPL) and this readme. For the code
style, following the existing code should work, it will be even better if the
style defined with the `.clang-format` file is used.