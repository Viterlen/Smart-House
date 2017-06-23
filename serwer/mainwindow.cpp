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
    connect(timer, SIGNAL(timeout()), this, SLOT(messageLoop()));

    timer->start(100);
    fillPorts();
    fillIds();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_b1_clicked()
{
    if(ui->c1->currentText().size() == 0)
        return;
    openPort(ui->c1->currentText());
    ui->l1->setText("OPEN");

}

void MainWindow::on_pushButton_clicked()
{
    if(ui->comboBox_2->currentText().size() == 0)
        return;
    addSlave(ui->comboBox_2->currentText().toInt());
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    if(widok == slaves.end())
        return;
    qDebug() << arg1;
    widok->setMin(Slave::Temp, arg1);
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    if(widok == slaves.end())
        return;
    widok->setMax(Slave::Temp, arg1);
}

void MainWindow::on_spinBox_3_valueChanged(int arg1)
{
    if(widok == slaves.end())
        return;
    widok->setMin(Slave::Jasn, arg1);
}

void MainWindow::on_spinBox_4_valueChanged(int arg1)
{
    if(widok == slaves.end())
        return;
    widok->setMax(Slave::Jasn, arg1);
}

void MainWindow::on_spinBox_5_valueChanged(int arg1)
{
    if(widok == slaves.end())
        return;
    widok->setMin(Slave::Wilg, arg1);
}

void MainWindow::on_spinBox_6_valueChanged(int arg1)
{
    if(widok == slaves.end())
        return;
    widok->setMax(Slave::Wilg, arg1);
}


void MainWindow::fillPorts()
{
    for(const auto& x : QSerialPortInfo::availablePorts())
    {
        if(x.isBusy())
            continue;
        ui->c1->addItem(x.portName());
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

void MainWindow::sendMessage(quint8 __1, quint8 __2)
{
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream << __1 << __2;
    serial->write(byteArray);
    serial->flush();
}

void MainWindow::sendDataRequest(int)
{
    sendMessage(widok->getId(), obslugiwanyCzujnik);
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
        if(values[0] != 0)
        {
            return;
        }

        obsluga->setVal(obslugiwanyCzujnik, values[Second]);
        if(obsluga == widok)
        {
            switch(obslugiwanyCzujnik)
            {
            case Slave::Temp:
                ui->verticalSlider->setValue(widok->getVal(Slave::Temp));
                break;
            case Slave::Jasn:
                ui->verticalSlider_2->setValue(widok->getVal(Slave::Jasn));
                break;
            case Slave::Wilg:
                ui->verticalSlider_3->setValue(widok->getVal(Slave::Wilg));
                break;
            }
        }

    }
}
void MainWindow::messageLoop()
{
    qDebug() << "Timer timeout!";
    if(obsluga == slaves.end())
        return;
    if(krok == Zapytanie)
    {
        sendMessage(obsluga->getId(), obslugiwanyCzujnik);
        krok = Regulacja;
    }
    else
    {
        enum
        {
            UP=1,
            Down=2
        };
        //4-6*(1+KIERUNEK)
        if(obsluga->getVal(obslugiwanyCzujnik) < obsluga->getMin(obslugiwanyCzujnik))
            sendMessage(obsluga->getId(), (obslugiwanyCzujnik+3)*UP);
        else if(obsluga->getVal(obslugiwanyCzujnik) > obsluga->getMax(obslugiwanyCzujnik))
            sendMessage(obsluga->getId(), (obslugiwanyCzujnik+3)*Down);

        if(++obslugiwanyCzujnik > Slave::Wilg)
        {
            obslugiwanyCzujnik = Slave::Temp;
            if(++obsluga == slaves.end())
                obsluga = slaves.begin();
        }
        krok = Zapytanie;
    }
}

void MainWindow::addSlave(int id)
{
    slaves.push_back(Slave(id));
    if(slaves.size() == 1)
    {
        widok = slaves.begin();
        obsluga = slaves.begin();
    }
    ui->comboBox->addItem(QString::number(id));
    ui->comboBox_2->removeItem(ui->comboBox_2->currentIndex());
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    int id = arg1.toInt();
    widok = std::find_if(slaves.begin(), slaves.end(), [id](auto x) {if(x.getId() == id)
            return true;
        return false;});
    if(widok == slaves.end())
        return;

    ui->verticalSlider->setValue(widok->getVal(Slave::Temp));
    ui->verticalSlider_2->setValue(widok->getVal(Slave::Jasn));
    ui->verticalSlider_3->setValue(widok->getVal(Slave::Wilg));

    qDebug() << widok->getMin(Slave::Temp);
    ui->spinBox->setValue(widok->getMin(Slave::Temp));
    ui->spinBox_2->setValue(widok->getMax(Slave::Temp));

    ui->spinBox_3->setValue(widok->getMin(Slave::Jasn));
    ui->spinBox_4->setValue(widok->getMax(Slave::Jasn));

    ui->spinBox_5->setValue(widok->getMin(Slave::Wilg));
    ui->spinBox_6->setValue(widok->getMax(Slave::Wilg));
}
