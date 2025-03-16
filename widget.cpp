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
}

Widget::~Widget()
{
    delete ui;
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


void Widget::slots_updateLabels(data::Data labelsData)
{
    QString devName = QString("远端设备:%1").arg(labelsData.deviceName());
    ui->label_DEVNAME->setText(devName);
    // 将 float 转换为 QString，并格式化为 "00.00" 样式
    QString tempString = QString("%1 %2").arg(labelsData.temp(), 5, 'f', 2, QChar('0')).arg("℃");
    ui->label_data_temp_show->setText(tempString);
    // 格式化湿度（假设湿度是整数，不带小数）
    QString humiString = QString("%1 RH%").arg(labelsData.humi());
    ui->label_data_humi_show->setText(humiString);
    // 空气质量
    QString airqueString = QString("%1 ppm").arg(labelsData.airque());
    ui->label_data_air_show->setText(airqueString);
    // 气压
    QString airPressString = QString("%1 hpa").arg(labelsData.airpress());
    ui->label_data_airpress_show->setText(airPressString);
    // PM2.5
    QString PMString = QString("%1 ug/m³").arg(QString::number(labelsData.density(), 'f', 2));
    ui->label_data_PM_show->setText(PMString);
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


