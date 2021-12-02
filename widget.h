#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtMqtt/qmqttclient.h>
#include <QTimer>
#include <QDateTime>
#include <QSerialPort>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    QMqttClient *pMqttCLient;
    QSerialPort *pSerialPort;
private slots:
    void connected_isr();
    void on_OutButton_clicked();
    void on_HomeButton_clicked();
    void on_BibiButton_clicked();
    void on_BibiButtonoff_clicked();
    void WriteSerial();
    void ReceivSerial();
    void on_testButton_clicked();
    void timerUpdate();
    void JsonToStr(QByteArray data);
    void recv_message(const QByteArray &message, const QMqttTopicName &topic);
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
