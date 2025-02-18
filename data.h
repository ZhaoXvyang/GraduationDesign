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
