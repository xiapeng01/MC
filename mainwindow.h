#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QtNetwork>
#include "Mitsubishi_MC_3E.h"
#include <QMetaObject>
#include "ui_mainwindow.h"
#include <QRegularExpression>
#include <QStringConverter>
#include <QtEndian>
#include <QtConcurrent>
#include <QtConcurrent/QtConcurrent>
#include <type_traits>
#include <QFuture>
#include <QMutex>
#include <mythread.h>
#include <QEvent>
#include <QCloseEvent>
#include "workclass.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

//信号
signals:
    void readSignal(QString,unsigned int,bool);
    void writeSignal(QString,unsigned int,bool,QString);
    void openNetwork(QString,int,int);
    void closeNetwork();

    void rPlc();//操作寄存器
    void rDb();//只是定时触发
    void wDb();//定时触发

//方法
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent *);

    QString format(QString str);

    ReadPlc *readPlc;
    ReadDatabase *readDb;
    writeDatabase *writeDb;

private:
    //QString read(QString,int,bool);
    void write(QString,int,bool,QString);

//属性
private:
    Ui::MainWindow *ui;
    Mitsubishi_MC_3E_bin *MC_3Einstance;
    QThread *t1,*t2,*t3;
    //QMutex mut;
    bool status;
    MyThread *myT1,*myT2,*myTrd[50];

    QStringList writeDBbuf;

    QSqlQueryModel *model,*mTemp;
    QList<QSqlQueryModel *> mList;

//槽
private slots:

    void upDateSlot();
    void querySlot(QString s);
    void writeDbSlot();

    void on_readBtn_clicked();
    void on_writeBtn_clicked();
    void on_btnConnect_clicked();
    void on_btnClose_clicked();

    void networkChengedSlot(QAbstractSocket::SocketState);

    void networkOpen();
    void networkClose();

    void on_actionStart_triggered();

    void on_actionStop_triggered();


    void on_actionReadSlot_triggered();
    void on_actionWriteSlot_triggered();

    void on_readBool_triggered();
    void on_readByte_triggered();
    void on_writeBool_triggered();
    void on_writeByte_triggered();
    void on_readShort_triggered();
    void on_writeShort_triggered();
    void on_readUShort_triggered();
    void on_readInt_triggered();
    void on_readUInt_triggered();
    void on_writeUShort_triggered();
    void on_writeInt_triggered();
    void on_writeUInt_triggered();
    void on_readLong_triggered();
    void on_readULong_triggered();
    void on_readFloat_triggered();
    void on_readDouble_triggered();
    void on_readString_triggered();
    void on_writeLong_triggered();
    void on_writeULong_triggered();
    void on_writeFloat_triggered();
    void on_writeDouble_triggered();
    void on_writeString_triggered();
    void on_readLongLong_triggered();
    void on_readULongLong_triggered();
    void on_writeLongLong_triggered();
    void on_writeULongLong_triggered();
};
#endif // MAINWINDOW_H
