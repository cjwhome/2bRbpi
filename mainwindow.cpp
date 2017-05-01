#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "defines.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug()<<"2";
    ui->setupUi(this);
    qDebug()<<"3";
    started_file = false;
    usbMounted = false;
    xmlDeviceReader = new XmlDeviceReader(":/deviceConfig.xml");
    xmlDeviceReader->read();

    createDevice();
    setupSerial();
}

MainWindow::~MainWindow()
{
    delete ui;
}



//build a device from the xml and prepare place to put the data
void MainWindow::createDevice(void){
    int i;
    twobTechDevice = xmlDeviceReader->getADevice(1);
    qDebug()<<"createdevice1";
    deviceProfile.setDevice_name(twobTechDevice.device_name);
    deviceProfile.setCom_port(twobTechDevice.getCom_port());
    deviceProfile.setBaud_rate(twobTechDevice.getBaud_rate());
    qDebug()<<"Device Profile name: "<<deviceProfile.getDevice_name();
    qDebug()<<"Device Profile comport: "<<deviceProfile.getCom_port();

    //determine the index of elements
    for(i=0;i<twobTechDevice.data_items.size();i++){
        SerialDataItem serialDataItem = twobTechDevice.data_items.at(i);
        if(serialDataItem.getName() == "Date")
            deviceProfile.setDate_position(i);
        else if(serialDataItem.getName()=="Time")
            deviceProfile.setTime_position(i);
        else if(serialDataItem.getPriority()==0){

            deviceProfile.setMain_display_position(i);
            deviceProfile.setMain_display_units(serialDataItem.getUnits());
            deviceProfile.setMain_display_name(serialDataItem.getName());
        }else if(serialDataItem.getPriority()==1){

            deviceProfile.setDiagnosticA_units(serialDataItem.getUnits());
            deviceProfile.setDiagnosticA_name(serialDataItem.getName());
            deviceProfile.setDiagnosticA_position(i);
        }else if(serialDataItem.getPriority()==2){

            deviceProfile.setDiagnosticB_units(serialDataItem.getUnits());
            deviceProfile.setDiagnosticB_name(serialDataItem.getName());
            deviceProfile.setDiagnosticB_position(i);
        }else if(serialDataItem.getPriority()==3){

            deviceProfile.setDiagnosticC_units(serialDataItem.getUnits());
            deviceProfile.setDiagnosticC_name(serialDataItem.getName());
            deviceProfile.setDiagnosticC_position(i);
        }else if(serialDataItem.getPriority()==4){

            deviceProfile.setDiagnosticD_units(serialDataItem.getUnits());
            deviceProfile.setDiagnosticD_name(serialDataItem.getName());
            deviceProfile.setDiagnosticD_position(i);
        }else if(serialDataItem.getPriority()==5){

            deviceProfile.setDiagnosticE_units(serialDataItem.getUnits());
            deviceProfile.setDiagnosticE_name(serialDataItem.getName());
            deviceProfile.setDiagnosticE_position(i);
        } else if(serialDataItem.getPriority() == 6) {

            deviceProfile.setDiagnosticF_uints(serialDataItem.getUnits());
            deviceProfile.setDiagnosticF_name(serialDataItem.getName());
            deviceProfile.setDiagnosticF_position(i);
        }
        //qDebug()<<"For "<<i<<" priority="<<serialDataItem.getPriority();
    }
    qDebug()<<"Finished Profile Setup";
    deviceProfile.setNumber_of_columns(i);
    //qDebug()<<"Number of columns:"<<deviceProfile.getNumber_of_columns();

}

void MainWindow::setupSerial(){
    QThread *thread = new QThread(this);
    serialHandler = new SerialHandler(thread);
    //serialHandler->writeSync(new QString("test"));
    connect(serialHandler, SIGNAL(dataAvailable(QString)), this, SLOT(newDataLine(QString)));
   //QTimer::singleShot(2000, serialHandler, SLOT(updateSettings()));
    qDebug()<<"Setup Serial port";
}


void MainWindow::newDataLine(QString dLine){
    qDebug()<<"New Line: "<<dLine;

    QElapsedTimer debugTimer;
    debugTimer.start();

    if(parseDataLine(dLine)){

        qDebug()<<"Parsed data line";
        //emit validDataReady();
    }


}



bool MainWindow::parseDataLine(QString dLine){
    QElapsedTimer debugTimer;
    debugTimer.start();

    QStringList fields;
    QVector<double> t,u;
    QDateTime tempDate;

    tempDLine = dLine;

    dLine.remove(QRegExp("[\\n\\t\\r]"));
    //qDebug()<<dLine;
    fields = dLine.split(QRegExp(","));
    if(fields.length()==deviceProfile.getNumber_of_columns()){

        QList<SerialDataItem> parsedDataRecord;       //create an list of parsed data to append to the list of all parsed records
        for(int a=0;a<deviceProfile.getNumber_of_columns();a++){
            SerialDataItem serialDataItem;
            if(a!=deviceProfile.getDate_position()||a!=deviceProfile.getTime_position()){
                serialDataItem.setDvalue(fields[a].toDouble());
            }
            parsedDataRecord.append(serialDataItem);
        }

        tempDate = QDateTime::fromString(fields[deviceProfile.getDate_position()]+fields[deviceProfile.getTime_position()], "dd/MM/yyhh:mm:ss");
        if(tempDate.date().year()<2000)
            tempDate = tempDate.addYears(100);      //only if century is not part of the format

        SerialDataItem serialDataItemb;
        serialDataItemb.setDateTime(tempDate);
        parsedDataRecord.insert(deviceProfile.getDate_position(),serialDataItemb);

        if(allParsedRecordsList.size()<MAXIMUM_PARSED_DATA_RECORDS){

            allParsedRecordsList.append(parsedDataRecord);
        }else{

            allParsedRecordsList.removeFirst();
            allParsedRecordsList.append(parsedDataRecord);
            qDebug()<<"Maxed out the qlist size, removing first element and adding";
        }

        //updateAverage(parsedDataRecord.at(deviceProfile.getMain_display_position()).getDvalue());

        data_point = QDateTime::currentDateTime().toTime_t();

        updateDisplay();
        //qDebug()<<"parseDataLine: "<<debugTimer.elapsed();
        return true;


    }if(fields.length()==deviceProfile.getNumber_of_columns()+1){
        qDebug()<<"Logging is detected";
        QList<SerialDataItem> parsedDataRecord;       //create an list of parsed data to append to the list of all parsed records
        for(int a=1;a<deviceProfile.getNumber_of_columns()+1;a++){
            SerialDataItem serialDataItem;
            if(a!=deviceProfile.getDate_position()||a!=deviceProfile.getTime_position()){
                serialDataItem.setDvalue(fields[a].toDouble());
            }
            parsedDataRecord.append(serialDataItem);
            //qDebug()<<"field["<<a;
            //qDebug()<<"]="<<fields.at(a);
        }

        tempDate = QDateTime::fromString(fields[deviceProfile.getDate_position()+1]+fields[deviceProfile.getTime_position()+1], "dd/MM/yyhh:mm:ss");
        if(tempDate.date().year()<2000)
            tempDate = tempDate.addYears(100);      //only if century is not part of the format
        //qDebug()<<"Tempdate:"<<tempDate;
        SerialDataItem serialDataItemb;
        serialDataItemb.setDateTime(tempDate);
        parsedDataRecord.insert(deviceProfile.getDate_position(),serialDataItemb);

        if(allParsedRecordsList.size()<MAXIMUM_PARSED_DATA_RECORDS){

            allParsedRecordsList.append(parsedDataRecord);
        }else{

            allParsedRecordsList.removeFirst();
            allParsedRecordsList.append(parsedDataRecord);
            qDebug()<<"Maxed out the qlist size, removing first element and adding";
        }

        updateAverage(parsedDataRecord.at(deviceProfile.getMain_display_position()).getDvalue());

        data_point = QDateTime::currentDateTime().toTime_t();




        updateDisplay();
        //qDebug()<<"parseDataLine: "<<debugTimer.elapsed();
        return true;


    }else{
        qDebug()<<"Incomplete line: "<<fields.length()<<" columns.";
        qDebug()<<"parseDataLine: "<<debugTimer.elapsed();
        return false;
    }
}

void MainWindow::updateAverage(double value) {
    QElapsedTimer debugTimer;
    debugTimer.start();
    //int avgIndex = settings->value("Avg").toInt();
    int avgIndex = 0;
    qDebug()<<"Avg Index: "<<avgIndex;
    int avgDur;
    if (avgIndex == 0) {
        avgDur = 2;
    } else if (avgIndex == 1) {
        avgDur = 10;
    } else if (avgIndex == 2) {
        avgDur = 60;
    } else if (avgIndex == 3) {
        avgDur = 60 * 15;
    } else if (avgIndex == 4) {
        avgDur = 60 * 60;
    }
    int avgCount = avgDur / 2;
    if(avgCount < 1) {
        avgCount = 1;
    }
    //qDebug()<<"Avg Count: "<<avgCount;

    int curLength = avgList.length();
    if(curLength > avgCount) {
        //Average The List By Pairs Until It is Less Than The New Value
        while(avgList.length() > avgCount) {
            qDebug()<<"Consolidating Avg List";
            QList<double> temp;
            for(int i = 0; i < (avgList.length() / 2); i++) {
                double a = avgList.at(i);
                i++;
                double b = avgList.at(i);

                double tAvg = (a + b) / 2;
                temp << tAvg;
            }
            //qDebug()<<"Temp List: "<<temp;
            avgList = temp;
        }
    } else if(curLength == avgCount) {
        QList<double> tempList;
        for(int i = 0; i < (avgList.length() - 1); i++) {
            tempList<<avgList.at(i+1);
        }
        avgList = tempList;
    }
    avgList<<value;
    qDebug()<<"Avg List: "<<avgList;

    double sum = 0;
    for(int i = 0; i < avgList.length(); i++) {
        sum += avgList.at(i);
    }
    avg = sum / avgList.length();

    qDebug()<<"updateAverage: "<<debugTimer.elapsed();
}

void MainWindow::updateDisplay(void){


    double current_value;

    if(!started_file){
        qDebug()<<"File not started yet, attempting to start file";

        this->createFileName();
    }
    SerialDataItem tempSerialDataItem;

    tempSerialDataItem = allParsedRecordsList.at(allParsedRecordsList.size() -1).at(deviceProfile.getMain_display_position());

    current_value = avg;//tempSerialDataItem.getDvalue();
    qDebug()<<"Current measurement for "<<tempSerialDataItem.getName()<<" is "<<current_value<<" "<<tempSerialDataItem.getUnits();




    QString mesStr = QString::number(current_value);

    if(mesStr.contains(".")) {
        while(mesStr.at(mesStr.length() - 2) != '.') {
            mesStr = mesStr.mid(0, mesStr.length() - 1);
        }
    } else {
       mesStr.append(".0");
    }

    tempSerialDataItem = allParsedRecordsList.at(allParsedRecordsList.size() -1).at(deviceProfile.getDate_position());



    this->writeFile();

    qDebug()<<"Here9";
}

void MainWindow::createFileName(void){
    QElapsedTimer debugTimer;
    debugTimer.start();
    QString dataPath;
    if(allParsedRecordsList.empty())
        return;

    if(fileWriter.createDataFolder(deviceProfile.getDevice_name())){
        //qDebug()<<"found usb mounted at path:"<<fileWriter.getUsbPath();
        dataPath = fileWriter.getFull_data_path();
    } else {
        dataPath = "/var/www/html/"+deviceProfile.getDevice_name() + "/";
        QDir dir(dataPath);
        if(!dir.exists()) {
            QDir().mkdir(dataPath);
        }
    }
    SerialDataItem tempSerialDataItem = allParsedRecordsList.at(allParsedRecordsList.size()-1).at(deviceProfile.getDate_position());
    QDateTime tempDateTime = tempSerialDataItem.getDateTime();

    QString fileName = dataPath + tempDateTime.toString("ddMMyyhhmmss")+".csv";
    qDebug()<<"Filename:"<<fileName;
    currentFile.setFileName(fileName);
    //currentFile = new QFile(fileName);
    started_file = true;
    qDebug()<<"createFileName: "<<debugTimer.elapsed();
}

void MainWindow::writeFile(void){
    QElapsedTimer debugTimer;
    debugTimer.start();

    if(currentFile.open(QIODevice::Append))
    {
        //qDebug()<<"Writing file: "<<currentFile.fileName();
        if(!tempDLine.contains("\n")){
            //qDebug()<<"Adding line feed carriage return";
            tempDLine.append("\n");
        }
        QTextStream stream(&currentFile);
        stream<<tempDLine;
        currentFile.close();
    }

    //qDebug()<<"writeFile: "<<debugTimer.elapsed();
}
