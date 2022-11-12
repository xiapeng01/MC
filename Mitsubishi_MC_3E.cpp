#include "./Mitsubishi_MC_3E.h"


Mitsubishi_MC_3E_bin::Mitsubishi_MC_3E_bin(QObject *parent)
    :comm{parent}
{
    QObject::connect(network,&QTcpSocket::connected,this,&Mitsubishi_MC_3E_bin::networkOpen);
    QObject::connect(network,&QTcpSocket::disconnected,this,&Mitsubishi_MC_3E_bin::networkClose);    
    QObject::connect(network,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SIGNAL(networkChangedSignal(QAbstractSocket::SocketState)));

    qDebug()<<"MC实例线程ID:"<<QThread::currentThreadId();
    token=0;
    lastToken=0;
}

Mitsubishi_MC_3E_bin::~Mitsubishi_MC_3E_bin()
{
    network->flush();
    network->close();
    network->deleteLater();
    network=0;;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//构造MC-3E帧
QString Mitsubishi_MC_3E_bin::enCodeMitsubishi_MC_3E(int method,QString address,unsigned int length,bool wordMode,QString value)
{
    //method=0 读
    //method=1 写

    QString subCommand,soft;//字指令，寄存器类型
    QString registerName=filterChar(address);

    //判断传过来的参数寄存器类型
    //subCommand="01 00"表示按位方式读写，subCommand="00 00"表示按字方式读写。
    //soft表示寄存器类型
    if(registerName.toUpper()=="X" && wordMode) //以下为按字方式读取的寄存器
    {
        subCommand="00 00";//按字方式16位
        soft="9C";//寄存器类型
    }else if(registerName.toUpper()=="Y" && wordMode) //////////
    {
        subCommand="00 00";//按字方式16位
        soft="9D";//寄存器类型
    }else if(registerName.toUpper()=="B" && wordMode) //////////
    {
        subCommand="00 00";//按字方式16位
        soft="A0";//寄存器类型
    }else if(registerName.toUpper()=="M" && wordMode)//////////
    {
        subCommand="00 00";//按字方式16位
        soft="90";//寄存器类型
    }else if(registerName.toUpper()=="L" && wordMode) //////////
    {
        subCommand="00 00";//按字方式16位
        soft="92";//寄存器类型
    }else if(registerName.toUpper()=="D")
    {
        subCommand="00 00";//按字方式16位
        soft="A8";//寄存器类型
    }else if(registerName.toUpper()=="R")
    {
        subCommand="00 00";//按字方式16位
        soft="AF";//寄存器类型
    }else if(registerName.toUpper()=="ZR")
    {
        subCommand="00 00";//按字方式16位
        soft="B0";//寄存器类型
    }else if(registerName.toUpper()=="W")
    {
        subCommand="00 00";//按字方式16位
        soft="B4";//寄存器类型
    }else if(registerName.toUpper()=="TN")
    {
        subCommand="00 00";//按字方式16位
        soft="C2";//寄存器类型
    }else if(registerName.toUpper()=="TS" && wordMode)//////////
    {
        subCommand="00 00";//按字方式16位
        soft="C1";//寄存器类型
    }else if(registerName.toUpper()=="CN")
    {
        subCommand="00 00";//按字方式16位
        soft="C5";//寄存器类型
    }else if(registerName.toUpper()=="CS" && wordMode)//////////
    {
        subCommand="00 00";//按字方式16位
        soft="C4";//寄存器类型
    }else if(registerName.toUpper()=="SM" && wordMode)//////////
    {
        subCommand="00 00";//按字方式16位
        soft="91";//寄存器类型
    }else if(registerName.toUpper()=="SD")
    {
        subCommand="00 00";//按字方式16位
        soft="A9";//寄存器类型
    }else if(registerName.toUpper()=="X") //以下为按位方式读取的寄存器
    {
        subCommand="01 00";//按位方式
        soft="9C";//寄存器类型
    }else if(registerName.toUpper()=="Y") //////////
    {
        subCommand="01 00";//按位方式
        soft="9D";//寄存器类型
    }else if(registerName.toUpper()=="B") //////////
    {
        subCommand="01 00";//按位方式
        soft="A0";//寄存器类型
    }else if(registerName.toUpper()=="M")//////////
    {
        subCommand="01 00";//按位方式
        soft="90";//寄存器类型
    }else if(registerName.toUpper()=="L") //////////
    {
        subCommand="01 00";//按位方式
        soft="92";//寄存器类型
    }else if(registerName.toUpper()=="TS")//////////
    {
        subCommand="01 00";//按位方式
        soft="C1";//寄存器类型
    }else if(registerName.toUpper()=="CS")//////////
    {
        subCommand="01 00";//按位方式
        soft="C4";//寄存器类型
    }else if(registerName.toUpper()=="SM")//////////
    {
        subCommand="01 00";//按位方式
        soft="91";//寄存器类型
    }
    //bool dFlag=address.left(1).toUpper()=="D";
    QString head="50 00 00 FF FF 03 00";//固定头
    QString len;
    QString timeout="10 00";
    QString command=(method==0)?"01 04":"01 14";//0=批量读取01 04;1=批量写入01 14
    //QString subCommand=dFlag?"00 00":"01 00";//true:字;false:位
    QString addr=QString::number(qToBigEndian(filterDec(address).toInt()),16).rightJustified(sizeof(int)/sizeof(char)*2,'0').left(6);//convert10216(filterDec(address).toInt(),6);
    //QString soft=dFlag?"A8":"90";//A8:D寄存器,90:M寄存器
    QString dataCount=QString::number(qToBigEndian((unsigned short)length),16).rightJustified(sizeof(unsigned short)/sizeof(char)*2,'0');

    //读取时，强制给0，按位模式时，若值位数为偶数则直接给值，否则追加0再给出，若为字模式，则直接给值
    QString str=((method==0)?"":(subCommand=="01 00")?((value.length()%2==0)||(value.length()<length)?value:value.append("0")):value);//按位方式或字方式时有不同

    QString data=timeout+" "+command+" "+subCommand+" "+addr+" "+soft+" "+dataCount+" "+str;
    QString temp=data;

    len=QString::number(qToBigEndian((unsigned short)(filterHex(temp).length()/2)),16).rightJustified(4,'0');

    QString ret=head+" "+len+" "+data;
    return filterHex(ret);//再次替换不需要的内容
}

//解析MC-3E帧
QString Mitsubishi_MC_3E_bin::deCodeMitsubishi_MC_3E(QString str)//只负责把返回帧的有效值分解出来
{
    if(str.left(12)=="D00000FFFF03" && str.length()>18)
    {
        recvErrorCode=str.mid(20,2)+str.mid(18,2);
        QString ret=str.mid(22,str.length());
        return ret;
    }else
    {
        qDebug()<<"Split String Error.";
        return NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
Q_INVOKABLE QString Mitsubishi_MC_3E_bin::readData(QString address,unsigned int length,bool wordMode)
{
    qDebug()<<QTime::currentTime().toString("hh:mm:ss.zzz")<<":MC_readData实例线程ID:"<<QThread::currentThreadId();
    int delay=0;
    QString ret=NULL;
    if(network->isOpen())
    {
        //while(busy) QCoreApplication::processEvents();
        funMut.lock();
        sendStr.clear();
        recvStr.clear();
        recvData.clear();
        recvDataList.clear();
        recvErrorCode.clear();
        sendStr=enCodeMitsubishi_MC_3E(0,address,length,wordMode,"");
        send(sendStr);
        while(!recvFinished())
        {
            sleep(1);
            delay++;
            //qDebug()<<delay;            
            if(delay>1000) break;
        }
        recvStr=readRecvBufferString();
        recvData=deCodeMitsubishi_MC_3E(recvStr);
        ret=recvData;
        funMut.unlock();
    }
    return ret;
}

Q_INVOKABLE QString Mitsubishi_MC_3E_bin::writeData(QString address,unsigned int length,bool wordMode,QString value)
{
    qDebug()<<QTime::currentTime().toString("hh:mm:ss.zzz")<<":MC_writeData实例线程ID:"<<QThread::currentThreadId();
    QString ret=NULL;
    if(network->isOpen())
    {
        //while(busy) QCoreApplication::processEvents();
        funMut.lock();
        int delay=0;
        sendStr.clear();
        recvStr.clear();
        recvData.clear();
        recvDataList.clear();
        recvErrorCode.clear();
        sendStr=enCodeMitsubishi_MC_3E(1,address,length,wordMode,value);
        send(sendStr);
        while(!recvFinished())
        {
            sleep(1);
            delay++;
            //qDebug()<<delay;            
            if(delay>1000) break;
        }
        recvStr=readRecvBufferString();
        recvData=deCodeMitsubishi_MC_3E(recvStr);
        ret=recvData;
        funMut.unlock();
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QString Mitsubishi_MC_3E_bin::read(QString address,unsigned int length,bool wordMode)
{
    QString ret=NULL;
    if(network->isOpen())
    {
        //while(busy)
        //{
            //qDebug()<<"read wait for comunication.";
            //QCoreApplication::processEvents();
        //}
        ret=process(readData(address,length,wordMode),registerMode(address,wordMode));
    }
    return ret;
}
QString Mitsubishi_MC_3E_bin::write(QString address,unsigned int length,bool wordMode,QString value)
{
    QString ret=NULL;
    if(network->isOpen())
    {
        //while(busy)
        //{
            //qDebug()<<"write wait for comunication.";
            //QCoreApplication::processEvents();
        //}
        ret=process(writeData(address,length,wordMode,splitString(value)),registerMode(address,wordMode));
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Q_INVOKABLE QString Mitsubishi_MC_3E_bin::readInvoke(QString address,unsigned int length,bool wordMode)
{
    QString ret=NULL;
    QMetaObject::invokeMethod(this,"read",
                              Q_RETURN_ARG(QString,ret),
                              Q_ARG(QString,address),
                              Q_ARG(unsigned int,length),
                              Q_ARG(bool,wordMode)
                              );
    return process(ret,registerMode(address,wordMode));
}

Q_INVOKABLE QString Mitsubishi_MC_3E_bin::writeInvoke(QString address,unsigned int length,bool wordMode,QString value)
{
    QString ret=NULL;
    QMetaObject::invokeMethod(this,"write",
                              Q_RETURN_ARG(QString,ret),
                              Q_ARG(QString,address),
                              Q_ARG(unsigned int,length),
                              Q_ARG(bool,wordMode),
                              Q_ARG(QString,value)
                              );
    return process(ret,registerMode(address,wordMode));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mitsubishi_MC_3E_bin::readSlot(QString address,unsigned int length,bool wordMode)
{
    qDebug()<<"in readDataSlot.";
    readData(address,length,wordMode);
    //QtConcurrent::run(readData,address,length,wordMode);

    if(token!=lastToken) QThread::msleep(100);
    lastToken=token;
    token=0;
}
void Mitsubishi_MC_3E_bin::writeSlot(QString address,unsigned int length,bool wordMode,QString value)
{
    qDebug()<<"in writeDataSlot.";
    writeData(address,length,wordMode,value);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mitsubishi_MC_3E_bin::sleep(int msec)
{
     QTime dieTime = QTime::currentTime().addMSecs(msec);
     while( QTime::currentTime() < dieTime )
     QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//将十六进制解析并转换为十进制
QString Mitsubishi_MC_3E_bin::process(QString str,bool wordMode)
{
    QString ret,s,temp;
    int i=0;
    recvDataList.clear();
    while(i<str.length() && wordMode)//符合条件则做拆分和大小端转换
    {
        s=QString::number(qFromBigEndian(str.mid(i,4).toUShort(nullptr,16)));
        ret += s+" ";
        recvDataList.append(s);
        i+=4;
    };
    ret+=str.mid(i,str.length());//保留剩余数据
    return ret;
}

//将十六进制解析并转换为小端十进制
QString Mitsubishi_MC_3E_bin::splitString(QString str)
{
    QStringList list=str.simplified().split(" ");
    QString ret;
    foreach(QString s,list)
    {
        ret+=QString::number(qFromBigEndian(s.toUShort()),16).rightJustified(4,'0')+" ";
    }
    return ret;
}

bool Mitsubishi_MC_3E_bin::registerMode(QString address,bool mode)
{
    ;
    QString regName=address.toUpper().replace(expChar,"");
    bool boolMode=false,wordMode=true;
    if(regName=="X" && mode) //以下为按字方式读取的寄存器
        return wordMode;
    else if(regName=="Y" && mode) //////////
        return wordMode;
    else if(regName=="B" && mode) //////////
        return wordMode;
    else if(regName=="M" && mode) //////////
        return wordMode;
    else if(regName=="L" && mode) //////////
        return wordMode;
    else if(regName=="D")
        return wordMode;
    else if(regName=="R")
        return wordMode;
    else if(regName=="ZR")
        return wordMode;
    else if(regName=="W")
        return wordMode;
    else if(regName=="TN")
        return wordMode;
    else if(regName=="TS" && mode)//////////
        return wordMode;
    else if(regName=="CN")
        return wordMode;
    else if(regName=="CS" && mode)//////////
        return wordMode;
    else if(regName=="SM" && mode)//////////
        return wordMode;
    else if(regName=="SD")
        return wordMode;
    else if(regName=="X") //以下为按位方式读取的寄存器
        return boolMode;
    else if(regName=="Y") //////////
        return boolMode;
    else if(regName=="B") //////////
        return boolMode;
    else if(regName=="M")//////////
        return boolMode;
    else if(regName=="L") //////////
        return boolMode;
    else if(regName=="TS")//////////
        return boolMode;
    else if(regName=="CS")//////////
        return boolMode;
    else if(regName=="SM")//////////
        return boolMode;
    else return wordMode;
}

QString Mitsubishi_MC_3E_bin::readSendString()
{
    return sendStr;
}

QString Mitsubishi_MC_3E_bin::readRecvString()
{
    return recvStr;
}

QString Mitsubishi_MC_3E_bin::readDataString()
{
    return recvData;
}

QStringList Mitsubishi_MC_3E_bin::readDataList()
{
    return recvDataList;
}

QString Mitsubishi_MC_3E_bin::readErrorCode()
{
    return recvErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 以下为工具

QString Mitsubishi_MC_3E_bin::filterBin(QString str)
{
    return str.replace(expBin,"").toUpper();
}

QString Mitsubishi_MC_3E_bin::filterDec(QString str)
{
    return str.replace(expDec,"").toUpper();
}

QString Mitsubishi_MC_3E_bin::filterHex(QString str)
{
    return str.replace(expHex,"").toUpper();
}

QString Mitsubishi_MC_3E_bin::filterChar(QString str)
{
    return str.replace(expChar,"").toUpper();
}
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//以下为按元类型读写数据的实现
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////位-bool
operateResult<bool> Mitsubishi_MC_3E_bin::readBool(QString address)
{
    operateResult<bool> ret;
    QString temp=readData(address,1,false);
    ret.content=temp.left(1)=="1"?1:0;
    return ret;
}

bool Mitsubishi_MC_3E_bin::writeBool(QString address,bool value)
{
    QString ret=NULL;
    QString v=QString::number(value);
    if(v.length()%2!=0) v.append("0");
    ret=writeData(address,1,false,v);
    return (ret=="0000");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////字节-byte
operateResult<unsigned char> Mitsubishi_MC_3E_bin::readByte(QString address)
{
    operateResult<unsigned char> ret;
    QString temp=readData(address,1,false);
    ret.content=temp.left(sizeof(ret)/sizeof(char)*2).toUInt(nullptr,16);
    return ret;
}

bool Mitsubishi_MC_3E_bin::writeByte(QString address,unsigned char value)
{
    QString ret=NULL;
    ret=writeData(address,1,false,QString::number(value,16));
    return (ret=="0000");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////短整型-short
operateResult<short> Mitsubishi_MC_3E_bin::readShort(QString address)
{
    operateResult<short> ret;
    unsigned length=1;
    QString temp=readData(address,length,false);
    ret.content=qFromBigEndian(temp.left(sizeof(ret)/sizeof(char)*2).toUInt(nullptr,16));
    return ret;
}

bool Mitsubishi_MC_3E_bin::writeShort(QString address,short value)
{
    QString ret=NULL;
    unsigned int length=1;
    ret=writeData(address,length,false,QString::number(qToBigEndian(value),16).rightJustified(sizeof(value)/sizeof(char)*2,'0'));
    return (ret=="0000");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////无符号短整型-ushort
operateResult<unsigned short> Mitsubishi_MC_3E_bin::readUShort(QString address)
{
    operateResult<unsigned short> ret;
    unsigned length=1;
    QString temp=readData(address,length,false);
    ret.content=qFromBigEndian(temp.left(sizeof(ret)/sizeof(char)*2).toUInt(nullptr,16));
    return (ret);
}

bool Mitsubishi_MC_3E_bin::writeUShort(QString address,unsigned short value)
{
    QString ret=NULL;
    unsigned int length=1;
    ret=writeData(address,length,false,QString::number(qToBigEndian(value),16).rightJustified(sizeof(value)/sizeof(char)*2,'0'));
    return (ret=="0000");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////整型-int
operateResult<int> Mitsubishi_MC_3E_bin::readInt(QString address)
{
   operateResult<int> ret;
    unsigned length=sizeof(ret)/sizeof(char)/2;
    QString temp=readData(address,length,false);
    ret.content=qFromBigEndian(temp.left(sizeof(ret)/sizeof(char)*2).toUInt(nullptr,16));//toint不能直接处理负数
    return (ret);
}

bool Mitsubishi_MC_3E_bin::writeInt(QString address,int value)
{
    QString ret=NULL;
    unsigned int length=sizeof(value)/sizeof(char)/2;
    ret=writeData(address,length,false,QString::number(qToBigEndian((unsigned int)value),16).rightJustified(sizeof(value)/sizeof(char)*2,'0'));
    return (ret=="0000");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////无符号整型-uint
operateResult<unsigned int> Mitsubishi_MC_3E_bin::readUInt(QString address)
{
    operateResult<unsigned int> ret;
    unsigned length=sizeof(ret)/sizeof(char)/2;
    QString temp=readData(address,length,false);
    ret.content=qFromBigEndian(temp.left(sizeof(ret)/sizeof(char)*2).toUInt(nullptr,16));
    return (ret);
}

bool Mitsubishi_MC_3E_bin::writeUInt(QString address,unsigned int value)
{
    QString ret=NULL;
    unsigned int length=sizeof(value)/sizeof(char)/2;
    ret=writeData(address,length,false,QString::number(qToBigEndian(value),16).rightJustified(sizeof(value)/sizeof(char)*2,'0'));
    return (ret=="0000");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////长整型-long-64bit
operateResult<long int> Mitsubishi_MC_3E_bin::readLongInt(QString address)
{
    operateResult<long int> ret;
    unsigned int length=sizeof(ret)/sizeof(char)/2;
    QString temp=readData(address,length,false);
    ret.content=qFromBigEndian(temp.left(sizeof(ret)/sizeof(char)*2).toULong(nullptr,16));
    return (ret);
}

bool Mitsubishi_MC_3E_bin::writeLongInt(QString address,long int value)
{
    QString ret=NULL;
    long int length=sizeof(value)/sizeof(char)/2;
    ret=writeData(address,length,false,QString::number(qToBigEndian(value),16).rightJustified(sizeof(value)/sizeof(char)*2,'0'));
    return (ret=="0000");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////无符号长整型-ulong-64bit
operateResult<unsigned long int> Mitsubishi_MC_3E_bin::readULongInt(QString address)
{
    operateResult<unsigned long int> ret;
    unsigned int length=sizeof(ret)/sizeof(char)/2;
    QString temp=readData(address,length,false);
    ret.content=qFromBigEndian(temp.left(sizeof(ret)/sizeof(char)*2).toULong(nullptr,16));
    return (ret);
}

bool Mitsubishi_MC_3E_bin::writeULongInt(QString address,unsigned long int value)
{
    QString ret=NULL;
    unsigned int length=sizeof(value)/sizeof(char)/2;
    ret=writeData(address,length,false,QString::number(qToBigEndian(value),16).rightJustified(sizeof(value)/sizeof(char)*2,'0'));
    return (ret=="0000");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////单精度浮点型-float-32bit--转成无符号32位整型
operateResult<float> Mitsubishi_MC_3E_bin::readFloat(QString address)
{
   operateResult<float> ret;
    float r;
    unsigned int length=sizeof(r)/sizeof(char)/2;
    QString temp=readData(address,length,false);
    unsigned int i=qFromBigEndian(temp.left(sizeof(r)/sizeof(char)*2).toUInt(nullptr,16));
    std::memcpy(&r,&i,sizeof(r)/sizeof(char));
    ret.content=r;
    return ret;
}

bool Mitsubishi_MC_3E_bin::writeFloat(QString address,float value)
{
    QString ret=NULL;
    unsigned int length=sizeof(value)/sizeof(char)/2;
    unsigned int i;
    std::memcpy(&i,&value,sizeof(value)/sizeof(char));
    ret=writeData(address,length,false,QString::number(qToBigEndian(i),16).rightJustified(sizeof(value)/sizeof(char)*2,'0'));
    return (ret=="0000");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////双精度浮点型-double float-32bit--转成无符号64位整型
operateResult<double> Mitsubishi_MC_3E_bin::readDouble(QString address)
{
    operateResult<double> ret;
    double r;
    unsigned int length=sizeof(r)/sizeof(char)/2;
    QString temp=readData(address,length,false);
    unsigned long long int l=qFromBigEndian(temp.left(sizeof(r)/sizeof(char)*2).toULongLong(nullptr,16));

    std::memcpy(&r,&l,sizeof(r)/sizeof(char));
    ret.content=r;
    return ret;
}

bool Mitsubishi_MC_3E_bin::writeDouble(QString address,double value)
{
    QString ret=NULL;
    unsigned int length=sizeof(value)/sizeof(char)/2;
    unsigned long long int l;
    std::memcpy(&l,&value,sizeof(value)/sizeof(char));
    ret=writeData(address,length,false,QString::number(qToBigEndian(l),16).rightJustified(sizeof(value)/sizeof(char)*2,'0'));
    return (ret=="0000");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////ASCII字符串
operateResult<QString> Mitsubishi_MC_3E_bin::readString(QString address,unsigned int count)
{
    operateResult<QString> ret;
    unsigned int length=count%2==0?count/2:count/2+1;
    QString temp=readData(address,length,false);//十六进制字面值
    ret.content=QByteArray::fromHex(temp.toLatin1());
    return ret;
}

bool Mitsubishi_MC_3E_bin::writeString(QString address,QString value)
{
    QString ret=NULL;
    QByteArray ary=QByteArray::fromStdString(value.toStdString());
    QString str=ary.toHex();
    unsigned int length=str.length()/4;
    ret=writeData(address,length,false,str);
    return (ret=="0000");
}
