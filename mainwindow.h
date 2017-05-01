#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include "filewriter.h"
#include "serialhandler.h"
#include "twobtechdevice.h"
#include "xmldevicereader.h"
#include "deviceprofile.h"
#include "parseddata.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool started_file;
    bool usbMounted;
    QSerialPort *serial;

private slots:
    void updateAverage(double value);
    void newDataLine(QString dLine);
    bool parseDataLine(QString dLine);

private:

    XmlDeviceReader* xmlDeviceReader;
    TwobTechDevice twobTechDevice;

    QFile currentFile;
    QString tempDLine;



    double data_point;
    int data_index;
    double start_time_seconds;
    double main_display_value;

    int deviceNumber;

    bool notDone;

    DeviceProfile deviceProfile;
    ParsedData parsedData;

    QList<double> avgList;
    double avg;
    QList< QList<SerialDataItem> > allParsedRecordsList;
    FileWriter fileWriter;
    SerialHandler *serialHandler;




    void createDevice(void);
    void setupSerial();
    void updateDisplay();

    void initFile();
    void createFileName();
    void writeFile();


    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
