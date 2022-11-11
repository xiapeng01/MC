#include <QCoreApplication>
#include <QObject>
#include <QString>
#include <QtNetwork/QtNetwork>
#include <QMutex>

#ifndef MY_COMM
#define MY_COMM

class comm:public QObject
{
    Q_OBJECT

//构造函数和析构函数
public:
    comm(QObject *parent = nullptr);
    ~comm();

    //方法
public:
        bool open(QString hostName,int port,int timeOut);
        bool close();
        bool connected();
        bool recvFinished();
        QString readRecvBufferString();//读取结果数据

protected:
        bool send(QString);

    //属性
protected:
        QTcpSocket *network;

private://以下内容不需要公开
        QMutex comMut;
        QString recvBufferString;
        bool recvSuccess;
        bool stop;

    //槽
protected slots:
        void recv();//只管返回帧

private slots:
        void openSlot(QString hostName,int port,int timeOut);
        void closeSlot();
};
#endif
