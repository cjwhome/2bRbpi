#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QDebug>
#include <QSettings>
#include <QtNetwork>
#include "xmldevicereader.h"
#include "twobtechdevice.h"

class SerialHandler : public QObject
{
    Q_OBJECT
public:
    explicit SerialHandler(QThread *thr, QObject *parent = 0);

    enum Types
    {
        Inactive,
        Asynchronously,
        Synchronously,
        Finished
    };
    SerialHandler::Types currentConnectionType;
    void setCommandString(QString command);

signals:
    void finished();

    void error();
    void dataAvailable(QString);

public slots:
    void writeSync(QString *dat);
    void writeAsync(QString *dat);
    void writeChar(char c);

    void dataReady();
    void netDataReady();
    void readData(QString data);

    void updateSettings();
    void write106(QString *dat);
    void sendSetCommandString(void);

private slots:
    void newConnection();

private:
    QThread *thread;

    QString portName;
    QSerialPort *serialPort;
    int baudRate;

    void configSerialPort();
    TwobTechDevice device;
    XmlDeviceReader *reader;

    QString *data;
    QList<QString*> dataList;
    void checkQueue();

    //void dataReady();
    void handleSyncData(QString retData);
    int syncIndex;

    QString commandString;

    QSettings *settings;
    bool gettingSettings;

    //Netowrking
    QTcpServer server;
    QList<QTcpSocket *> netSockets;
};

#endif // SERIALHANDLER_H
