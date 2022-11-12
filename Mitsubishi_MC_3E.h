#include "comm.h"
#include <QCoreApplication>
#include <QtNetwork/QtNetwork>
#include <QRegularExpression>
#include <QtEndian>
#include <QEventLoop>
#include <QMutex>
#include <QMetaObject>
#include <QtConcurrent>
#include <QTcpSocket>


#ifndef MT_MC_3E_BIN
#define MT_MC_3E_BIN

static QRegularExpression expBin("[^01]");
static QRegularExpression expDec("[^0-9]");
static QRegularExpression expSDec("[^\\-?\\+?0-9]");
static QRegularExpression expFDec("[^\\-?\\+?\\.?0-9]");
static QRegularExpression expHex("[^0-9a-fA-F]");
static QRegularExpression expChar("[^a-zA-Z]");


template <typename T>
struct operateResult
{
public:
    T content;
    bool isSuccess;
};

struct retData
{
    short ErrCode;
    QString value;
};


class Mitsubishi_MC_3E_bin:public comm
{
    Q_OBJECT
//信号
    signals:
        void networkChangedSignal(QAbstractSocket::SocketState);
        void networkOpen();
        void networkClose();

public:
        //QRegularExpression *exp;
//以下为方法
public:
    Mitsubishi_MC_3E_bin(QObject *parent = nullptr);
    ~Mitsubishi_MC_3E_bin();

    retData read(QString address,unsigned int length,bool wordMode);//返回值字符串
    retData write(QString address,unsigned int length,bool wordMode,QString value);//返回结果

    Q_INVOKABLE retData readInvoke(QString address,unsigned int length,bool wordMode);//返回值字符串
    Q_INVOKABLE retData writeInvoke(QString address,unsigned int length,bool wordMode,QString value);//返回结果

    void formatAll(bool mode);//格式化所有数值
    void sleep(int msec);//等待延时
    QString readSendString();//读取发送字符串
    QString readRecvString();//读取返回字符串
    QString readDataString();//读取结果值
    QString readErrorCode();//读取错误代码
    QStringList readDataList();//读取返回值列表

private://重写以下两个方法，覆盖父类的方法
    Q_INVOKABLE retData readData(QString address,unsigned int length,bool wordMode);//返回值字符串
    Q_INVOKABLE retData writeData(QString address,unsigned int length,bool wordMode,QString value);//返回结果

    QString enCodeMitsubishi_MC_3E(int,QString address,unsigned int length,bool wordMode,QString value);//构造数据帧
    retData deCodeMitsubishi_MC_3E(QString Str);//解析数据帧,将解析结果分作两个部分，第一个部分是错误代码，第二部分是返回值

    QString format(QString);//格式化接收数据
    QString splitString(QString str);//拆分接收数据
    QString filterBin(QString);//过滤2进制数值-并转大写
    QString filterDec(QString);//过滤十进制数值-并转大写
    QString filterHex(QString);//过滤十六进制数值-并转大写
    QString filterChar(QString);//过滤英文字符-并转大写

    bool registerMode(QString address,bool mode);//判断寄存器类型
    QString process(QString str,bool wordMode);//处理数据


//以下为属性
private:
    QString str,sendStr,recvStr,recvData,recvErrorCode;
    QStringList recvDataList;
    QMutex funMut;
    bool busy;
    int token,lastToken;

    //以下为槽
private slots:
        void readSlot(QString,unsigned int,bool);//返回值字符串
        void writeSlot(QString,unsigned int,bool,QString);//返回结果

public:
        operateResult<bool> readBool(QString address);
        operateResult<unsigned char> readByte(QString address);
        operateResult<short> readShort(QString address);
        operateResult<unsigned short> readUShort(QString address);
        operateResult<int> readInt(QString address);
        operateResult<unsigned int> readUInt(QString address);
        operateResult<long int> readLongInt(QString address);
        operateResult<unsigned long int> readULongInt(QString address);
        operateResult<float> readFloat(QString address);
        operateResult<double> readDouble(QString address);
        operateResult<QString> readString(QString address,unsigned int length);

/*
        bool readBool(QString address);
        unsigned char readByte(QString address);
        short readShort(QString address);
        unsigned short readUShort(QString address);
        int readInt(QString address);
        unsigned int readUInt(QString address);
        long int readLongInt(QString address);
        unsigned long int readULongInt(QString address);
        float readFloat(QString address);
        double readDouble(QString address);
        QString readString(QString address,unsigned int length);
*/

        operateResult<bool> writeBool(QString address,bool value);
        operateResult<bool> writeByte(QString address,unsigned char value);
        operateResult<bool> writeShort(QString address,short value);
        operateResult<bool> writeUShort(QString address,unsigned short value);
        operateResult<bool> writeInt(QString address,int value);
        operateResult<bool> writeUInt(QString address,unsigned int value);
        operateResult<bool> writeLongInt(QString address,long int value);
        operateResult<bool> writeULongInt(QString address,unsigned long int value);
        operateResult<bool> writeFloat(QString address,float value);
        operateResult<bool> writeDouble(QString address,double value);
        operateResult<bool> writeString(QString address,QString value);
};

#endif
