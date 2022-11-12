#include "MyThread.h"
#include <QMetaObject>
#include <QDebug>

MyThread::MyThread(QObject *parent)
    : QThread{parent}
{    
    stopped=false;
}

void MyThread::run()
{
    MC_3E=new Mitsubishi_MC_3E_bin(this);
    MC_3E->open("127.0.0.1",6000,1000);

    address="D100";
    regType="int";
    count=1;
    unsigned short value;
    QString adr2="D101";


    qDebug()<<"MyThread通信实例线程ID:"<<QThread::currentThreadId();
    this->msleep(1000);
    qreal i=0;
    bool flag;
    while(!stopped)
    {
        this->msleep(3);
        //MC_3E->writeInt(adr2,2);
        value=MC_3E->readUShort(address).content;
        if(value==1)
            MC_3E->writeUShort(adr2,2);
        else if(value==0)
            MC_3E->writeUShort(adr2,3);
        if(value==0 && MC_3E->readUShort("D102").content==1) MC_3E->writeShort("D103",56);
    }
    stopped=false;
}

void MyThread::stop()
{    
    stopped=true;
}

comThread::comThread(QObject *parent)
    : QThread{parent}
{
    ;
}

void comThread::run()
{
    qDebug()<<"comThread通信实例线程ID:"<<QThread::currentThreadId();
    this->exec();
}
