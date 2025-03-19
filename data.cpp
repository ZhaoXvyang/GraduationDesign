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

/**
 * @brief 将设备数据转换为 JSON 字符串。
 *
 * 此函数构建一个包含设备当前状态的 JSON 对象，并将其转换为字符串格式。
 * JSON 对象包含以下字段：
 * - "params": 包含设备的参数，如温度、湿度、空气质量和 LED 状态。
 * - "version": 设备数据的版本号。
 *
 * @return 返回包含设备数据的 JSON 字符串。
 *
 * @note 当前硬编码了设备的温度 (temp)、湿度 (humi)、空气质量 (airque)
 *       和 LED 连接状态 (led_connected)。如需动态数据，需根据实际数据进行修改。
 *
 * @warning 该函数未对参数进行有效性检查，若需要从其他来源获取参数值，
 *          请确保值的正确性。
 */
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

/**
 * @brief 从 QJsonDocument 初始化设备数据。
 *
 * 此函数接受一个 QJsonDocument 对象，解析其中的 JSON 数据并初始化设备的各项参数。
 * 函数首先检查传入的 JSON 文档是否有效，然后根据 JSON 文档的结构解析数据。
 *
 * 函数支持两种不同的 JSON 格式：
 * 1. 格式一：包含 "params" 字段，参数以键值对的形式存在。
 * 2. 格式二：包含 "items" 字段，参数以嵌套的对象形式存在。
 *
 * @param jsDoc 包含设备数据的 QJsonDocument 对象。
 *
 * @note 如果传入的 JSON 文档为 null 或不是对象，则会发出警告并返回。
 *       所有未解析的字段将被设置为默认值，以确保数据完整性。
 *
 * @warning 如果 JSON 文档不符合预期格式，可能会导致部分参数未正确初始化。
 */

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

/**
 * @brief 从 JSON 字符串初始化数据。
 *
 * 此函数接受一个包含 JSON 数据的 QString 对象，并将其转换为 QJsonDocument，
 * 然后调用相应的初始化函数来设置数据成员。
 *
 * @param jsonQString 包含 JSON 格式数据的字符串。该字符串应符合 JSON 格式的标准。
 *
 * @note 此函数主要用于从外部数据源（如文件或网络）读取 JSON 字符串并
 *       将其解析为数据对象。确保传入的字符串是有效的 JSON 格式，以避免
 *       解析错误。
 */
void Data::initDataFromQString(const QString &jsonQString)
{
    initDataFromQJsonDocument(QJsonDocument::fromJson(jsonQString.toUtf8()));
}

/**
 * @brief 打印设备的数据状态。
 *
 * 此函数用于输出设备的温度、湿度、LED 状态及其他相关信息到调试控制台。
 *
 * 打印的内容包括：
 * - 温度 (m_temp)
 * - 湿度 (m_humi)
 * - LED 连接状态 (m_led_connected)
 * - 版本 (m_version)
 * - 空气质量 (m_airque)
 * - 气压 (airpress())
 * - PM2.5 密度 (density())
 * - 温度阈值 (tempThreshold())
 * - 湿度阈值 (humiThreshold())
 * - 空气质量阈值 (airQThreshold())
 * - 气压阈值 (pressThreshold())
 * - PM2.5 阈值 (pm25Threshold())
 *
 * 如果设备类型 (m_deviceType) 不为空，还会打印以下信息：
 * - 设备类型 (m_deviceType)
 * - IoT ID (m_iotId)
 * - 请求 ID (m_requestId)
 * - 产品密钥 (m_productKey)
 * - 创建时间 (m_gmtCreate，以“yyyy-MM-dd HH:mm:ss”格式输出)
 * - 设备名称 (m_deviceName)
 *
 * 注意：某些字段的打印被注释掉，若需要打印，请取消相关注释。
 */
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
