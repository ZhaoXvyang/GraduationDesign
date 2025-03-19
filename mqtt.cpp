#include "mqtt.h"
#include <QDebug>
#include <QMessageBox>
#include <QByteArray>

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

/**
 * @brief 订阅指定的 MQTT 主题。
 *
 * 此函数用于订阅指定的 MQTT 主题，以接收来自该主题的消息。
 *
 * 在进行订阅之前，函数会检查与 MQTT 服务器的连接状态。
 * 如果未连接，将打印警告信息并返回。
 *
 * @note 函数将调用客户端的 subscribe 方法进行主题订阅，
 *       并在成功后打印订阅成功的消息。
 *
 * @warning 确保在调用此函数之前，MQTT 客户端已正确连接到服务器。
 */
void MQTTClient::subscribe()
{
    if(!isConnected()){qDebug()<<"未连接Mqtt服务器";return;}
    m_client->subscribe(TOPIC_EGT);
    qDebug()<<"订阅成功";
}

/**
 * @brief 发布消息到 MQTT 服务器。
 *
 * 此函数用于将数据发布到指定的 MQTT 主题。
 *
 * 在发布消息之前，函数会检查与 MQTT 服务器的连接状态。
 * 如果未连接，将打印警告信息并返回。
 *
 * @note 函数创建一个 Data 对象并将其转换为 JSON 字符串，
 *       然后将字符串转换为 QByteArray 格式以进行发布。
 *       发布成功后，将打印成功消息。
 *
 * @warning 确保在调用此函数之前，MQTT 客户端已正确连接到服务器。
 */
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

void MQTTClient::publishJson(const QString &jsonData)
{
    if (!isConnected()) {
        qDebug() << "未连接 Mqtt 服务器";
        return;
    }

    QByteArray ba = jsonData.toUtf8();
    m_client->publish(TOPIC_POST, ba);
    qDebug() << "发布成功: " << jsonData;
}


void MQTTClient::disconnected()
{
    if (isConnected()) {
        m_client->disconnectFromHost();  // 断开连接
    } else {
        qDebug() << "客户端已经处于断开状态";
    }
}

/**
 * @brief 连接到 MQTT 服务器。
 *
 * 此函数用于建立与 MQTT 服务器的连接。
 *
 * 在连接之前，函数会检查当前的连接状态。如果已连接，则直接返回，不会进行重复连接。
 *
 * @note 函数调用客户端的 connectToHost 方法以建立连接。
 *
 * @warning 确保在调用此函数之前，MQTT 客户端已正确初始化。
 */
bool MQTTClient::isConnected()
{
    return m_is_connected.load();
}

/**
 * @brief 处理成功连接到 MQTT 服务器的事件。
 *
 * 此槽函数在成功建立与 MQTT 服务器的连接时被调用。
 * 它将设置连接状态，并连接消息接收信号与相应的处理槽。
 *
 * @note 如果当前已连接到服务器，函数将直接返回，不会执行后续操作。
 *       在成功连接后，将通过信号连接实现消息接收的处理。
 *
 * @warning 确保在调用此函数之前已正确初始化 MQTT 客户端。
 */
void MQTTClient::slot_connected()
{
    if (isConnected()) return;
    connect(m_client,&QMqttClient::messageReceived,this,&MQTTClient::slot_recvMsg);
    m_is_connected.store(true);
    qDebug()<<"连接成功!";
}

/**
 * @brief 处理接收到的 MQTT 消息。
 *
 * 此槽函数用于接收并处理从 MQTT 服务器推送的消息。
 * 在处理消息之前，函数会检查与 MQTT 服务器的连接状态。
 *
 * @param message 接收到的消息内容，以 QByteArray 形式表示。
 * @param topic 消息的主题名称，使用 QMqttTopicName 表示。
 *
 * @note 函数首先打印接收到的主题和消息内容，然后解析消息生成 Data 对象。
 *       如果解析的阈值数据有效，则发出更新阈值标签的信号。
 *       如果接收到的实时数据有效，则发出更新标签和新数据接收的信号。
 *
 * @warning 如果未连接到 MQTT 服务器，函数将打印警告信息并返回，不会进行后续处理。
 */
void MQTTClient::slot_recvMsg(const QByteArray& message, const QMqttTopicName& topic) {
    if (!isConnected()) {
        qDebug() << "未连接Mqtt服务器";
        return;
    }

    //qDebug() << "收到服务器推送消息:" << topic.name();
    QString msg = QString(message);
    //qDebug() << "message:" << msg;

    data::Data d1(msg, this);
    //d1.printData();

    // 检查阈值数据的解析是否正确
    if (d1.tempThreshold() > 0 && d1.humiThreshold() > 0 && d1.airQThreshold() > 0 && d1.pm25Threshold() > 0) {
        // 接收到阈值数据
        emit signal_updateThresholdLabels(d1);
        //qDebug() << "更新阈值数据";
    } else if (d1.airpress() > 0 && d1.humi()) {
        // 接收到实时数据
        emit signal_update_labels(d1);
        emit signal_new_data_recevied(d1.temp(), d1.humi(), d1.gmtCreate());
        //qDebug() << "更新实时数据";
    }
}
