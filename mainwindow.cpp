#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("QT三菱MC-3E帧读写寄存器");

    //打印当前线程ID
    qDebug()<<"主线程ID:"<<QThread::currentThreadId();

    //预置初始内容
    ui->addressLineEdit->setText("D100");//初始化值
    ui->countLineEdit->setText("5");//初始化值
    ui->valueLineEdit->setText("1234");//初始化值
    ui->IPLineEdit->setText("127.0.0.1");
    ui->portLineEdit->setText("6000");

    //按钮状态设置
    ui->btnClose->setEnabled(false);
    ui->readBtn->setEnabled(false);
    ui->writeBtn->setEnabled(false);

    //初始化新的线程
    t1=new QThread();
    t2=new QThread();

    MC_3Einstance=new Mitsubishi_MC_3E_bin();
    //MC_3Einstance->moveToThread(t1);

    myT1=new MyThread();
    myT2=new MyThread();

    for(unsigned int i=0;i<sizeof(myTrd)/sizeof(myTrd[0]);i++)myTrd[i]=new MyThread();


    connect(myT1,SIGNAL(upDate(QString,uint,bool)),MC_3Einstance,SLOT(readSlot(QString,unsigned int,bool)));

    connect(MC_3Einstance,SIGNAL(networkChangedSignal(QAbstractSocket::SocketState)),this,SLOT(networkChengedSlot(QAbstractSocket::SocketState)));
    connect(MC_3Einstance,&Mitsubishi_MC_3E_bin::networkOpen,this,&MainWindow::networkOpen);
    connect(MC_3Einstance,&Mitsubishi_MC_3E_bin::networkClose,this,&MainWindow::networkClose);


    connect(this,SIGNAL(openNetwork(QString,int,int)),MC_3Einstance,SLOT(openSlot(QString,int,int)));//打开网络
    connect(this,SIGNAL(closeNetwork()),MC_3Einstance,SLOT(closeSlot()));//关闭网络

    MC_3Einstance->open(ui->IPLineEdit->text(),ui->portLineEdit->text().toInt(),1000);

    connect(this,SIGNAL(readSignal(QString,unsigned int,bool)),MC_3Einstance,SLOT(readSlot(QString,unsigned int,bool)));
    connect(this,SIGNAL(writeSignal(QString,unsigned int,bool,QString)),MC_3Einstance,SLOT(writeSlot(QString,unsigned int,bool,QString)));

    //myT1->start();

    if(!t1->isRunning()) t1->start();
    //if(!t2->isRunning()) t2->start();
    emit openNetwork(ui->IPLineEdit->text(),ui->portLineEdit->text().toInt(),1000);

}

MainWindow::~MainWindow()
{
    t1->quit();
    QThread::msleep(1000);
    t1->deleteLater();
    delete ui;
}

void MainWindow::on_readBtn_clicked()
{
    QString ret=MC_3Einstance->read(ui->addressLineEdit->text(),ui->countLineEdit->text().toInt(),ui->wordModeCheckBox->isChecked()).value;
    ui->sendLineEdit->setText(format(MC_3Einstance->readSendString()));
    ui->recvLineEdit->setText(format(MC_3Einstance->readRecvString()));
    ui->resultLineEdit->setText(MC_3Einstance->readErrorCode());
    ui->valueLineEdit->setText(ret);
}

void MainWindow::on_writeBtn_clicked()
{
    MC_3Einstance->write(ui->addressLineEdit->text(),ui->countLineEdit->text().toInt(),ui->wordModeCheckBox->isChecked(),ui->valueLineEdit->text());
    ui->sendLineEdit->setText(format(MC_3Einstance->readSendString()));
    ui->recvLineEdit->setText(format(MC_3Einstance->readRecvString()));
    ui->resultLineEdit->setText(MC_3Einstance->readErrorCode());
}

QString MainWindow::format(QString str)
{
    QString ret=str;
    int i=2;
    while(i<ret.length())
    {
        ret.insert(i,' ');
        i+=3;
    }
    return ret;
}

void MainWindow::on_btnConnect_clicked()
{
    //MC_3Einstance->open(ui->IPLineEdit->text(),ui->portLineEdit->text().toInt(),1000);
    emit openNetwork(ui->IPLineEdit->text(),ui->portLineEdit->text().toInt(),1000);
}

void MainWindow::on_btnClose_clicked()
{
    //MC_3Einstance->close();
    emit closeNetwork();
}

void MainWindow::networkChengedSlot(QAbstractSocket::SocketState state)
{
    QMetaEnum met=QMetaEnum::fromType<QAbstractSocket::SocketState>();
    ui->statusBar->showMessage(met.valueToKey(state));
}

void MainWindow::networkOpen()
{
    ui->readBtn->setEnabled(true);
    ui->writeBtn->setEnabled(true);
    ui->btnConnect->setEnabled(false);
    ui->btnClose->setEnabled(true);
}

void MainWindow::networkClose()
{
    ui->readBtn->setEnabled(false);
    ui->writeBtn->setEnabled(false);
    ui->btnConnect->setEnabled(true);
    ui->btnClose->setEnabled(false);
}

void MainWindow::on_actionStart_triggered()
{
    myT1->start();
    //myT2->start();
    //t1->start();
    //for(int i=0;i<sizeof(myTrd)/sizeof(myTrd[0]);i++)myTrd[i]->start();
}


void MainWindow::on_actionStop_triggered()
{
    //t2->quit();
    myT1->stop();
    //myT2->stop();
    //for(int i=0;i<sizeof(myTrd)/sizeof(myTrd[0]);i++)myTrd[i]->stop();
}

void MainWindow::on_actionReadSlot_triggered()
{
    emit readSignal(ui->addressLineEdit->text(),ui->countLineEdit->text().toInt(),ui->wordModeCheckBox->isChecked());
}

void MainWindow::on_actionWriteSlot_triggered()
{
    emit writeSignal(ui->addressLineEdit->text(),ui->countLineEdit->text().toInt(),ui->wordModeCheckBox->isChecked(),ui->valueLineEdit->text());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////read
void MainWindow::on_readBool_triggered()
{
    bool ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3Einstance->readBool(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readByte_triggered()
{
    unsigned char ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3Einstance->readByte(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readShort_triggered()
{
    short ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3Einstance->readShort(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readUShort_triggered()
{
    unsigned short ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3Einstance->readUShort(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_intRead_triggered()
{
    int ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3Einstance->readInt(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_uintRead_triggered()
{
    unsigned int ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3Einstance->readUInt(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readLong_triggered()
{
    long ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3Einstance->readLongInt(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}


void MainWindow::on_readULong_triggered()
{
    unsigned long ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3Einstance->readULongInt(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}


void MainWindow::on_readFloat_triggered()
{
    float ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3Einstance->readFloat(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}


void MainWindow::on_readDouble_triggered()
{
    double ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3Einstance->readDouble(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readString_triggered()
{
    QString ret;
    QString address=ui->addressLineEdit->text();
    int count=ui->countLineEdit->text().toUInt();
    ret=MC_3Einstance->readString(address,count).content;
    ui->valueLineEdit->setText(ret);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////write
void MainWindow::on_writeBool_triggered()
{
    QString address=ui->addressLineEdit->text();
    bool value=ui->valueLineEdit->text().replace(expBin,"").toUpper().left(1)=="1"?1:0;//写之前过滤无用内容
    MC_3Einstance->writeBool(address,value);
}

void MainWindow::on_writeByte_triggered()
{
    QString address=ui->addressLineEdit->text();
    unsigned char value=(unsigned char) ui->valueLineEdit->text().replace(expDec,"").toUpper().toUInt();//写之前过滤无用内容
    MC_3Einstance->writeByte(address,value);
}

void MainWindow::on_writeShort_triggered()
{
    QString address=ui->addressLineEdit->text();
    short value=(short) ui->valueLineEdit->text().replace(expSDec,"").toUpper().toInt();//写之前过滤无用内容
    MC_3Einstance->writeShort(address,value);
}

void MainWindow::on_ushortWrite_triggered()
{
    QString address=ui->addressLineEdit->text();
    unsigned short value=(unsigned short) ui->valueLineEdit->text().replace(expDec,"").toUpper().toUInt();//写之前过滤无用内容
    MC_3Einstance->writeUShort(address,value);
}

void MainWindow::on_intWrite_triggered()
{
    QString address=ui->addressLineEdit->text();
    int value=(int) ui->valueLineEdit->text().replace(expSDec,"").toUpper().toInt();//写之前过滤无用内容
    MC_3Einstance->writeInt(address,value);
}

void MainWindow::on_uintWrite_triggered()
{
    QString address=ui->addressLineEdit->text();
    unsigned int value=(unsigned int) ui->valueLineEdit->text().replace(expDec,"").toUpper().toUInt();//写之前过滤无用内容
    MC_3Einstance->writeUInt(address,value);
}

void MainWindow::on_writeLong_triggered()
{
    QString address=ui->addressLineEdit->text();
    long value=(long) ui->valueLineEdit->text().replace(expSDec,"").toUpper().toULong();//写之前过滤无用内容
    MC_3Einstance->writeLongInt(address,value);
}

void MainWindow::on_writeULong_triggered()
{
    QString address=ui->addressLineEdit->text();
    unsigned long value=(unsigned long) ui->valueLineEdit->text().replace(expSDec,"").toUpper().toULong();//写之前过滤无用内容
    MC_3Einstance->writeULongInt(address,value);
}

void MainWindow::on_writeFloat_triggered()
{
    QString address=ui->addressLineEdit->text();
    float value=ui->valueLineEdit->text().replace(expFDec,"").toUpper().toFloat();//写之前过滤无用内容
    MC_3Einstance->writeFloat(address,value);
}

void MainWindow::on_writeDouble_triggered()
{
    QString address=ui->addressLineEdit->text();
    double value=ui->valueLineEdit->text().replace(expFDec,"").toUpper().toDouble();//写之前过滤无用内容
    MC_3Einstance->writeDouble(address,value);
}

void MainWindow::on_writeString_triggered()
{
    QString address=ui->addressLineEdit->text();
    QString value=ui->valueLineEdit->text();
    MC_3Einstance->writeString(address,value);
}

