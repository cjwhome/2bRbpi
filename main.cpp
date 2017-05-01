#include "mainwindow.h"
#include "defines.h"
#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <QThread>
#include <QProcess>
#include <QDebug>



int main(int argc, char *argv[])
{
    qDebug()<<"1";
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    return a.exec();
}
