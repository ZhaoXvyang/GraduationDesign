#include "widget.h"
#include "ui_widget.h"
#include "drawdata.h"
#include <QLabel>
#include <QTimer>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setFixedSize(1024, 768);
    setWindowTitle("GraduationDesign");

    DrawData* dd = new DrawData(ui->widget);
    client = new MQTTClient;

}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_connect_clicked()
{
    qDebug()<<"on_pushButton_connect_clicked";
    if(client!=nullptr&&(client->m_client!=nullptr)){
        client->m_client->connectToHost();
        qDebug()<<"连接成功";
    }
}


void Widget::on_pushButton_subscribe_clicked()
{
    qDebug()<<"on_pushButton_subscribe_clicked";
    if(client!=nullptr&&(client->m_client!=nullptr)){
        client->subscribe();
        qDebug()<<"订阅成功";
    }
}


void Widget::on_pushButton_publish_clicked()
{
    qDebug()<<"on_pushButton_publish_clicked";
    if(client!=nullptr&&(client->m_client!=nullptr)){
        client->publish();
        qDebug()<<"发布成功";
    }
}

