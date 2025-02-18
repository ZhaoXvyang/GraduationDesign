#include "widget.h"
#include "ui_widget.h"
#include "drawdata.h"
#include "MyTools.h"
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
    LabelUtils::setLabelAsRealTimeClock(ui->label_time);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_subscribe_clicked()
{
    qDebug()<<"on_pushButton_subscribe_clicked";
    if(client!=nullptr&&(client->m_client!=nullptr)){
        client->subscribe();
    }
}


void Widget::on_pushButton_publish_clicked()
{
    qDebug()<<"on_pushButton_publish_clicked";
    if(client!=nullptr&&(client->m_client!=nullptr)){
        client->publish();
    }
}

// 设置连接状态的函数
void Widget::setConnectionStatus(const QString &status)
{
    ui->label_connectedStatus->setText("连接状态：" + status);  // 设置连接状态文本
}

void Widget::on_pushButton_disconnect_clicked()
{
    if (client == nullptr || client->m_client == nullptr) {
        qDebug() << "客户端未初始化";
        return;
    }

    if (!client->isConnected()) {
        qDebug() << "已断开!!!"<<client->isConnected();
        return;
    }

    qDebug() << "on_pushButton_disconnect_clicked";
    client->disconnected();  // 断开连接
    setConnectionStatus("已断开");  // 更新连接状态
}

void Widget::on_pushButton_connect_clicked()
{
    if (client == nullptr || client->m_client == nullptr) {
        qDebug() << "客户端未初始化";
        return;
    }

    if (client->isConnected()) {
        qDebug() << "已连接,无需重复连接";
        return;
    }

    qDebug() << "on_pushButton_connect_clicked";
    client->connectHost();  // 连接服务器
    setConnectionStatus("已连接");  // 更新连接状态
}


