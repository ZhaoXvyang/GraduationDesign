#include "data.h"
#include "QDateTime"


namespace data{

data::Data::Data(QObject* parent)
    :QObject(parent)
{
    m_temp = 0;
    m_humi = 0;
    m_led_connected = 0;
    m_airque = 0.1;
    m_version = 0;
}

Data::Data(QString& jsonString,QObject* parent)
    :QObject(parent)
{
    initDataFromQString(jsonString);
}

QString Data::toJsonString()
{
    // 构建 JSON 对象
    QJsonObject paramsObj;
    paramsObj["temp"] = 31;
    paramsObj["humi"] = 22;
    paramsObj["airque"] = 0.55;
    paramsObj["led_connected"] = 0;

    QJsonObject jsonObj;
    jsonObj["params"] = paramsObj;
    jsonObj["version"] = "1.0.0";

    jsDoc.setObject(jsonObj);


    return QString(jsDoc.toJson());
}

void Data::initDataFromQJsonDocument(const QJsonDocument& jsDoc)
{
    if (jsDoc.isNull() || !jsDoc.isObject()) {
        qWarning() << "Invalid or null JSON document!";
        return;
    }

    QJsonObject obj = jsDoc.object();

    // 第一种格式的解析
    if (obj.contains("params") && obj["params"].isObject()) {
        QJsonObject params = obj["params"].toObject();
        if (params.contains("temp") && params["temp"].isDouble()) {
            m_temp = params["temp"].toDouble();
        }
        if (params.contains("humi") && params["humi"].isDouble()) {
            m_humi = static_cast<int>(params["humi"].toDouble());
        }
        if (params.contains("airque") && params["airque"].isDouble()) {
            m_airque = params["airque"].toDouble();
        }
        if (obj.contains("version") && obj["version"].isString()) {
            m_version = obj["version"].toString();
        }
        if (params.contains("led_connected") && params["led_connected"].isBool()) {
            m_led_connected = params["led_connected"].toBool();
        }
    }
    // 第二种格式的解析
    else if (obj.contains("items") && obj["items"].isObject()) {
        QJsonObject items = obj["items"].toObject();
        if (items.contains("temp") && items["temp"].isObject()) {
            m_temp = items["temp"].toObject()["value"].toDouble();
        }
        if (items.contains("humi") && items["humi"].isObject()) {
            m_humi = static_cast<int>(items["humi"].toObject()["value"].toDouble());
        }
        if (items.contains("airque") && items["airque"].isObject()) {
            m_airque = items["airque"].toObject()["value"].toDouble();
        }
        // 解析根数据
        if (obj.contains("deviceType") && obj["deviceType"].isString()) {
            m_deviceType = obj["deviceType"].toString();
        }
        if (obj.contains("iotId") && obj["iotId"].isString()) {
            m_iotId = obj["iotId"].toString();
        }
        if (obj.contains("requestId") && obj["requestId"].isString()) {
            m_requestId = obj["requestId"].toString();
        }
        if (obj.contains("productKey") && obj["productKey"].isString()) {
            m_productKey = obj["productKey"].toString();
        }
        if (obj.contains("gmtCreate") && obj["gmtCreate"].isDouble()) {
            m_gmtCreate = static_cast<qint64>(obj["gmtCreate"].toDouble());
        }
        if (obj.contains("deviceName") && obj["deviceName"].isString()) {
            m_deviceName = obj["deviceName"].toString();
        }
    }
}



void Data::initDataFromQString(const QString &jsonQString)
{
    initDataFromQJsonDocument(QJsonDocument::fromJson(jsonQString.toUtf8()));
}

void Data::printData()
{
    // 打印温度、湿度和 LED 状态
    qDebug() << "temp:" << m_temp;
    qDebug() << "humi:" << m_humi;
    qDebug() << "led_connected:" << m_led_connected;
    qDebug() << "version:" << m_version;
    qDebug() << "airque:" << m_airque;
    // 后面的字段被注释掉了，如果需要打印的话可以取消注释
    if(m_deviceType.isEmpty()) return;
    qDebug() << "deviceType:" << m_deviceType;
    qDebug() << "iotId:" << m_iotId;
    qDebug() << "requestId:" << m_requestId;
    qDebug() << "productKey:" << m_productKey;
    qDebug() << "gmtCreate:" <<
        QDateTime::fromMSecsSinceEpoch(m_gmtCreate).toString("yyyy-MM-dd HH:mm:ss");
    qDebug() << "deviceName:" << m_deviceName;
}




}// end namespace data
