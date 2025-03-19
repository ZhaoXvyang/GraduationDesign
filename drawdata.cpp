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

/**
 * @brief 初始化环境数据图表。
 *
 * 此函数用于创建和配置环境数据的图表，包括温度和湿度的动态变化。
 *
 * 函数执行以下操作：
 * 1. 创建一个新的 QChart 对象，并设置标题为 "环境数据动态变化"。
 * 2. 创建两个 QSplineSeries 对象，分别用于温度和湿度的数据序列，并设置它们的名称。
 * 3. 将温度和湿度系列添加到图表中。
 * 4. 创建一个 QDateTimeAxis 作为 X 轴，设置标题、格式、范围和刻度数。
 * 5. 创建两个 QValueAxis 分别作为 Y 轴，分别用于温度和湿度，并设置它们的标题、范围和刻度数。
 * 6. 将 X 轴和 Y 轴添加到图表中，并将数据系列附加到相应的轴上。
 * 7. 创建一个 QChartView 对象用于显示图表，设置渲染提示和父部件，配置其几何位置。
 * 8. 设置温度系列和湿度系列的颜色，并禁用动画以提升性能。
 * 9. 显示图表视图。
 *
 * @note 初始时，X 轴的范围设置为当前时间到未来 30 秒。
 *       温度 Y 轴的范围为 0 到 40°C，湿度 Y 轴的范围为 5 到 65%。
 *
 * @warning 确保在调用此函数之前已正确设置 m_parentWidget 作为图表的父窗口。
 */
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
    m_axisY1->setRange(0, 40);
    m_axisY1->setTickCount(9);

    m_axisY2 = new QValueAxis();
    m_axisY2->setTitleText("湿度 (%)");
    m_axisY2->setRange(5, 65);

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
/**
 * @brief 开始定时更新图表数据。
 *
 * 此函数用于启动一个定时器，以定期更新环境数据图表。
 *
 * 如果图表正在运行，则此函数将不会执行任何操作。
 * 否则，将设置 m_running 标志为 true，并创建一个新的 QTimer 对象，
 * 该定时器每 2000 毫秒（2 秒）触发一次 timeout 信号，从而调用 updateChart() 方法。
 *
 * @note 定时器的间隔设置为 2000 毫秒，以平衡性能与图表更新的流畅度。
 *
 * @warning 确保在调用此函数之前，图表对象已被正确初始化。
 */
void DrawData::startUpdating()
{
    if (m_running) return;

    m_running = true;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DrawData::updateChart);
    // 在startUpdating()中调整定时器间隔
    timer->start(2000); // 1s刷新平衡性能与流畅度
}

/**
 * @brief 将新传感器数据添加到数据队列中。
 *
 * 此函数用于将温度和湿度数据添加到内部数据队列。
 * 在添加数据之前，通过 QMutexLocker 确保线程安全。
 *
 * @param temp 传感器读取的温度值，单位为摄氏度。
 * @param humidity 传感器读取的湿度值，单位为百分比。
 * @param timestamp 时间戳参数被忽略，使用当前系统时间的毫秒级时间戳。
 *
 * @note 函数内部使用当前系统时间生成时间戳，并将传感器数据封装为 SensorData 对象，
 *       然后将其入队到 m_dataQueue 中。
 *
 * @warning 确保在使用此函数时，数据队列和互斥锁已被正确初始化。
 */
void DrawData::appendData(double temp, double humidity, qint64 /*timestamp*/)
{
    QMutexLocker locker(&m_mutex);
    // 使用当前系统时间的毫秒级时间戳
    qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();
    m_dataQueue.enqueue(SensorData(temp, humidity, currentTimestamp));
}

/**
 * @brief 更新图表数据并刷新显示。
 *
 * 此函数定期调用以更新温度和湿度图表的数据。它执行以下操作：
 *
 * 1. 获取当前时间并对齐到整秒边界，以计算数据窗口的开始和结束时间。
 * 2. 使用 QMutexLocker 确保线程安全，并处理数据队列中的所有传感器数据。
 * 3. 将提取的新数据点添加到温度和湿度的系列中。
 * 4. 如果没有新数据，则添加最后读取的温度和湿度值。
 * 5. 清理过时的数据点，以保持图表的整洁性。
 * 6. 设置 X 轴的范围，以对齐到整秒，并确保每秒有一个刻度。
 * 7. 刷新图表以显示更新后的数据。
 *
 * @note 数据清理只保留在最近 30 秒内的点，以确保图表只显示当前有效数据。
 *
 * @warning 确保在调用此函数之前，数据队列和图表对象已被正确初始化。
 */
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

/**
 * @brief 停止定时更新图表数据。
 *
 * 此函数用于停止定时器，从而暂停图表数据的更新。
 *
 * 调用此函数将设置 m_running 标志为 false，并停止定时器的运行。
 *
 * @note 在停止更新之前，请确保图表正在运行。
 *
 * @warning 调用此函数后，图表将不再更新，需重新调用 startUpdating() 来恢复更新。
 */
void DrawData::stopUpdating()
{
    m_running = false;
    timer->stop();
}
