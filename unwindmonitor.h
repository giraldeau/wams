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
#include <QTextStream>
#include <QStringList>
#include <QProcess>
#include <unwindprocess.h>

class UnwindMonitor : public QObject
{
    Q_OBJECT
public:
    explicit UnwindMonitor(QObject *parent = 0);
    void execute(const QStringList args);

signals:
    void done();
public slots:
    void childFinished(int exitCode, QProcess::ExitStatus status);
    void childError(QProcess::ProcessError error);
    void childRead();
    void doBacktrace(QProcess::ProcessState state);

private:
    QTextStream out;
    UnwindProcess child;
    unw_addr_space_t as;
};

#endif // UNWINDMONITOR_H
