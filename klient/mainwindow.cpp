#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <Qtimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    timer = new QTimer(this);

    connect(serial, SIGNAL(readyRead()), this, SLOT(readMessage()));

    fillPorts();
    fillIds();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(ui->comboBox->currentText().size() == 0)
        return;
    openPort(ui->comboBox->currentText());
}

void MainWindow::on_pushButton_2_clicked()
{
    if(ui->comboBox_2->currentText().size() == 0)
        return;
    myId = ui->comboBox_2->currentText().toInt();
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    myValues.setVal(Slave::Temp, value);
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    myValues.setVal(Slave::Jasn, value);
}

void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    myValues.setVal(Slave::Wilg, value);
}


void MainWindow::readMessage()
{
    if(!serial->isOpen())
    {
        return;
    }
    quint8 values[2];

    enum Fields
    {
        First = 0,
        Second,
    };

    qDebug() << "received message";
    if(serial->bytesAvailable()>=2)
    {
        QByteArray dataArray = serial->readAll();
        QDataStream stream(&dataArray, QIODevice::ReadOnly);

        for(int i=0; i<2; ++i)
            stream >> values[i];
        qDebug() << values[0];
        if(values[0] != myId)
        {
            return;
        }
        enum
        {
            UP=1,
            Down=2
        };
        if(values[Second] < 3)
        {
            sendMessage(0, myValues.getVal(values[Second]));
        }
        else if(values[Second] <= ((Slave::Wilg+3)*UP))
        {
            switch(values[Second])
            {
            case (Slave::Temp+3)*UP:
                myValues.setVal(Slave::Temp, myValues.getVal(Slave::Temp)+1);
                ui->horizontalSlider->setValue(myValues.getVal(Slave::Temp));
                break;
            case (Slave::Jasn+3)*UP:
                myValues.setVal(Slave::Jasn, myValues.getVal(Slave::Jasn)+1);
                ui->horizontalSlider_2->setValue(myValues.getVal(Slave::Jasn));
                break;
            case (Slave::Wilg+3)*UP:
                myValues.setVal(Slave::Wilg, myValues.getVal(Slave::Wilg)+1);
                ui->horizontalSlider_3->setValue(myValues.getVal(Slave::Wilg));
                break;
            }
        }
        else
        {
            switch(values[Second])
            {
            case (Slave::Temp+3)*Down:
                myValues.setVal(Slave::Temp, myValues.getVal(Slave::Temp)-1);
                ui->horizontalSlider->setValue(myValues.getVal(Slave::Temp));
                break;
            case (Slave::Jasn+3)*Down:
                myValues.setVal(Slave::Jasn, myValues.getVal(Slave::Jasn)-1);
                ui->horizontalSlider_2->setValue(myValues.getVal(Slave::Jasn));
                break;
            case (Slave::Wilg+3)*Down:
                myValues.setVal(Slave::Wilg, myValues.getVal(Slave::Wilg)-1);
                ui->horizontalSlider_3->setValue(myValues.getVal(Slave::Wilg));
                break;
            }
        }
    }
}

void MainWindow::sendMessage(quint8 __1, quint8 __2)
{
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream << __1 << __2;
    serial->write(byteArray);
    serial->flush();
}

void MainWindow::fillPorts()
{
    for(const auto& x : QSerialPortInfo::availablePorts())
    {
        if(x.isBusy())
            continue;
        ui->comboBox->addItem(x.portName());
    }
}

void MainWindow::openPort(QString text)
{
    serial->setPortName(text);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->open(QIODevice::ReadWrite);
}

void MainWindow::fillIds()
{
    for(int i=1; i<256; ++i)
        ui->comboBox_2->addItem(QString::number(i));
}
