// DrawData.h

#ifndef DRAWDATA_H
#define DRAWDATA_H

#include <QObject>
#include <QWidget>
#include <QQueue>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QDateTimeAxis>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QTimer>

struct SensorData {
    double temperature;
    double humidity;
    qint64 timestamp;

    SensorData(double temp, double hum, qint64 ts)
        : temperature(temp), humidity(hum), timestamp(ts) {}
    SensorData()
        : temperature(2), humidity(2), timestamp(0) {}
};


class DrawData : public QObject
{
    Q_OBJECT

public:
    DrawData(QWidget *parentWidget);

    void appendData(double temp, double humidity, qint64 timestamp); // 生产者：接收并放入数据
    void startUpdating(); // 启动消费者线程
    void stopUpdating(); // 停止消费者线程

    void initChart();
private:
    QWidget* m_parentWidget;
    QChart* m_chart;              // 图表对象
    QChartView* m_charView;       // 图表视图
    QQueue<SensorData> m_dataQueue; // 存储温湿度数据的队列
    QSplineSeries *m_tempSeries;  // 温度数据系列
    QSplineSeries *m_humiditySeries; // 湿度数据系列
    QDateTimeAxis *m_axisX;         // X轴（时间）
    QValueAxis *m_axisY1;        // Y轴1（温度）
    QValueAxis *m_axisY2;        // Y轴2（湿度）

    QMutex m_mutex; // 保护数据队列的锁
    bool m_running; // 用于控制消费者线程的停止

    void updateChart(); // 消费者：从队列中取数据并更新图表

private:
    QTimer* timer;
    double m_lastTemp;     // 记录最新的温度值
    double m_lastHumidity; // 记录最新的湿度值
};

#endif // DRAWDATA_H
