#ifndef MQTT_H
#define MQTT_H

#include <QObject>
#include <QString>
#include <QtMqtt/qmqttclient.h>
#include "data.h"

static const QString SERVER_ADDRESS = QStringLiteral("iot-06z00fmnyhez6wa.mqtt.iothub.aliyuncs.com");
static const QString CLIENT_ID = QStringLiteral("k282yy0B25v.ESP8266_Client|securemode=2,signmethod=hmacsha256,timestamp=1739806999760|");
static const QString TOPIC_POST = QStringLiteral("/sys/k282yy0B25v/ESP8266_Client/thing/event/property/post");
static const QString TOPIC_EGT = QStringLiteral("/sys/k282yy0B25v/ESP8266_Client/thing/service/property/set");
static const QString USER_NAME = QStringLiteral("ESP8266_Client&k282yy0B25v");
static const QString PASSWAD = QStringLiteral("07db379304aa99492e794e04566b70dfe70b93316275eee210edac1c944e72ec");

static const QString product_key = QStringLiteral("k282yy0B25v");
static const QString device_name = QStringLiteral("ESP8266_Client");
static const QString device_secret = QStringLiteral("aab84cc896d567fcc290af6b4b7d4158");


class MQTTClient : public QObject
{
    Q_OBJECT
public:
    explicit MQTTClient(QObject *parent = nullptr);
    ~MQTTClient() = default;

    QMqttClient *m_client = nullptr;

    void MyMQTTSubscribe(QString);
    void MyMQTTSendMessage(const QString, const QString);
    void connectHost();
    void subscribe();
    void publish();
    void disconnected();
    bool isConnected();

signals:
    void signal_new_data_recevied(double temp,double humi,qint64 timestamp);
    void signal_update_labels(data::Data label_Data);
    void signal_updateThresholdLabels(data::Data label_Data);

public slots:
    void slot_connected();
    void slot_recvMsg(const QByteArray& message,const QMqttTopicName& topic);

private:
    std::atomic<bool> m_is_connected;
};


#endif // MQTT_H
