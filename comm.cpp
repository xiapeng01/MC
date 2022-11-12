#include <comm.h>

comm::comm(QObject *parent)
    :QObject{parent}
{    
    create();
}

comm::comm(QString hostName,int port)
{
    create();
    open(hostName,port,1000);
}

comm::comm(QString hostName,int port,int timeOut)
{
    create();
    open(hostName,port,timeOut);
}

comm::~comm()
{
    network->flush();
    network->close();
    network->deleteLater();
}

void comm::create()
{
    network=new QTcpSocket(this);
    QObject::connect(network,&QTcpSocket::connected,[](){qDebug()<<"Connect to PLC successfully!";});
    QObject::connect(network,&QTcpSocket::disconnected,[](){qDebug()<<"Disconnect from PLC1!";});
    QObject::connect(network,&QTcpSocket::stateChanged,[](QAbstractSocket::SocketState socketState){qDebug()<<"SocketState changed:"<<socketState;});
    QObject::connect(network,&QTcpSocket::readyRead,[&](){recv();});
    recvSuccess=false;
}

//以下为连接和断开管理--来自comm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief comm::open
/// \param hostName
/// \param port
/// \param timeOut
/// \return
///
bool comm::open()
{
    if(!netCfg.hostName.isEmpty()) return open(netCfg.hostName,netCfg.port,netCfg.timeOut);
    return false;
}

bool comm::open(QString hostName,int port,int timeOut)
{
    netCfg.hostName=hostName;
    netCfg.port=port;
    netCfg.timeOut=timeOut;
    if(network && !network->isOpen())
    {
       stopFlag=false;
       network->connectToHost(hostName,port);
       network->waitForConnected(timeOut);
       qDebug()<<"open connect success.";
       return true;
    }else
    {
        qDebug()<<"cannot open connect.";
        return false;
    }
}

bool comm::close()
{
    if(network && network->isOpen())
    {   stopFlag=true;
        QThread::msleep(100);
        network->flush();
        network->close();
        qDebug()<<"close success.";
        return true;
    }else
    {
        qDebug()<<"cannot close connect.";
        return false;
    }
}

void comm::openSlot(QString hostName,int port,int timeOut)
{
    qDebug()<<QDateTime::currentDateTime().toString()<<":comm通信openSlot实例线程ID:"<<QThread::currentThreadId();
    qDebug()<<"in open slot.";
    open(hostName,port,timeOut);
}
void comm::closeSlot()
{
    qDebug()<<QDateTime::currentDateTime().toString()<<":comm通信closeSlot实例线程ID:"<<QThread::currentThreadId();
    qDebug()<<"in close slot.";
    close();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////


//发送
bool comm::send(QString str)
{
    if(network && network->isOpen() && !stopFlag)//当停止标志置ON时，不进行发送操作
    {
        comMut.lock();
        recvSuccess=false;
        QByteArray array=QByteArray::fromHex(str.toUpper().toLatin1());
        network->write(array);
        network->waitForBytesWritten();
        network->flush();
        comMut.unlock();
        return true;
    }else
    {
        qDebug()<<"Network is close!";
        throw("Network is close!");
    }
    return false;
}

//接收
void comm::recv()
{
    QByteArray array;
    array =network->readAll();
    QString str=array.toHex().toUpper();
    recvBufferString=str;
    recvSuccess=true;
}

QString comm::readRecvBufferString()
{
    QString ret=recvBufferString;
    recvBufferString.clear();
    recvSuccess=false;
    return ret;
}

bool comm::recvFinished()
{
    return recvSuccess;
}


void comm::start()
{
    stopFlag=false;
}

void comm::stop()
{
    stopFlag=true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

