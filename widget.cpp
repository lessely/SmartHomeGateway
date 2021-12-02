#include "widget.h"
#include "ui_widget.h"
#include "QDebug"
#include "QString"
#include "QSerialPortInfo"
#include "QtCore"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    pSerialPort=new QSerialPort(this);
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));//时间
    timer->start(1000);

    pSerialPort->setPortName("ttyUSB0");
    pSerialPort->setBaudRate(QSerialPort::Baud115200);
    pSerialPort->setDataBits(QSerialPort::Data8);
    pSerialPort->setStopBits(QSerialPort::OneStop);
    pSerialPort->setParity(QSerialPort::NoParity);
    pSerialPort->setFlowControl(QSerialPort::NoFlowControl);
    if(pSerialPort->open(QSerialPort::ReadWrite))
    {
        qDebug()<<"open serial successfully";
        connect(pSerialPort,&QSerialPort::readyRead,this,&Widget::ReceivSerial);
        ui->ComLable->setText("连接成功");
    }else{
        qDebug()<<"open serial failed";
        ui->ComLable->setText("连接失败");
    }

    pMqttCLient=new QMqttClient(this);
    pMqttCLient->setHostname("lessely.qicp.vip");
    pMqttCLient->setPort(1883);
    pMqttCLient->setClientId("qt");
    pMqttCLient->setUsername("1");
    pMqttCLient->setPassword("1");
    pMqttCLient->connectToHost();

    if(connect(pMqttCLient,&QMqttClient::connected,this,&Widget::connected_isr))
    {
        ui->NetworkLable->setText("连接成功");
        connect(pMqttCLient,&QMqttClient::messageReceived,this,&Widget::recv_message);
    }
    else{
        ui->NetworkLable->setText("连接失败");
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_testButton_clicked()
{
    QList<QSerialPortInfo> serialInfo=QSerialPortInfo::availablePorts();
    ui->ModeLable->setText("test");
    for(int i=0;i<serialInfo.count();i++)
    {
        qDebug()<<serialInfo[i].portName();
        ui->ComLable->setText(serialInfo[i].portName());
        if(serialInfo[i].portName()==QString::fromLocal8Bit("ttyUSB0"))
        {
            pSerialPort->open(QSerialPort::ReadWrite);
            qDebug()<<"open serial successfully";
            connect(pSerialPort,&QSerialPort::readyRead,this,&Widget::ReceivSerial);
            ui->ComLable->setText("连接成功");
        }
    }
}


void Widget::JsonToStr(QByteArray data)
{
    int Mode;
    int Tem,Hum,Pm;
    QJsonParseError jsonError;
    QJsonDocument root_doc=QJsonDocument::fromJson(data,&jsonError);
    if((!root_doc.isNull())&&(jsonError.error==QJsonParseError::NoError))
    {
        QJsonObject root_obj=root_doc.object();
        Tem=root_obj.value("Tem").toInt();
        Hum=root_obj.value("Hum").toInt();
        Pm=root_obj.value("PM").toInt();
        Mode = root_obj.value("Home").toInt();

        ui->TemLable->setText(QString::number(Tem));
        ui->HumLable->setText(QString::number(Hum));
        ui->PMLable->setText(QString::number(Pm));

          if(Mode==1)
            {
                ui->ModeLable->setText("回家模式");
            }
            else
            {
                ui->ModeLable->setText("外出模式");
            }

}
}
void Widget::ReceivSerial()
{
    QThread::msleep(260);
    QString buff=pSerialPort->readAll();
    qDebug()<<buff;
    pMqttCLient->publish(QMqttTopicName("data"),buff.toLocal8Bit());
    JsonToStr(buff.toLocal8Bit());
}


void Widget::recv_message(const QByteArray &message, const QMqttTopicName &topic)//mqtt接收到消息进行处理
{
    QString serialStr = "mqtt receive"+topic.name()+message;
    qDebug()<<serialStr;
    if(topic.name()==QString::fromLocal8Bit("control"))
    {
        if(message==QString::fromLocal8Bit("1111"))
        {
            qDebug("bibi on");
            on_BibiButton_clicked();
        }
        if(message==QString::fromLocal8Bit("1100"))
        {
            on_BibiButtonoff_clicked();
        }
        if(message==QString::fromLocal8Bit("2222"))
        {
            on_HomeButton_clicked();
        }    if(message==QString::fromLocal8Bit("2200"))
        {
            on_OutButton_clicked();
        }
   }
}

void Widget::timerUpdate()
{
    QDateTime time=QDateTime::currentDateTime();
    QString str = time.toString("hh:mm:ss");
    ui->TimeLable->setText(str);
}

void Widget::connected_isr()
{
    pMqttCLient->subscribe(QMqttTopicFilter("control"),0);
    qDebug()<<"connected successfully!";
}

void Widget::on_HomeButton_clicked()
{
    char home[]="2222";
    pSerialPort->write(home);
}

void Widget::on_OutButton_clicked()
{
    char out[]="2200";
    pSerialPort->write(out);
}

void Widget::on_BibiButton_clicked()
{
    char Bibion[]="1111";
    pSerialPort->write(Bibion);
}

void Widget::on_BibiButtonoff_clicked()
{
    char Bibioff[]="1100";
    pSerialPort->write(Bibioff);
}

void Widget::WriteSerial()
{
    char buf[]="hello world";
    pSerialPort->write(buf);
}

