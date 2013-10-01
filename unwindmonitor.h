#ifndef UNWINDMONITOR_H
#define UNWINDMONITOR_H

#include <unistd.h>
#include <inttypes.h>
#include <libunwind.h>
#include <libunwind-ptrace.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <QObject>
#include <QStringList>
#include <QProcess>
#include <unwindprocess.h>

class UnwindMonitor : public QObject
{
    Q_OBJECT
public:
    explicit UnwindMonitor(QObject *parent = 0);
    void execute(QStringList args);

signals:
    void done();
public slots:
    void childExited();
    void doBacktrace();
private:
    UnwindProcess child;
    unw_addr_space_t as;
};

#endif // UNWINDMONITOR_H
