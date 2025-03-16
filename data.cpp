#include "data.h"
#include "QDateTime"


namespace data{

data::Data::Data(QObject* parent)
    :QObject(parent)
{
    m_temp = 0;
    m_humi = 0;
    m_led_connected = 0;  // 注意，这里应该是 bool 类型，建议初始化为 false
    m_airque = 0;
    m_version = "";  // 使用空字符串初始化版本
    m_density = 0.0;
    m_airpress = 0;
    m_tempThreshold = 0;
    m_humiThreshold = 0;
    m_pm25Threshold = 0;
    m_airQThreshold = 0;
    m_pressThreshold = 0;
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

    // 解析 JSON 对象的参数
    auto parseParams = [&](const QJsonObject& params) {
        if (params.contains("temp") && params["temp"].isDouble()) {
            m_temp = params["temp"].toDouble();
        } else {
            m_temp = 0.0;  // 设置默认值
        }

        if (params.contains("humi") && params["humi"].isDouble()) {
            m_humi = static_cast<int>(params["humi"].toDouble());
        } else {
            m_humi = 0;  // 设置默认值
        }

        if (params.contains("airque") && params["airque"].isDouble()) {
            m_airque = params["airque"].toDouble();
        } else {
            m_airque = 0.0;  // 设置默认值
        }

        if (params.contains("PM") && params["PM"].isDouble()) {
            m_density = params["PM"].toDouble();
        } else {
            m_density = 0.0;  // 设置默认值
        }

        if (params.contains("airpress") && params["airpress"].isDouble()) {
            m_airpress = params["airpress"].toDouble();
        } else {
            m_airpress = 0.0;  // 设置默认值
        }

        if (params.contains("led_connected") && params["led_connected"].isBool()) {
            m_led_connected = params["led_connected"].toBool();
        } else {
            m_led_connected = false;  // 设置默认值
        }
    };

    // 第一种格式的解析
    if (obj.contains("params") && obj["params"].isObject()) {
        QJsonObject params = obj["params"].toObject();
        parseParams(params);

        if (obj.contains("version") && obj["version"].isString()) {
            m_version = obj["version"].toString();
        } else {
            m_version = "";  // 设置默认值
        }
    }
    // 第二种格式的解析
    else if (obj.contains("items") && obj["items"].isObject()) {
        QJsonObject items = obj["items"].toObject();

        if (items.contains("temp") && items["temp"].isObject()) {
            m_temp = items["temp"].toObject()["value"].toDouble();
        } else {
            m_temp = 0.0;  // 设置默认值
        }

        if (items.contains("humi") && items["humi"].isObject()) {
            m_humi = static_cast<int>(items["humi"].toObject()["value"].toDouble());
        } else {
            m_humi = 0;  // 设置默认值
        }

        if (items.contains("airque") && items["airque"].isObject()) {
            m_airque = static_cast<int>(items["airque"].toObject()["value"].toDouble());
        } else {
            m_airque = 0.0;  // 设置默认值
        }

        if (items.contains("PM") && items["PM"].isObject()) {
            m_density = items["PM"].toObject()["value"].toDouble();
        } else {
            m_density = 0.0;  // 设置默认值
        }

        if (items.contains("airpress") && items["airpress"].isObject()) {
            m_airpress = static_cast<int>(items["airpress"].toObject()["value"].toDouble());
        } else {
            m_airpress = 0;  // 设置默认值
        }

        // 解析阈值数据
        if (items.contains("tempThreshold") && items["tempThreshold"].isObject()) {
            m_tempThreshold = items["tempThreshold"].toObject()["value"].toDouble();
        } else {
            m_tempThreshold = 0.0;  // 设置默认值
        }

        if (items.contains("humiThreshold") && items["humiThreshold"].isObject()) {
            m_humiThreshold = static_cast<int>(items["humiThreshold"].toObject()["value"].toDouble());
        } else {
            m_humiThreshold = 0;  // 设置默认值
        }

        if (items.contains("airQThreshold") && items["airQThreshold"].isObject()) {
            m_airQThreshold = static_cast<int>(items["airQThreshold"].toObject()["value"].toDouble());
        } else {
            m_airQThreshold = 0;  // 设置默认值
        }

        if (items.contains("pressThreshold") && items["pressThreshold"].isObject()) {
            m_pressThreshold = static_cast<int>(items["pressThreshold"].toObject()["value"].toDouble());
        } else {
            m_pressThreshold = 0;  // 设置默认值
        }

        if (items.contains("pm25Threshold") && items["pm25Threshold"].isObject()) {
            m_pm25Threshold = items["pm25Threshold"].toObject()["value"].toDouble();
        } else {
            m_pm25Threshold = 0.0;  // 设置默认值
        }

        // 解析根数据
        if (obj.contains("deviceType") && obj["deviceType"].isString()) {
            m_deviceType = obj["deviceType"].toString();
        } else {
            m_deviceType = "";  // 设置默认值
        }

        if (obj.contains("iotId") && obj["iotId"].isString()) {
            m_iotId = obj["iotId"].toString();
        } else {
            m_iotId = "";  // 设置默认值
        }

        if (obj.contains("requestId") && obj["requestId"].isString()) {
            m_requestId = obj["requestId"].toString();
        } else {
            m_requestId = "";  // 设置默认值
        }

        if (obj.contains("productKey") && obj["productKey"].isString()) {
            m_productKey = obj["productKey"].toString();
        } else {
            m_productKey = "";  // 设置默认值
        }

        if (obj.contains("gmtCreate") && obj["gmtCreate"].isDouble()) {
            m_gmtCreate = static_cast<qint64>(obj["gmtCreate"].toDouble());
        } else {
            m_gmtCreate = 0;  // 设置默认值
        }

        if (obj.contains("deviceName") && obj["deviceName"].isString()) {
            m_deviceName = obj["deviceName"].toString();
        } else {
            m_deviceName = "";  // 设置默认值
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
    qDebug() << "airpress:" << airpress();
    qDebug() << "PM2.5:" << density();
    qDebug() << "tempThreshold:" << tempThreshold();       // 打印温度阈值
    qDebug() << "humiThreshold:" << humiThreshold();       // 打印湿度阈值
    qDebug() << "airQThreshold:" << airQThreshold();       // 打印空气质量阈值
    qDebug() << "pressThreshold:" << pressThreshold();     // 打印气压阈值
    qDebug() << "pm25Threshold:" << pm25Threshold();       // 打印 PM2.5 阈值
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
