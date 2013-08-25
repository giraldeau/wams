wams: Where Are My Syscalls
===========================

Wams locates where system calls are made in the code. It uses ptrace and
libunwind, allowing to do complete and precise backtrace of optimized, stripped
and without frame pointers. 

