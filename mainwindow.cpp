#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("QT三菱MC-3E帧读写寄存器");

    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UNICODE"));
            //setCodecForCStrings(QTextCodec::codecForName("GB2312"));

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
    myT1=new MyThread(this);
    t1=new QThread(this);
    t2=new QThread(this);
    t3=new QThread(this);

    readPlc = new ReadPlc();
    readDb = new ReadDatabase();
    writeDb = new writeDatabase();

    readPlc->moveToThread(t1);
    readDb->moveToThread(t2);
    writeDb->moveToThread(t3);

    //通讯实现
    MC_3E = new Mit::MC_3E_bin(this);

    connect(readPlc,SIGNAL(update(QString)),this,SLOT(querySlot(QString)));//
    connect(readDb,SIGNAL(update()),this,SLOT(upDateSlot()));//更新主画面的表格
    connect(writeDb,SIGNAL(update()),this,SLOT(writeDbSlot()));//写数据库

    connect(this,SIGNAL(rPlc()),readPlc,SLOT(doWork()));
    connect(this,SIGNAL(rDb()),readDb,SLOT(doWork()));
    connect(this,SIGNAL(wDb()),writeDb,SLOT(doWork()));

    t1->start();//启动管理线程
    t2->start();//启动管理线程
    t3->start();//启动管理线程

    emit rPlc();//发信号启动槽函数
    emit rDb();//发信号启动槽函数
    emit wDb();//触发写数据库

    //50个线程压力测试
    //for(unsigned int i=0;i<sizeof(myTrd)/sizeof(myTrd[0]);i++)myTrd[i]=new MyThread();

    //connect(myT1,SIGNAL(upDate(QString,uint,bool)),MC_3E,SLOT(readSlot(QString,unsigned int,bool)));

    //状态栏的状态显示
    //connect(MC_3E,SIGNAL(networkChangedSignal(QAbstractSocket::SocketState)),this,SLOT(networkChengedSlot(QAbstractSocket::SocketState)));//变更状态栏显示状态
    connect(MC_3E,&Mit::MC_3E_bin::signalOpen,this,&MainWindow::networkOpen);//打开网络信号
    connect(MC_3E,&Mit::MC_3E_bin::signalClose,this,&MainWindow::networkClose);//关闭网络信号

    //主线程的实例连接
    //MC_3E->open(ui->IPLineEdit->text(),ui->portLineEdit->text().toInt(),1000);

    //槽读取和写入
    connect(this,SIGNAL(readSignal(QString,unsigned int,bool)),MC_3E,SLOT(readSlot(QString,unsigned int,bool)));
    connect(this,SIGNAL(writeSignal(QString,unsigned int,bool,QString)),MC_3E,SLOT(writeSlot(QString,unsigned int,bool,QString)));

    //emit openNetwork(ui->IPLineEdit->text(),ui->portLineEdit->text().toInt(),1000);
}

MainWindow::~MainWindow()
{

    readPlc->stop();
    readDb->stop();

    t1->quit();
    t2->quit();
    myT1->quit();
    myT2->quit();
    delete t1;
    delete t2;
    t1->deleteLater();
    t2->deleteLater();
    myT1->deleteLater();
    myT2->deleteLater();

    MC_3E->close();
    delete MC_3E;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{

    myT1->stop();
    myT1->quit();
    myT1->deleteLater();

    MC_3E->close();
    delete MC_3E;

    event->accept();
}

void MainWindow::on_readBtn_clicked()
{
    QString ret=MC_3E->read(ui->addressLineEdit->text(),ui->countLineEdit->text().toInt(),ui->wordModeCheckBox->isChecked()).value;
    ui->sendLineEdit->setText(format(MC_3E->readSendString()));
    ui->recvLineEdit->setText(format(MC_3E->readRecvString()));
    ui->resultLineEdit->setText(MC_3E->readErrorCode());
    ui->valueLineEdit->setText(ret);
}

void MainWindow::on_writeBtn_clicked()
{
    MC_3E->write(ui->addressLineEdit->text(),ui->countLineEdit->text().toInt(),ui->wordModeCheckBox->isChecked(),ui->valueLineEdit->text());
    ui->sendLineEdit->setText(format(MC_3E->readSendString()));
    ui->recvLineEdit->setText(format(MC_3E->readRecvString()));
    ui->resultLineEdit->setText(MC_3E->readErrorCode());
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
    MC_3E->open(ui->IPLineEdit->text(),ui->portLineEdit->text().toInt(),1000);
    //emit openNetwork(ui->IPLineEdit->text(),ui->portLineEdit->text().toInt(),1000);
}

void MainWindow::on_btnClose_clicked()
{
    MC_3E->close();
    //emit closeNetwork();
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
    qDebug()<<"start slot.";
    t1->start();
    t2->start();

    //myT1->start();
    //myT2->start();
    //t1->start();
    //for(int i=0;i<sizeof(myTrd)/sizeof(myTrd[0]);i++)myTrd[i]->start();
}


void MainWindow::on_actionStop_triggered()
{
    qDebug()<<"stop slot.";
    readPlc->stop();
    readDb->stop();
    //t2->quit();
    //myT1->stop();
    //myT1->quit();
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

void MainWindow::upDateSlot()//把数据库读出来并显示到UI
{
    model = new QSqlQueryModel;
    model->setQuery("select * from user");
    ui->tableView->setModel(model);
    mList.append(model);
    while(mList.size()>=2)
    {
        mList.first()->deleteLater();
        mList.removeFirst();
    }
}

void MainWindow::querySlot(QString s)//追加数据
{
    qDebug()<<"get signal str:"<<s;
    writeDBbuf.append(s);
}

void MainWindow::writeDbSlot()//写数据库
{
    QStringList list=writeDBbuf;
    writeDBbuf.clear();
    foreach(QString s,list)
    {
        qDebug()<<"buf str:"<<s;
        QSqlQuery query;
        query.exec(s);
    }
    list.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////read
void MainWindow::on_readBool_triggered()
{
    bool ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3E->readBool(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readByte_triggered()
{
    unsigned char ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3E->readByte(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readShort_triggered()
{
    short ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3E->readShort(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readUShort_triggered()
{
    unsigned short ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3E->readUShort(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readInt_triggered()
{
    int ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3E->readInt(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readUInt_triggered()
{
    unsigned int ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3E->readUInt(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readLong_triggered()
{
    long ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3E->readLongInt(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}


void MainWindow::on_readULong_triggered()
{
    unsigned long ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3E->readULongInt(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readLongLong_triggered()
{
    long long ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3E->readLongLongInt(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readULongLong_triggered()
{
    unsigned long long ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3E->readULongLongInt(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readFloat_triggered()
{
    float ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3E->readFloat(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readDouble_triggered()
{
    double ret;
    QString address=ui->addressLineEdit->text();
    ret=MC_3E->readDouble(address).content;
    ui->valueLineEdit->setText(QString::number(ret));
}

void MainWindow::on_readString_triggered()
{
    QString ret;
    QString address=ui->addressLineEdit->text();
    int count=ui->countLineEdit->text().toUInt();
    ret=MC_3E->readString(address,count).content;
    ui->valueLineEdit->setText(ret);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////write
void MainWindow::on_writeBool_triggered()
{
    QString address=ui->addressLineEdit->text();
    bool value=ui->valueLineEdit->text().replace(expBin,"").toUpper().left(1)=="1"?1:0;//写之前过滤无用内容
    MC_3E->writeBool(address,value);
}

void MainWindow::on_writeByte_triggered()
{
    QString address=ui->addressLineEdit->text();
    unsigned char value=(unsigned char) ui->valueLineEdit->text().replace(expDec,"").toUpper().toUInt();//写之前过滤无用内容
    MC_3E->writeByte(address,value);
}

void MainWindow::on_writeShort_triggered()
{
    QString address=ui->addressLineEdit->text();
    short value=(short) ui->valueLineEdit->text().replace(expSDec,"").toUpper().toInt();//写之前过滤无用内容
    MC_3E->writeShort(address,value);
}

void MainWindow::on_writeUShort_triggered()
{
    QString address=ui->addressLineEdit->text();
    unsigned short value=(unsigned short) ui->valueLineEdit->text().replace(expDec,"").toUpper().toUInt();//写之前过滤无用内容
    MC_3E->writeUShort(address,value);
}

void MainWindow::on_writeInt_triggered()
{
    QString address=ui->addressLineEdit->text();
    int value=(int) ui->valueLineEdit->text().replace(expSDec,"").toUpper().toInt();//写之前过滤无用内容
    MC_3E->writeInt(address,value);
}

void MainWindow::on_writeUInt_triggered()
{
    QString address=ui->addressLineEdit->text();
    unsigned int value=(unsigned int) ui->valueLineEdit->text().replace(expDec,"").toUpper().toUInt();//写之前过滤无用内容
    MC_3E->writeUInt(address,value);
}

void MainWindow::on_writeLong_triggered()
{
    QString address=ui->addressLineEdit->text();
    long value=(long) ui->valueLineEdit->text().replace(expSDec,"").toUpper().toLong();//写之前过滤无用内容
    MC_3E->writeLongInt(address,value);
}

void MainWindow::on_writeULong_triggered()
{
    QString address=ui->addressLineEdit->text();
    unsigned long value=(unsigned long) ui->valueLineEdit->text().replace(expSDec,"").toUpper().toULong();//写之前过滤无用内容
    MC_3E->writeULongInt(address,value);
}

void MainWindow::on_writeLongLong_triggered()
{
    QString address=ui->addressLineEdit->text();
    long long value=(long long) ui->valueLineEdit->text().replace(expSDec,"").toUpper().toLongLong();//写之前过滤无用内容
    MC_3E->writeLongLongInt(address,value);
}

void MainWindow::on_writeULongLong_triggered()
{
    QString address=ui->addressLineEdit->text();
    unsigned long long value=(unsigned long long) ui->valueLineEdit->text().replace(expSDec,"").toUpper().toULongLong();//写之前过滤无用内容
    MC_3E->writeULongLongInt(address,value);
}

void MainWindow::on_writeFloat_triggered()
{
    QString address=ui->addressLineEdit->text();
    float value=ui->valueLineEdit->text().replace(expFDec,"").toUpper().toFloat();//写之前过滤无用内容
    MC_3E->writeFloat(address,value);
}

void MainWindow::on_writeDouble_triggered()
{
    QString address=ui->addressLineEdit->text();
    double value=ui->valueLineEdit->text().replace(expFDec,"").toUpper().toDouble();//写之前过滤无用内容
    MC_3E->writeDouble(address,value);
}

void MainWindow::on_writeString_triggered()
{
    QString address=ui->addressLineEdit->text();
    QString value=ui->valueLineEdit->text();
    MC_3E->writeString(address,value);
}
