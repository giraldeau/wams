#include "unwindmonitor.h"
#include <unistd.h>

#include <QDebug>
#include <iostream>
using namespace std;

UnwindMonitor::UnwindMonitor(QObject *parent) :
    QObject(parent),
    out(stdout)
{
    as = unw_create_addr_space(&_UPT_accessors, 0);
    connect(&child, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(childFinished(int, QProcess::ExitStatus)));
    connect(&child, SIGNAL(error(QProcess::ProcessError)), this, SLOT(childError(QProcess::ProcessError)));
    connect(&child, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(doBacktrace(QProcess::ProcessState)));
    //connect(&child, SIGNAL(started()), this, SLOT(doBacktrace()));
    connect(&child, SIGNAL(readyRead()), this, SLOT(childRead()));
}

void UnwindMonitor::execute(const QStringList argsList)
{
    if (argsList.length() == 0) {
        emit done();
        return;
    }
    QString prog = argsList.first();
    QStringList args(argsList);
    args.removeFirst();
    child.start(prog, args);
    cout << "after start " << getpid() << endl;
}

void UnwindMonitor::childFinished(int exitCode, QProcess::ExitStatus status)
{
    out << "childFinished " << exitCode << " " << status << endl;
    emit done();
}

void UnwindMonitor::childError(QProcess::ProcessError error)
{
    out << "childError " << error << endl;
    emit done();
}

void UnwindMonitor::childRead()
{
    /*
     * Redirect stdout
     */
    out << child.readAll();
}

void UnwindMonitor::doBacktrace(QProcess::ProcessState state)
{
    out << "doBacktrace state=" << state << " " << getpid() << " " << child.pid() << endl;
    if (state == QProcess::NotRunning)
        return;

    int status;
    struct UPT_info *ui;

    Q_PID pid = child.pid();
    ui = (UPT_info*) _UPT_create(pid);
    waitpid(pid, &status, 0);

    /*
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
    */
}

