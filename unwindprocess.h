#ifndef UNWINDPROCESS_H
#define UNWINDPROCESS_H

#include <QProcess>

class UnwindProcess : public QProcess
{
    Q_OBJECT
public:
    explicit UnwindProcess(QObject *parent = 0);
    
signals:
    
public slots:
protected:
    void setupChildProcess();
};

#endif // UNWINDPROCESS_H
