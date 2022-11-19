#include <ENet.h>

ENet::ENet(QObject *parent)
    :QObject{parent}
{    
    create();
}

ENet::ENet(QString hostName,int port)
{
    create();
    open(hostName,port,1000);
}

ENet::ENet(QString hostName,int port,int timeOut)
{
    create();
    open(hostName,port,timeOut);
}

ENet::~ENet()
{
    if(network->isOpen())
    {
        network->flush();
        network->close();
        network->deleteLater();
    }
}

void ENet::create()
{
    network=new QTcpSocket();

    connect(network,&QTcpSocket::connected,this,&ENet::signalOpen);
    connect(network,&QTcpSocket::disconnected,this,&ENet::signalClose);

    QObject::connect(network,&QTcpSocket::readyRead,[&](){recv();});//接收函数
    recvSuccess=false;
}

//以下为连接和断开管理--来自comm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ENet::open(QString hostName,int port,int timeOut)
{
    if(network && network->state()!=QTcpSocket::ConnectedState)
    {
       if(network->isOpen()) network->close();
       stopFlag=false;
       network->connectToHost(hostName,port);
       network->waitForConnected(timeOut);
       qDebug()<<"Open:operation success.";
       return true;
    }else
    {
        qDebug()<<"Open:network is not exist.";
        return false;
    }
}
bool ENet::close()
{
    if(network && network->isOpen())
    {   stopFlag=true;
        network->flush();
        network->close();
        qDebug()<<"Close:operation success.";
        return true;
    }else
    {
        qDebug()<<"Close:network is not exist.";
        return false;
    }
}

void ENet::openSlot(QString hostName,int port,int timeOut)
{
    qDebug()<<QDateTime::currentDateTime().toString()<<":comm通信openSlot实例线程ID:"<<QThread::currentThreadId();
    qDebug()<<"in open slot.";
    open(hostName,port,timeOut);
}

void ENet::closeSlot()
{
    qDebug()<<QDateTime::currentDateTime().toString()<<":comm通信closeSlot实例线程ID:"<<QThread::currentThreadId();
    qDebug()<<"in close slot.";
    close();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

//发送
bool ENet::send(QString str)
{
    if(network && network->isOpen() && !stopFlag &&network->state()==QTcpSocket::ConnectedState)//当停止标志置ON时，不进行发送操作
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
        //throw("Network is close!");
    }
    return false;
}

//接收
void ENet::recv()
{
    QByteArray array;
    array =network->readAll();
    QString str=array.toHex().toUpper();
    recvBufferString=str;
    recvSuccess=true;
}

QString ENet::readRecvBufferString()
{
    QString ret=recvBufferString;
    recvBufferString.clear();
    recvSuccess=false;
    return ret;
}

bool ENet::recvFinished()
{
    return recvSuccess;
}


void ENet::start()
{
    stopFlag=false;
}

void ENet::stop()
{
    stopFlag=true;
}

QTcpSocket::SocketState ENet::state()
{
    return network->state();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

