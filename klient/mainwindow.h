#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_3_valueChanged(int value);

    void fillPorts();

    void openPort(QString text);

    void fillIds();

    void readMessage();

    void sendMessage(quint8 __1, quint8 __2);
private:
    Ui::MainWindow *ui;
    QSerialPort* serial;
    QTimer* timer;
    int myId;
    class Slave
    {
    public:
        enum
        {
            Temp = 0,
            Jasn,
            Wilg
        };

        void setMin(int id, int x) {vals[id].setMin(x);}
        void setVal(int id, int x) {vals[id].setVal(x);}
        void setMax(int id, int x) {vals[id].setMax(x);}
        int getMin(int id) const {return vals[id].getMin();}
        int getVal(int id) const {return vals[id].getVal();}
        int getMax(int id) const {return vals[id].getMax();}
    private:
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
    }myValues;
};

#endif // MAINWINDOW_H
