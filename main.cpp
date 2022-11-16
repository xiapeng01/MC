#include "mainwindow.h"

#include <QApplication>
#include "connectDB.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!createConnection()) return 1;//创建数据库连接
    MainWindow w;    
    w.show();
    //qDebug() << __cplusplus;
    return a.exec();
}
