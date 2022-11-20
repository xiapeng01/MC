#include "./protocol.h"



Mit::MC_3E_bin::MC_3E_bin(QObject *parent)
    :OPMethod{parent}
{

}

Mit::MC_3E_bin::~MC_3E_bin()
{
    ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//构造MC-3E帧
QString Mit::MC_3E_bin::enCode(int method,QString address,unsigned int length,bool wordMode,QString value)
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
retData Mit::MC_3E_bin::deCode(QString str)//只负责把返回帧的有效值分解出来
{
    retData ret;
    recvErrorCode.clear();
    if(str.left(12)=="D00000FFFF03")
    {
        if(str.length()>22) //正常情况，返回帧的长度大于21个ASCII字符
        {
            recvErrorCode=str.mid(20,2)+str.mid(18,2);//错误码交换高低字节
            ret.ErrCode=qFromBigEndian(recvErrorCode.toShort(nullptr,16));
            ret.value=str.mid(22,str.length());//错误码以后的都原样给出
        }else//强制替换为错误值,即使出错也给出两个成员
        {
            ret.ErrCode=-1;
            ret.value=0;
        }
        return ret;
    }else
    {
        qDebug()<<"Split String Error.";
        ret.ErrCode=-1;
        ret.value=0;
        return ret;
    }
}

regType Mit::MC_3E_bin::registerMode(QString address,bool mode)
{
    ;
    QString regName=address.toUpper().replace(expChar,"");
    regType boolMode=regType::bit,wordMode=regType::word;
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
