#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLinkedList>
class QSerialPort;
class QTimer;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_b1_clicked();

    void on_pushButton_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

    void on_spinBox_3_valueChanged(int arg1);

    void on_spinBox_4_valueChanged(int arg1);

    void on_spinBox_5_valueChanged(int arg1);

    void on_spinBox_6_valueChanged(int arg1);

    void fillPorts();

    void openPort(QString text);

    void fillIds();

    void readMessage();

    void messageLoop();

    void sendMessage(quint8 __1, quint8 __2);

    void sendDataRequest(int);

    void addSlave(int);

    void on_comboBox_currentTextChanged(const QString &arg1);


private:
    Ui::MainWindow *ui;
    QSerialPort* serial;
    QTimer* timer;

    class Slave
    {
    public:
        enum
        {
            Temp = 0,
            Jasn,
            Wilg
        };

        Slave(int Id) : id(Id) {}
        int getId() const {return id;}
        void setMin(int id, int x) {vals[id].setMin(x);}
        void setVal(int id, int x) {vals[id].setVal(x);}
        void setMax(int id, int x) {vals[id].setMax(x);}
        int getMin(int id) const {return vals[id].getMin();}
        int getVal(int id) const {return vals[id].getVal();}
        int getMax(int id) const {return vals[id].getMax();}
    private:
        int id;
        class Bar
        {
        public:
            void setMin(int x) {min = x;}
            void setVal(int x) {val = x;}
            void setMax(int x) {max = x;}
            int getMin() const {return min;}
            int getVal() const {return val;}
            int getMax() const {return max;}
        private:
            int min =0;
            int val =0;
            int max =255;
        }vals[3];
    };

    enum
    {
        Zapytanie = 0,
        Regulacja
    };

    QLinkedList<Slave> slaves;
    QLinkedList<Slave>::iterator widok = slaves.begin();
    QLinkedList<Slave>::iterator obsluga = slaves.begin();
    int obslugiwanyCzujnik;
    int krok = Zapytanie;

};

#endif // MAINWINDOW_H
