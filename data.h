#ifndef DATA_H
#define DATA_H

#include <QObject>
#include <QVariant>
#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

namespace data{

class Data : public QObject
{
    Q_OBJECT
public:
    explicit Data(QObject* parent = nullptr);
    explicit Data(QString& jsonString,QObject* parent = nullptr);

public:
    QString toJsonString();
    void initDataFromQJsonDocument(const QJsonDocument& jsDoc);
    void initDataFromQString(const QString& jsonString);
    void printData();

    // Getter methods for each member variable
    double temp() const { return m_temp; }            // 获取温度
    double humi() const { return m_humi; }            // 获取湿度
    bool ledConnected() const { return m_led_connected; } // 获取LED连接状态
    QString version() const { return m_version; }      // 获取版本
    QString deviceType() const { return m_deviceType; } // 获取设备类型
    QString iotId() const { return m_iotId; }          // 获取IOT ID
    QString requestId() const { return m_requestId; }  // 获取请求ID
    QString productKey() const { return m_productKey; } // 获取产品Key
    qint64 gmtCreate() const { return m_gmtCreate; }   // 获取创建时间戳
    QString deviceName() const { return m_deviceName; } // 获取设备名称
    QJsonDocument jsonDocument() const { return jsDoc; } // 获取QJsonDocument对象

protected:

private:
    double m_temp;          // 温度
    double m_humi;          // 湿度
    bool m_led_connected;   // LED等控制开关
    QString m_version;      // 版本
    QString m_deviceType;   // 设备类型
    QString m_iotId;        // IOT ID
    QString m_requestId;    // 请求 ID
    QString m_productKey;   // 产品 Key
    qint64 m_gmtCreate;     // 创建时间戳
    QString m_deviceName;   // 设备名称
    QJsonDocument jsDoc;
};


} // end namespace data
#endif // DATA_H
