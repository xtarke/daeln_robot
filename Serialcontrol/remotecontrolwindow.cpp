#include <iostream>

#include "remotecontrolwindow.h"
#include "ui_remotecontrolwindow.h"
#include "serialsettingsdialog.h"

#include <QDebug>
#include <QtTest/QSignalSpy>

#include <unistd.h>

RemoteControlWindow::RemoteControlWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoteControlWindow)
{
    ui->setupUi(this);
    
    error_message = new QErrorMessage(this);
    settings = new SettingsDialog;

    /* Setup serial communication */
    serial = new QSerialPort(this);
    comm = new Communication(*serial);

    memset(ServoPos, 50 , sizeof(ServoPos));
    
    currentUpdateTimer = new QTimer(this);

//    timer_yes = new QTimer(this);
//    timer_no = new QTimer(this);
//    timer_leftArm = new QTimer(this);
//    timer_rigtArm = new QTimer(this);
        
//    sensors_thread = new SensorTread(*comm);
        
//    connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT (on_dialChanged()));

      connect(currentUpdateTimer, SIGNAL(timeout()), this, SLOT(update_servo_current()));

//    connect(sensors_thread, SIGNAL(ReadMe_A0(double)), ui->lcdNumberA0, SLOT(display(double)));
//    connect(sensors_thread, SIGNAL(ReadMe_A1(double)), ui->lcdNumberA1, SLOT(display(double)));
//    connect(sensors_thread, SIGNAL(ReadMe_A2(double)), ui->lcdNumberA2, SLOT(display(double)));
//    connect(sensors_thread, SIGNAL(ReadMe_A3(double)), ui->lcdNumberA3, SLOT(display(double)));
    
//    connect(timer_no, SIGNAL(timeout()), this, SLOT(onTimerNoTimeout()));
//    connect(timer_yes, SIGNAL(timeout()), this, SLOT(onTimerYesTimeout()));

//    connect(timer_leftArm, SIGNAL(timeout()), this, SLOT(onTimerLeftArmTimeout()));
//    connect(timer_rigtArm, SIGNAL(timeout()), this, SLOT(onTimerRightArmTimeout()));


    fillServoParameters();
}

RemoteControlWindow::~RemoteControlWindow()
{
    if (comm->isReady())
        comm->SerialClose();

    delete ui;
    delete settings;
    delete error_message;
}


void RemoteControlWindow::on_toolButtonPort_clicked(){
    settings->show();   
}

void RemoteControlWindow::on_toolButtonOpen_clicked(){
        
    SettingsDialog::Settings p = settings->settings();

    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);

    comm->SerialOpen();

    if (!comm->isReady()){
        if (!comm->SerialOpen()) {
            error_message->showMessage("Could not open serial");
            return;
        }
    }
    
    ui->toolButtonClose->setEnabled(true);
    ui->toolButtonIncrease->setEnabled(true);
    ui->toolButtonDecrease->setEnabled(true);
}

void RemoteControlWindow::on_toolButtonClose_clicked()
{
    if (comm->isReady()){
        comm->SerialClose();
        ui->toolButtonClose->setEnabled(false);
    }
}

void RemoteControlWindow::on_toolButtonCurr_clicked(){
    static bool on = false;

    if (on){
        currentUpdateTimer->stop();
        on = false;
    }
    else {
        currentUpdateTimer->setInterval(100);
        currentUpdateTimer->start();
        on = true;
    }
}



void RemoteControlWindow::on_toolButtonIncrease_clicked(){
//    QByteArray data;
    
//    Head head(*comm);
    
//    int currentIndex = ui->comboBoxServoList->currentIndex();
//    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
    
//    if (!comm->isReady()){
//        error_message->showMessage("Serial port is not open!");
//        return;
//    }
    
//    ServoPos[ServoId]+=10;

//    if (ServoPos[ServoId] > 100){
//        ServoPos[ServoId] = 100;
//        return;
//    }
    
//    if (ServoId == 1)
//        head.move_h(ServoPos[ServoId]);
//    else
//        head.move_v(ServoPos[ServoId]);
    
    
//    //ui->dial->setValue(int(ServoPos[ServoId]));
}

void RemoteControlWindow::on_toolButtonDecrease_clicked(){  
        
//    QByteArray data;
//    Head head(*comm);

//    int currentIndex = ui->comboBoxServoList->currentIndex();
//    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
    
//    if (!comm->isReady()){
//        error_message->showMessage("Serial port is not open!");
//        return;
//    }
    
//    ServoPos[ServoId]-=10;

//    if (ServoPos[ServoId] > 100){
//        ServoPos[ServoId] = 100;
//        return;
//    }
    
//    if (ServoId == 1)
//       head.move_h(ServoPos[ServoId]);
//    else
//       head.move_v(ServoPos[ServoId]);
    
//    //ui->dial->setValue(int(ServoPos[ServoId]));
}

void RemoteControlWindow::fillServoParameters()
{
    ui->comboBoxServoList->addItem(QStringLiteral("Servo 0 (V)"), 0x00);
    ui->comboBoxServoList->addItem(QStringLiteral("Servo 1 (H)"), 0x01);
    ui->comboBoxServoList->addItem(QStringLiteral("Servo 2"), 0x02);
    ui->comboBoxServoList->addItem(QStringLiteral("Servo 3"), 0x03);
    
}


void RemoteControlWindow::on_dialChanged(){
    
//    QByteArray data;
//    QByteArray package;
//    Head head(*comm);
        
//    int currentIndex = ui->comboBoxServoList->currentIndex();
//    int ServoId = ui->comboBoxServoList->itemData(currentIndex).toInt();
        
//    if (!comm->isReady()){
//        error_message->showMessage("Serial port is not open!");
//        return;
//    }
    
//    ServoPos[ServoId] = (uint8_t)ui->dial->value();
    
//    /* Package head data */
//    data += 0x01;
//    data += (uint8_t)ServoId;
//    data += ServoPos[ServoId];
    
//    package = comm->make_pgk(data);
            
//    /* Send data */
//    comm->send_data(package, 6);
            
}


void RemoteControlWindow::update_servo_current(){
    QByteArray data;
    QByteArray package;
    QByteArray response;

    if (!comm->isReady()){
        error_message->showMessage("Serial port is not open!");
        return;
    }

    /* Package data */
    data += 0x11;
    data += (uint8_t)0x1;

    package = comm->make_pgk(data);

#ifdef DEBUG
    for (int i=0; i < package.size(); i++)
         qDebug() <<  "Datum:" << hex << (int)package[i];

    qDebug() << "--------------";
#endif

    /* Send data */
    response = comm->send_rcv_data(package,7);

#ifdef DEBUG
    for (int i=0; i < response.size(); i++)
         qDebug() <<  "Resp:" << hex << (int)response[i];

    qDebug() << "--------------";
#endif

    unsigned int adcdata = (uint8_t) response[4] << 8 | (uint8_t)response[5];

    if (response.size() <  4)
        for (int i=0; i < response.size(); i++)
             qDebug() <<  "Resp:" << hex << (int)response[i];


#ifdef DEBUG
    qDebug() << "Data:" << adcdata;
    printf("Data: %x\n", adcdata);
#endif

    ui->lcdNumberA0->display((double)adcdata);
}

void RemoteControlWindow::on_pushButtonYes_clicked(){
//    /* Current on/off state */
//    static bool on = false;
//    /* Robot head */
//    Head head(*comm);

//    if (!comm->isReady()){
//        error_message->showMessage("Serial port is not open!");
//        return;
//    }

//    if (on == false){
//        timer_yes->setInterval(1000);
//        timer_yes->start();
//        on = true;
//        ui->pushButtonYes->setChecked(true);
//    }else {
//        timer_yes->stop();
//        head.move_v(50);
//        ui->pushButtonYes->setChecked(false);
//        on = false;
//    }
}
 

void RemoteControlWindow::on_pushButtonLed_clicked(){
//    /* Current on/off state */
//    static uint8_t led = 0;
//   /* Robot head */
//    Head head(*comm);

//    if (!comm->isReady()){
//        error_message->showMessage("Serial port is not open!");
//        return;
//    }

//    if (led == 0){
//        led = 1;
//        head.led_on();
//    }
//    else{
//        led = 0;
//        head.led_off();
//    }
}


void RemoteControlWindow::on_pushButtonNo_clicked(){
//    /* Current on/off state */
//    static bool on = false;
//    /* Robot head */
//    Head head(*comm);

//    if (!comm->isReady()){
//        error_message->showMessage("Serial port is not open!");
//        return;
//    }

//    if (on == false){
//        timer_no->setInterval(3000);
//        timer_no->start();
//        on = true;
//        ui->pushButtonNo->setChecked(true);
//    }else {
//        timer_no->stop();
//        head.move_h(50);
//        ui->pushButtonNo->setChecked(false);
//        on = false;
//    }

}

void RemoteControlWindow::onTimerYesTimeout(){
//    /* Robot head */
//    Head head(*comm);
//    /* Current side */
//    static bool up = false;

//    if (!comm->isReady()){
//        error_message->showMessage("Serial port is not open!");
//        return;
//    }

//    if (up == false){
//        head.move_v(100);
//        up = true;
//    }
//    else {
//        head.move_v(0);
//        up = false;
//    }
}

void RemoteControlWindow::onTimerNoTimeout(){
//    /* Robot head */
//    Head head(*comm);
//    /* Current side */
//    static bool left = false;

//    if (left == false){
//        head.move_h(75);
//        left = true;
//    }
//    else {
//        head.move_h(25);
//        left = false;
//    }
}

void RemoteControlWindow::on_pushButtonArmLeft_clicked(){
//    /* Current on/off state */
//    static bool on = false;

//    if (!comm->isReady()){
//        error_message->showMessage("Serial port is not open!");
//        return;
//    }

//    if (on == false){
//        timer_leftArm->setInterval(5000);
//        timer_leftArm->start();
//        on = true;
//        ui->pushButtonArmLeft->setChecked(true);
//    }else {
//        timer_leftArm->stop();
//        //head.move_v(50);
//        ui->pushButtonArmLeft->setChecked(false);
//        on = false;
//    }
}


void RemoteControlWindow::onTimerLeftArmTimeout(){
//    static bool left = false;
//    QByteArray data;
//    QByteArray package;
//    uint8_t move = 0;

//    /* Package head data */
//    data += 0x01;
//    data += (uint8_t)02;

//    if (left == false){
//        move = 0x10;
//        left = true;
//    }
//    else {
//        move = 0x64;
//        left = false;
//    }

//    data += move;
//    package = comm->make_pgk(data);

//    /* Send data */
//    comm->send_data(package, 6);

}

void RemoteControlWindow::on_pushButtonArmRight_clicked(){
//    /* Current on/off state */
//    static bool on = false;

//    if (!comm->isReady()){
//        error_message->showMessage("Serial port is not open!");
//        return;
//    }

//    if (on == false){
//        timer_rigtArm->setInterval(5000);
//        timer_rigtArm->start();
//        on = true;
//        ui->pushButtonArmRight->setChecked(true);
//    }else {
//        timer_rigtArm->stop();
//        //head.move_v(50);
//        ui->pushButtonArmRight->setChecked(false);
//        on = false;
//    }


}


void RemoteControlWindow::onTimerRightArmTimeout(){
//    static bool left = false;
//    QByteArray data;
//    QByteArray package;
//    uint8_t move = 0;

//    /* Package head data */
//    data += 0x01;
//    data += (uint8_t)03;

//    if (left == false){
//        move = 0x10;
//        left = true;
//    }
//    else {
//        move = 0x64;
//        left = false;
//    }

//    data += move;
//    package = comm->make_pgk(data);

//    /* Send data */
//    comm->send_data(package, 6);
}
