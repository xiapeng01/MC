#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QDebug>
#include <QThread>
#include <QObject>
#include <QEventLoop>
#include <Mitsubishi_MC_3E.h>

class MyThread : public QThread
{
    Q_OBJECT

signals:
   void upDate(QString,unsigned int,bool);

public:
    explicit MyThread(QObject *parent = nullptr);

    void stop();

protected:
    void run();

private:
    volatile bool stopped;
    Mitsubishi_MC_3E_bin *MC_3E;
    QString address;
    QString regType;
    int count;
};

class comThread:public QThread
{
    Q_OBJECT

public:
    explicit comThread(QObject *parent = nullptr);

protected:
    void run();
};

#endif // MYTHREAD_H
