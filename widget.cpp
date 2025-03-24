#include "widget.h"
#include "ui_widget.h"
#include "MyTools.h"
#include <QLabel>
#include <QTimer>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setFixedSize(1024, 768);
    setWindowTitle("GraduationDesign");

    drawData = new DrawData(ui->widget);
    client = new MQTTClient;
    LabelUtils::setLabelAsRealTimeClock(ui->label_time); // 显示当前系统时间
    LabelUtils::setLabelDeviceName(ui->label_osName);
    ui->label_connectedStatus->setStyleSheet("color: red;");

    // 检测连接状态
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setInterval(3000); // 3秒检测一次
    connect(m_timeoutTimer, &QTimer::timeout, this, &Widget::checkConnectionStatus);
    m_timeoutTimer->start(); // 启动定时器

}

Widget::~Widget()
{
    delete ui;
}

void Widget::checkConnectionStatus()
{
    setConnectionStatus("单片机断开 ❌");
}

// 设置连接状态的函数
void Widget::setConnectionStatus(const QString &status)
{
    if (status == "已连接") {
        ui->label_connectedStatus->setStyleSheet("color: green;");
    } else {
        ui->label_connectedStatus->setStyleSheet("color: red;");
    }
    ui->label_connectedStatus->setText("连接状态：" + status);  // 设置连接状态文本
}

// 更新温度状态文本
void Widget::updateTempStatus(float temp)
{
    QString tempStatus;
    if (temp < 0) {
        tempStatus = "寒冷 ❄️";
        ui->label_temp_status->setStyleSheet("color: blue;"); // 设置样式为蓝色
    } else if (temp < 10) {
        tempStatus = "较冷 🌬️";
        ui->label_temp_status->setStyleSheet("color: lightblue;"); // 设置样式为浅蓝色
    } else if (temp < 25) {
        tempStatus = "舒适 🙂";
        ui->label_temp_status->setStyleSheet("color: green;"); // 设置样式为绿色
    } else if (temp < 35) {
        tempStatus = "炎热 🔥";
        ui->label_temp_status->setStyleSheet("color: orange;"); // 设置样式为橙色
    } else {
        tempStatus = "酷热 🥵";
        ui->label_temp_status->setStyleSheet("color: red;"); // 设置样式为红色
    }
    ui->label_temp_status->setText(tempStatus);
}

// 更新湿度状态文本
void Widget::updateHumidityStatus(int humidity)
{
    QString humidityStatus;
    if (humidity < 30) {
        humidityStatus = "干燥 💨";
        ui->label_humi_status->setStyleSheet("color: orange;"); // 设置样式为橙色
    } else if (humidity < 60) {
        humidityStatus = "适中 ✅";
        ui->label_humi_status->setStyleSheet("color: green;"); // 设置样式为绿色
    } else {
        humidityStatus = "潮湿 💦";
        ui->label_humi_status->setStyleSheet("color: blue;"); // 设置样式为蓝色
    }
    ui->label_humi_status->setText(humidityStatus);
}

// 更新空气质量状态文本
void Widget::updateAirQualityStatus(int airQuality)
{
    QString airQualityStatus;
    if (airQuality < 50) {
        airQualityStatus = "优 ✅";
        ui->label_air_status->setStyleSheet("color: green;"); // 设置样式为绿色
    } else if (airQuality < 100) {
        airQualityStatus = "良 🙂";
        ui->label_air_status->setStyleSheet("color: lightgreen;"); // 设置样式为浅绿色
    } else if (airQuality < 150) {
        airQualityStatus = "轻度污染 😷";
        ui->label_air_status->setStyleSheet("color: orange;"); // 设置样式为橙色
    } else if (airQuality < 200) {
        airQualityStatus = "中度污染 😨";
        ui->label_air_status->setStyleSheet("color: red;"); // 设置样式为红色
    } else {
        airQualityStatus = "重度污染 ☠️";
        ui->label_air_status->setStyleSheet("color: darkred;"); // 设置样式为深红色
    }
    ui->label_air_status->setText(airQualityStatus);
}

// 更新气压状态文本
void Widget::updatePressureStatus(float pressure)
{
    QString pressureStatus;
    if (pressure < 980) {
        pressureStatus = "低气压 ⬇️";
        ui->label_airpress_status->setStyleSheet("color: orange;"); // 设置样式为橙色
    } else if (pressure < 1020) {
        pressureStatus = "正常 🌍";
        ui->label_airpress_status->setStyleSheet("color: green;"); // 设置样式为绿色
    } else {
        pressureStatus = "高气压 ⬆️";
        ui->label_airpress_status->setStyleSheet("color: red;"); // 设置样式为红色
    }
    ui->label_airpress_status->setText(pressureStatus);
}

// 更新 PM2.5 状态文本
void Widget::updatePMStatus(float pm)
{
    QString PMStatus;
    if (pm < 35) {
        PMStatus = "优 ✅";
        ui->label_PM_status->setStyleSheet("color: green;"); // 设置样式为绿色
    } else if (pm < 75) {
        PMStatus = "良 🙂";
        ui->label_PM_status->setStyleSheet("color: lightgreen;"); // 设置样式为浅绿色
    } else if (pm < 115) {
        PMStatus = "轻度污染 😷";
        ui->label_PM_status->setStyleSheet("color: orange;"); // 设置样式为橙色
    } else if (pm < 150) {
        PMStatus = "中度污染 😨";
        ui->label_PM_status->setStyleSheet("color: red;"); // 设置样式为红色
    } else {
        PMStatus = "重度污染 ☠️";
        ui->label_PM_status->setStyleSheet("color: darkred;"); // 设置样式为深红色
    }
    ui->label_PM_status->setText(PMStatus);
}

// 更新所有状态显示
void Widget::updateAllStatuses(const data::Data &labelsData)
{
    updateTempStatus(labelsData.temp());
    updateHumidityStatus(labelsData.humi());
    updateAirQualityStatus(labelsData.airque());
    updatePressureStatus(labelsData.airpress());
    updatePMStatus(labelsData.density());
}

// 主更新函数
void Widget::slots_updateLabels(data::Data labelsData)
{
    setConnectionStatus("已连接"); // 更新连接状态
    m_timeoutTimer->start(); // 复位定时器，防止误判掉线

    ui->label_DEVNAME->setText(QString("远端设备:%1").arg(labelsData.deviceName()));

    // 更新数据数值
    ui->label_data_temp_show->setText(QString("%1 ℃").arg(labelsData.temp(), 5, 'f', 2, QChar('0')));
    ui->label_data_humi_show->setText(QString("%1 RH%").arg(labelsData.humi()));
    ui->label_data_air_show->setText(QString("%1 ppm").arg(labelsData.airque()));
    ui->label_data_airpress_show->setText(QString("%1 hpa").arg(labelsData.airpress()));
    ui->label_data_PM_show->setText(QString("%1 ug/m³").arg(QString::number(labelsData.density(), 'f', 2)));

    // 更新状态文本
    updateAllStatuses(labelsData);
}

void Widget::slots_updateThresholdLabels(data::Data labelsData)
{
    // 温度阈值
    QString tempThresholdString = QString("阈值: %1 %2").arg(labelsData.tempThreshold(), 5, 'f', 2, QChar('0')).arg("℃");
    ui->label_tempThreshold->setText(tempThresholdString);
    // 湿度阈值（假设湿度是整数，不带小数）
    QString humiThresholdString = QString("阈值: %1 RH%").arg(labelsData.humiThreshold());
    ui->label_humiThreshold->setText(humiThresholdString);
    // 空气质量阈值
    QString airQThresholdString = QString("阈值: %1 ppm").arg(labelsData.airQThreshold());
    ui->label_airQThreshold->setText(airQThresholdString);
    // 气压阈值
    QString pressThresholdString = QString("阈值: %1 hpa").arg(labelsData.pressThreshold());
    ui->label_pressThreshold->setText(pressThresholdString);
    // PM2.5 阈值
    QString pm25ThresholdString =
        QString("阈值: %1 ug/m³").arg(QString::number(labelsData.pm25Threshold(), 'f', 2));
    ui->label_pm25Threshold->setText(pm25ThresholdString);
}

void Widget::on_pushButton_disconnect_clicked()
{
    if (client == nullptr || client->m_client == nullptr) {
        qDebug() << "客户端未初始化";
        return;
    }

    if (!client->isConnected()) {
        qDebug() << "已断开!!!"<<client->isConnected();
        return;
    }

    qDebug() << "on_pushButton_disconnect_clicked";
    client->disconnected();  // 断开连接
    setConnectionStatus("已断开");  // 更新连接状态
    drawData->stopUpdating();
}

void Widget::on_pushButton_connect_clicked()
{
    if (client == nullptr || client->m_client == nullptr) {
        qDebug() << "客户端未初始化"; 
        return;
    }

    if (client->isConnected()) {
        qDebug() << "已连接,无需重复连接";
        return;
    }

    qDebug() << "on_pushButton_connect_clicked";
    client->connectHost();  // 连接服务器
    setConnectionStatus("已连接");  // 更新连接状态
    client->subscribe();
    connect(client, &MQTTClient::signal_new_data_recevied, this,
            [this](double temp, double humidity, qint64 timestamp)
            {
                if (drawData) {  // 检查 drawData 是否为空
                    drawData->appendData(temp, humidity, timestamp);
                } else {
                    qDebug() << "drawData is nullptr!";
                }
            });
    connect(client,&MQTTClient::signal_update_labels,this,
            &Widget::slots_updateLabels);
    connect(client,&MQTTClient::signal_updateThresholdLabels,this,
            &Widget::slots_updateThresholdLabels);
    drawData->startUpdating();
}

// 通用的阈值发送方法（模板版本）
template <typename T>
void Widget::sendThreshold(const QString &key, T value)
{
    // 构造 JSON
    QString json = MQTTJsonHelper::constructThresholdJson(key, value);
    qDebug() << "构造的 JSON：" << json;

    // 创建 QTimer 定时发送
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this, json, timer]() {
        client->publishJson(json);
        qDebug() << "发送 JSON：" << json;
    });

    timer->setInterval(500);
    timer->start();

    // 创建 QTimer 用于停止发送
    QTimer *stopTimer = new QTimer(this);
    connect(stopTimer, &QTimer::timeout, [timer, stopTimer]() {
        timer->stop();
        timer->deleteLater();
        stopTimer->stop();
        stopTimer->deleteLater();
        qDebug() << "停止发送 JSON。";
    });

    stopTimer->setSingleShot(true);
    stopTimer->start(2000);
}

// 处理温度阈值的按钮点击事件
void Widget::on_pushButton_set_tempThresholed_clicked()
{
    QString inputText = ui->lineEdit_tempThresholed->text();
    bool ok;
    double tempThreshold = inputText.toDouble(&ok);

    if (!ok) {
        qDebug() << "输入的温度阈值无效";
        return;
    }

    sendThreshold("tempThreshold", tempThreshold);
}

// 处理湿度阈值的按钮点击事件
void Widget::on_pushButton_set_humiThresholed_clicked()
{
    QString inputText = ui->lineEdit_humiThresholed->text();
    bool ok;
    int humiThreshold = inputText.toInt(&ok);  // 这里使用 int，因为湿度一般是整数

    if (!ok) {
        qDebug() << "输入的湿度阈值无效";
        return;
    }

    sendThreshold("humiThreshold", humiThreshold);
}



// 处理 PM2.5 阈值的按钮点击事件
void Widget::on_pushButton_set_PMThresholed_clicked()
{
    QString inputText = ui->lineEdit_PMThresholed->text();
    bool ok;
    double pmThreshold = inputText.toDouble(&ok);  // PM2.5 通常是浮点数

    if (!ok) {
        qDebug() << "输入的 PM2.5 阈值无效";
        return;
    }

    sendThreshold("pm25Threshold", pmThreshold);
}

// 处理气压阈值的按钮点击事件
void Widget::on_pushButton_set_pressThresholed_clicked()
{
    QString inputText = ui->lineEdit_pressThresholed->text();
    bool ok;
    int pressThreshold = inputText.toInt(&ok);

    if (!ok) {
        qDebug() << "输入的气压阈值无效";
        return;
    }

    sendThreshold("pressThreshold", pressThreshold);
}

// 处理空气质量阈值的按钮点击事件
void Widget::on_pushButton_set_airQThresholed_clicked()
{
    QString inputText = ui->lineEdit_airQThresholed->text();
    bool ok;
    int airQThreshold = inputText.toInt(&ok);  // 空气质量通常是整数（AQI）

    if (!ok) {
        qDebug() << "输入的空气质量阈值无效";
        return;
    }

    sendThreshold("airQThreshold", airQThreshold);
}


