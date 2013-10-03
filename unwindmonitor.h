#ifndef UNWINDMONITOR_H
#define UNWINDMONITOR_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif

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
#include <QTextStream>
#include <QStringList>
#include <QProcess>

class UnwindMonitor : public QObject
{
    Q_OBJECT
public:
    explicit UnwindMonitor(QObject *parent = 0);
    ~UnwindMonitor();
    void execute(const QStringList args);
    void execChild(const QStringList args);
    void traceProcess(pid_t pid);
    void doBacktrace(struct UPT_info *ui);

signals:
    void done();
    void backtraceReady();
public slots:

private:
    QTextStream out;
    unw_addr_space_t as;
};

#endif // UNWINDMONITOR_H
