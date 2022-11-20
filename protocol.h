#include "ENet.h"
#include "opmethod.h"



#ifndef MT_MC_3E_BIN
#define MT_MC_3E_BIN



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 三菱协议集合
///
namespace  Mit {

//MC协议
class MC_3E_bin:public OPMethod
{
    Q_OBJECT

//以下为方法
public:
    MC_3E_bin(QObject *parent = nullptr);//构造函数
    ~MC_3E_bin();//析构函数

protected:
    //以下为私有工具方法
    QString enCode(int,QString address,unsigned int length,bool wordMode,QString value);//构造数据帧
    retData deCode(QString Str);//解析数据帧,将解析结果分作两个部分，第一个部分是错误代码，第二部分是返回值

    regType registerMode(QString address,bool mode);//判断寄存器类型


//以下为私有属性
private:    
    bool busy;
    int token,lastToken;
};

}
#endif
