#ifndef MYTOOLS_H
#define MYTOOLS_H
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QSysInfo>

class LabelUtils {
public:
    static void setLabelAsRealTimeClock(QLabel* label) {
        if (!label) return; // 确保指针有效

        // 创建定时器（堆内存，避免静态函数结束时被销毁）
        QTimer* timer = new QTimer(label); // 绑定到label，随label销毁自动释放
        QObject::connect(timer, &QTimer::timeout, [label]() {
            // 获取当前时间并格式化
            QString timeStr = QDateTime::currentDateTime().toString("MM-dd HH:mm:ss");
            label->setText(timeStr);
        });

        timer->start(1000); // 每秒更新一次
    }
    static void setLabelDeviceName(QLabel* label) {
        // 获取操作系统类型、版本和 CPU 架构
        QString osType = QSysInfo::productType();   // 操作系统类型，例如 "windows"
        QString osVersion = QSysInfo::productVersion(); // 操作系统版本，例如 "11"
        QString osArch = QSysInfo::currentCpuArchitecture(); // CPU 架构，例如 "x86_64"

        // 设置 QLabel 的文本
        QString deviceInfo = QString("%1 %2_%3")
                                 .arg(osType)
                                 .arg(osVersion)
                                 .arg(osArch);
        label->setText(deviceInfo);
    }

};
class MQTTJsonHelper
{
public:
    template <typename T>
    static QString constructThresholdJson(const QString &key, T value)
    {
        QJsonObject root;
        QJsonObject params;

        params.insert(key, QJsonValue::fromVariant(value));
        root.insert("params", params);

        return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
    }
};
#endif // MYTOOLS_H
