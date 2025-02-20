// DrawData.cpp

#include "drawdata.h"
#include <QDebug>
#include <QTimer>
#include <QPair>
#include <qdatetime.h>
#include <QDateTimeAxis>

DrawData::DrawData(QWidget *parentWidget)
    : QObject(parentWidget->parent()), m_parentWidget(parentWidget), m_running(false),
    m_lastTemp(25.0), m_lastHumidity(60.0) // 初始化默认值
{
    initChart();
}

void DrawData::initChart()
{
    m_chart = new QChart();
    m_chart->setTitle("环境数据动态变化");

    m_tempSeries = new QSplineSeries();
    m_humiditySeries = new QSplineSeries();
    m_tempSeries->setName("温度");
    m_humiditySeries->setName("湿度");

    m_chart->addSeries(m_tempSeries);
    m_chart->addSeries(m_humiditySeries);

    m_axisX = new QDateTimeAxis();  // 使用 QDateTimeAxis
    m_axisX->setTitleText("时间 (秒)");
    m_axisX->setFormat("mm:ss");  // 设置时间格式
    m_axisX->setRange(QDateTime::currentDateTime(), QDateTime::currentDateTime().addSecs(30));  // 初始显示最近30秒
    m_axisX->setTickCount(16);

    m_axisY1 = new QValueAxis();
    m_axisY1->setTitleText("温度 (°C)");
    m_axisY1->setRange(15, 50);

    m_axisY2 = new QValueAxis();
    m_axisY2->setTitleText("湿度 (%)");
    m_axisY2->setRange(30, 100);

    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY1, Qt::AlignLeft);
    m_chart->addAxis(m_axisY2, Qt::AlignRight);

    m_tempSeries->attachAxis(m_axisX);
    m_tempSeries->attachAxis(m_axisY1);
    m_humiditySeries->attachAxis(m_axisX);
    m_humiditySeries->attachAxis(m_axisY2);

    m_charView = new QChartView(m_chart);
    m_charView->setRenderHint(QPainter::Antialiasing);
    m_charView->setParent(m_parentWidget);
    m_charView->setGeometry(0, 0, 990, 400);

    m_tempSeries->setColor(Qt::green);
    m_humiditySeries->setColor(Qt::blue);
    m_chart->setAnimationOptions(QChart::NoAnimation); // 禁用动画提升性能

    m_charView->show();

}

void DrawData::startUpdating()
{
    if (m_running) return;

    m_running = true;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DrawData::updateChart);
    // 在startUpdating()中调整定时器间隔
    timer->start(1000); // 1s刷新平衡性能与流畅度
}


void DrawData::appendData(double temp, double humidity, qint64 /*timestamp*/)
{
    QMutexLocker locker(&m_mutex);
    // 使用当前系统时间的毫秒级时间戳
    qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();
    m_dataQueue.enqueue(SensorData(temp, humidity, currentTimestamp));
}

void DrawData::updateChart()
{
    const qint64 now = QDateTime::currentMSecsSinceEpoch();

    // 对齐到整秒边界
    const qint64 alignedNow = (now / 1000) * 1000;
    const qint64 windowStart = alignedNow - 30 * 1000;
    const qint64 windowEnd = alignedNow;

    // 处理队列数据
    QVector<SensorData> newData;
    {
        QMutexLocker locker(&m_mutex);
        while (!m_dataQueue.isEmpty()) {
            SensorData data = m_dataQueue.dequeue();
            m_lastTemp = data.temperature;
            m_lastHumidity = data.humidity;
            newData.append(data);
        }
    }

    // 添加新数据点（保持原始时间戳）
    foreach (const SensorData &data, newData) {
        m_tempSeries->append(data.timestamp, data.temperature);
        m_humiditySeries->append(data.timestamp, data.humidity);
    }

    // 无新数据时添加当前值
    if (newData.isEmpty()) {
        m_tempSeries->append(now, m_lastTemp);
        m_humiditySeries->append(now, m_lastHumidity);
    }

    // 清理旧数据
    const qint64 cleanupThreshold = windowStart;
    while (!m_tempSeries->points().isEmpty() &&
           m_tempSeries->points().first().x() < cleanupThreshold) {
        m_tempSeries->remove(0);
    }
    while (!m_humiditySeries->points().isEmpty() &&
           m_humiditySeries->points().first().x() < cleanupThreshold) {
        m_humiditySeries->remove(0);
    }

    // 设置轴范围（强制对齐整秒）
    m_axisX->setRange(QDateTime::fromMSecsSinceEpoch(windowStart),
                      QDateTime::fromMSecsSinceEpoch(windowEnd));
    m_axisX->setTickCount(16); // 确保每秒一个刻度

    m_chart->update();
}

void DrawData::stopUpdating()
{
    m_running = false;
    timer->stop();
}
