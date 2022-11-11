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

//方法
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString format(QString str);
private:
    //QString read(QString,int,bool);
    void write(QString,int,bool,QString);

//属性
private:
    Ui::MainWindow *ui;
    Mitsubishi_MC_3E_bin *MC_3Einstance;
    QThread *t1,*t2;
    //QMutex mut;
    bool status;
    MyThread *myT1,*myT2,*myTrd[50];

//槽
private slots:
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
    void on_intRead_triggered();
    void on_uintRead_triggered();
    void on_ushortWrite_triggered();
    void on_intWrite_triggered();
    void on_uintWrite_triggered();
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
};
#endif // MAINWINDOW_H
