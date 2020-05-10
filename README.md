# psh
[![Build Status](https://travis-ci.com/myzhang1029/psh.svg?branch=master)](https://travis-ci.com/myzhang1029/psh)
[![Gitpod Ready-to-Code](https://img.shields.io/badge/Gitpod-Ready--to--Code-blue?logo=gitpod)](https://gitpod.io/#https://github.com/myzhang1029/psh) 

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

## Usage
Psh is equipped with CMake build system.
    mkdir build
    cd build
    cmake ..
    make

Now just run:
    psh/psh

## Future Plans
Have a look at the GitHub projects
[Roadmap](https://github.com/myzhang1029/psh/projects/2) kanban!
