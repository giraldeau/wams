#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <unistd.h>
#include <QDebug>
#include <iostream>
#include <cxxabi.h>

#include <unistd.h>
#include <inttypes.h>
#include <libunwind.h>
#include <libunwind-ptrace.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "unwindmonitor.h"

using namespace std;

UnwindMonitor::UnwindMonitor(QObject *parent) :
    QObject(parent),
    out(stdout)
{
    as = unw_create_addr_space(&_UPT_accessors, 0);
}

UnwindMonitor::~UnwindMonitor()
{
    unw_destroy_addr_space(as);
}

void UnwindMonitor::execChild(const QStringList args)
{
    // setup arguments, impedence between C and C++ is obvious
    char *prog = ::strdup(args.first().toLocal8Bit().constData());
    char **argv = new char*[args.size() + 1];
    argv[args.size()] = 0;
    for(int i = 0; i < args.size(); i++) {
        argv[i] = ::strdup(args.at(i).toLocal8Bit().constData());
    }

    // setup ptrace
    ptrace(PTRACE_TRACEME, 0, 0, 0);
    kill(getpid(), SIGINT);
    int ret = execvp(prog, argv);

    // handle error
    if (ret < 0) {
        free(prog);
        for (int i = 0; i < args.size(); i++) {
            free(argv[i]);
        }
        delete argv;
        out << "execvp error" << endl;
    }
}

void UnwindMonitor::execute(const QStringList args)
{
    if (args.isEmpty()) {
        emit done();
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        execChild(args);
    } else if (pid > 0) {
        traceProcess(pid);
    }
    emit done();
    return;
}

bool UnwindMonitor::waitSyscall(pid_t pid)
{
    int status;

    ptrace(PTRACE_SYSCALL, pid, 0, 0);
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        return false;
    return true;
}

void UnwindMonitor::traceProcess(pid_t pid)
{
    int status;
    struct UPT_info *ui;

    ui = (UPT_info*) _UPT_create(pid);

    // Initialisation
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        goto out;

    while(1) {
        if (!waitSyscall(pid)) // sys_exit
            goto out;
        if (!waitSyscall(pid)) // sys_entry
            break;
        int rax = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, 0);
        cout << rax << endl;
        doBacktrace(ui);
    }
out:
    _UPT_destroy(ui);
}

QString demangle(char *orig)
{
    QString name;
    int status;

    char *demang = abi::__cxa_demangle(orig, 0, 0, &status);
    switch(status) {
    case 0:
        name.append(demang);
        free(demang);
        break;
    case -1:
        break;
    case -2:
    case -3:
        name.append(orig);
        break;
    default:
        name.append("demangle error");
        break;
    }
    return name;
}

void UnwindMonitor::doBacktrace(struct UPT_info *ui)
{
    unw_cursor_t cursor;
    unw_word_t ip, prev_ip = 0;
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
        if (ip == prev_ip)
            break;
        prev_ip = ip;
        unw_get_proc_name(&cursor, buf, sizeof(buf), &offp);
        QString funcName = demangle(buf);
        out << QString("ip = %1 %2").arg((long) ip, 0, 16).arg(funcName) << endl;
        ret = unw_step(&cursor);
    } while (ret > 0);
}

