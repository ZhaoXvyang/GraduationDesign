#include "mqtt.h"
#include <QDebug>
#include <QMessageBox>
#include <QByteArray>
#include "data.h"

MQTTClient::MQTTClient(QObject *parent)
    :QObject(parent),m_is_connected(false)
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
        m_is_connected.store(false);
    });
}

void MQTTClient::connectHost()
{
    if(isConnected()) return;
    m_client->connectToHost();

}

void MQTTClient::subscribe()
{
    if(!isConnected()){qDebug()<<"未连接Mqtt服务器";return;}
    m_client->subscribe(TOPIC_EGT);
    qDebug()<<"订阅成功";
}

void MQTTClient::publish()
{
    if(!isConnected()){qDebug()<<"未连接Mqtt服务器";return;}

    QByteArray ba;
    data::Data data;
    QString txt = data.toJsonString();
    qDebug()<<txt;

    ba.append(txt.toUtf8());
    m_client->publish(TOPIC_POST, ba);
    qDebug()<<"发布成功";
}

void MQTTClient::disconnected()
{
    if (isConnected()) {
        m_client->disconnectFromHost();  // 断开连接
    } else {
        qDebug() << "客户端已经处于断开状态";
    }
}

bool MQTTClient::isConnected()
{
    return m_is_connected.load();
}

void MQTTClient::slot_connected()
{
    if (isConnected()) return;
    connect(m_client,&QMqttClient::messageReceived,this,&MQTTClient::slot_recvMsg);
    m_is_connected.store(true);
    qDebug()<<"连接成功!";
}

void MQTTClient::slot_recvMsg(const QByteArray& message,const QMqttTopicName& topic)
{
    if(!isConnected()){qDebug()<<"未连接Mqtt服务器";return;}
    qDebug()<<"收到服务器推送消息:"<<topic.name();
    QString msg = QString(message);
    qDebug()<<"message:"+msg;
    data::Data d1(msg,this);
    d1.printData();

    emit signal_new_data_recevied(d1.temp(),d1.humi(),d1.gmtCreate());
}
