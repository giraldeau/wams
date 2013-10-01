#include "unwindmonitor.h"

#include <iostream>
using namespace std;

UnwindMonitor::UnwindMonitor(QObject *parent) :
    QObject(parent)
{
    as = unw_create_addr_space(&_UPT_accessors, 0);
    connect(&child, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(childExited()));
    connect(&child, SIGNAL(started()), this, SLOT(doBacktrace()));
}

void UnwindMonitor::execute(QStringList args)
{
    cout << "before start" << endl;
    child.start(args[0], args);
    cout << "after start" << endl;
}

void UnwindMonitor::childExited()
{
    emit done();
}

void UnwindMonitor::doBacktrace()
{
    int status;
    struct UPT_info *ui;

    cout << "doBacktrace" << endl;
    Q_PID pid = child.pid();
    ui = (UPT_info*) _UPT_create(pid);
    waitpid(pid, &status, 0);
    ptrace(PTRACE_SYSCALL, pid, 0, 0);
    while(1) {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            break;
        int rax = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, 0);
        cout << rax << endl;
        //do_backtrace(pid);
        ptrace(PTRACE_SYSCALL, pid, 0, 0);
    }
    _UPT_destroy(ui);
}
