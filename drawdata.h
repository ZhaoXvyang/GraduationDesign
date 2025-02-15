#ifndef DRAWDATA_H
#define DRAWDATA_H

#include <QWidget>
#include <QVector>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>


class DrawData
{
public:
    DrawData(QWidget *parentWidget);

private:
    QWidget* m_parentWidget;
    QChart* m_chart; // 图表对象
    QChartView* m_charView; // 图标视图
};

#endif // DRAWDATA_H
