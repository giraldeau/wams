#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <QCoreApplication>
#include <QTextStream>
#include <QDataStream>
#include <QStringList>
#include <QTimer>

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

#include "unwindmonitor.h"

using namespace std;

int main(int argc, char *argv[])
{
    cout << "master " << getpid() << endl;
    QCoreApplication app(argc, argv);
    QStringList args = QCoreApplication::arguments();
    args.removeFirst();

    UnwindMonitor um;
    QObject::connect(&um, SIGNAL(done()), &app, SLOT(quit()), Qt::QueuedConnection);
    um.execute(args);

    return app.exec();
}
