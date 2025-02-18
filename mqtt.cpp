#include "mqtt.h"
#include <QDebug>
#include <QMessageBox>
#include <QByteArray>

MQTTClient::MQTTClient(QObject *parent)
    :QObject(parent)
{
    m_client = new QMqttClient(this);
    m_client->setHostname(SERVER_ADDRESS);
    m_client->setPort(1883);
    m_client->setUsername(USER_NAME);
    m_client->setPassword(PASSWAD);
    m_client->setKeepAlive(60);
    m_client->setClientId(CLIENT_ID);

    connect(m_client,&QMqttClient::connected,this,MQTTClient::slot_connected);
    connect(m_client,&QMqttClient::disconnected,[this](){
        qDebug()<<"连接断开";
    });
}

void MQTTClient::subscribe()
{
    m_client->subscribe(TOPIC_EGT);
}

void MQTTClient::publish()
{
    QByteArray ba;
    QString txt = "{\"method\":\"thing.service.property.set\",\"id\":\"1473019453\",\"params\":{\"temp\":31,\"humi\":22,\"led_connected\":0},\"version\":\"1.0.0\"}";
    ba.append(txt.toUtf8());
    m_client->publish(TOPIC_POST, ba);
}

void MQTTClient::brokerConnected()
{

}

void MQTTClient::slot_connected()
{
    qDebug()<<"连接成功!";
    connect(m_client,&QMqttClient::messageReceived,this,&MQTTClient::slot_recvMsg);
}

void MQTTClient::slot_recvMsg(const QByteArray& message,const QMqttTopicName& topic)
{
    qDebug()<<"收到服务器推送消息:"<<topic.name()+QString(message);
}
