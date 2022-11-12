#include <QCoreApplication>
#include <QObject>
#include <QString>
#include <QtNetwork/QtNetwork>
#include <QMutex>

#include <QtSerialPort/QtSerialPort>>

#ifndef MY_COMM
#define MY_COMM

struct netConfig
{
    QString hostName;
    unsigned int port;
    unsigned int timeOut;
};

struct serialConfig
{
    QString portName;
    unsigned int baudRate;
    unsigned int dataBits;
    QString parity;
    unsigned int stopBits;
};

class comm:public QObject
{
    Q_OBJECT

//构造函数和析构函数
public:
    comm(QObject *parent = nullptr);
    comm(QString hostName,int port);
    comm(QString hostName,int port,int timeOut);
    ~comm();

    //方法
public:
    bool open();
    bool open(QString hostName,int port,int timeOut);



    bool close();
    bool connected();
    bool recvFinished();
    void start();
    void stop();
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
        bool stopFlag;
        void create();
        netConfig netCfg;
        serialConfig serialCfg;

    //槽
protected slots:
        void recv();//只管返回帧

private slots:
        void openSlot(QString hostName,int port,int timeOut);
        void closeSlot();
};
#endif
