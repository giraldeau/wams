#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <libunwind.h>
#include <libunwind-ptrace.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

static struct UPT_info *ui;
static unw_addr_space_t as;

void do_backtrace (int pid)
{
    unw_cursor_t cursor;
    unw_word_t ip;
    unw_word_t offp;
    int ret;
    char buf[512];

    ret = unw_init_remote(&cursor, as, ui);
    if (ret < 0) {
        cerr << "unw_init_remote() failed" << endl;
        return;
    }

    do {
        ret = unw_get_reg(&cursor, UNW_REG_IP, &ip);
        if (ret < 0) {
            cerr << "unw_init_remote() failed" << endl;
            return;
        }

        buf[0] = '\0';
        unw_get_proc_name(&cursor, buf, sizeof(buf), &offp);
        printf("ip = %lx %s\n", (long) ip, buf);
        ret = unw_step(&cursor);
    } while (ret > 0);

}

/*
 * Some dummy code
 */

volatile int x;
void d() { x++; sleep(1); printf("test\n"); }
void c() { x++; d(); }
void b() { x++; c(); }
void a() { x++; b(); }

void spawn_child()
{
    ptrace(PTRACE_TRACEME, 0, 0, 0);
    /*
     * Send signal to start ptrace
     */
    kill(getpid(), SIGINT);
    a();
}

void trace_process(int pid)
{
    int status;

    ui = (UPT_info*) _UPT_create(pid);

    waitpid(pid, &status, 0);
    ptrace(PTRACE_SYSCALL, pid, 0, 0);

    while(1) {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            break;
        int rax = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, 0);
        cout << rax << endl;
        do_backtrace(pid);
        ptrace(PTRACE_SYSCALL, pid, 0, 0);
    }

    _UPT_destroy(ui);
}

int main(int argc, char *argv[])
{
    int pid;

    as = unw_create_addr_space(&_UPT_accessors, 0);
    if (!as) {
        cerr << "unw_create_addr_space() failed" << endl;
        return -1;
    }

    pid = fork();
    if (pid == 0) {
        spawn_child();
    } else if (pid > 0) {
        trace_process(pid);
    } else {
        perror("fork error");
    }

    return 0;
}
