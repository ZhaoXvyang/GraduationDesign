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
    // 拷贝构造函数
    Data(const Data& other) : QObject(other.parent()),
        m_temp(other.m_temp),
        m_humi(other.m_humi),
        m_airque(other.m_airque),
        m_led_connected(other.m_led_connected),
        m_version(other.m_version),
        m_deviceType(other.m_deviceType),
        m_density(other.m_density),
        m_airpress(other.airpress()),
        m_tempThreshold(other.m_tempThreshold),
        m_humiThreshold(other.m_humiThreshold),
        m_airQThreshold(other.m_airQThreshold),
        m_pressThreshold(other.m_pressThreshold),
        m_pm25Threshold(other.m_pm25Threshold),
        m_iotId(other.m_iotId),
        m_requestId(other.m_requestId),
        m_productKey(other.m_productKey),
        m_gmtCreate(other.m_gmtCreate),
        m_deviceName(other.m_deviceName),
        jsDoc(other.jsDoc) {}

    // Getter methods for each member variable
    float temp() const { return m_temp; }            // 获取温度
    int humi() const { return m_humi; }            // 获取湿度
    bool ledConnected() const { return m_led_connected; } // 获取连接状态
    int airque() const {return m_airque;}             // 获取空气质量
    float density() const {return m_density; }    // PM2.5
    int airpress() const {return m_airpress; }    // 气压

    float tempThreshold() const { return m_tempThreshold; }    // 获取温度阈值
    int humiThreshold() const { return m_humiThreshold; }      // 获取湿度阈值
    int airQThreshold() const { return m_airQThreshold; }      // 获取空气质量阈值
    int pressThreshold() const { return m_pressThreshold; }     // 获取气压阈值
    float pm25Threshold() const { return m_pm25Threshold; }     // 获取 PM2.5 阈值


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
    float m_temp;           // 温度
    int   m_humi;           // 湿度
    int   m_airque;         // 空气质量
    float m_density;        // PM2.5
    int   m_airpress;       // 气压
    bool  m_led_connected;  // LED等控制开关

    float m_tempThreshold;    // 温度阈值
    int m_humiThreshold;      // 湿度阈值
    int m_airQThreshold;      // 空气质量阈值
    int m_pressThreshold;     // 气压阈值
    float m_pm25Threshold;    // PM2.5 阈值

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
