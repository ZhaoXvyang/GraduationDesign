#ifndef DRAWDATA_H
#define DRAWDATA_H

#include <QWidget>
#include <QVector>
#include <QQueue>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include "data.h"

class DrawData
{
public:
    DrawData(QWidget *parentWidget);

private:
    QWidget* m_parentWidget;
    QChart* m_chart; // 图表对象
    QChartView* m_charView; // 图标视图
    QQueue<data::Data*> m_buffer_queue; // 数据缓冲区
};

#endif // DRAWDATA_H
