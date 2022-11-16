#include <QObject>
#include <QCoreApplication>
#include <QObject>
#include <QString>
#include <QtNetwork/QtNetwork>
#include <QMutex>

#ifndef MY_COMM
#define MY_COMM
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENet:public QObject
{
    Q_OBJECT
signals:
    void signalOpen();
    void signalClose();
    void signalChanged();

//构造函数和析构函数
public:
    explicit ENet(QObject *parent = nullptr);
    explicit ENet(QString hostName,int port);
    explicit ENet(QString hostName,int port,int timeOut);
    ~ENet();

    //方法
    bool open(QString hostName,int port,int timeOut);

    bool close();

    bool connected();
    bool recvFinished();
    void start();
    void stop();

    QTcpSocket::SocketState state();

    QString readRecvBufferString();//读取结果数据

protected:
    bool send(QString);
    QTcpSocket *network;

//槽
protected slots:
        void recv();//只管返回帧


private://以下内容不需要公开
        QMutex comMut;
        QString recvBufferString;
        bool recvSuccess;
        bool stopFlag;
        void create();

private slots:
        void openSlot(QString hostName,int port,int timeOut);
        void closeSlot();
};
#endif
