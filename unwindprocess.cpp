#include "unwindprocess.h"
#include <sys/ptrace.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include <iostream>
using namespace std;

UnwindProcess::UnwindProcess(QObject *parent) :
    QProcess(parent)
{
}

void UnwindProcess::setupChildProcess()
{
    cout << "setupChildProcess " << getpid() << endl;
    ::ptrace(PTRACE_TRACEME, 0, 0, 0);
    ::kill(getpid(), SIGINT);
    cout << "ptrace started " << getpid() << endl;
}
