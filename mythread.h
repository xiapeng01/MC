#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QDebug>
#include <QThread>
#include <QObject>
#include <QEventLoop>
#include <protocol.h>
#include <QtSql/QSql>
#include <QtSql/QSqlQueryModel>

class MyThread : public QThread
{
    Q_OBJECT

signals:
   void upDate(QString,unsigned int,bool);

public:
    explicit MyThread(QObject *parent = nullptr);
    ~MyThread();
    void stop();

protected:
    void run();

private:
    volatile bool stopped;
    Mit::MC_3E_bin *MC_3E;
    QString address;
    QString regType;
    int count;

    QSqlDatabase *db;
    QSqlQueryModel *model;
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
