#include "drawdata.h"

#include <QSplineSeries>
#include <QChart>
#include <QValueAxis>
#include <QChartView>

DrawData::DrawData(QWidget *parentWidget)
    : m_parentWidget(parentWidget)
{
    // 创建图表实例
    m_chart = new QChart();
    m_chart->setTitle("环境数据变化");  // 设置图表标题

    // 创建温度和湿度数据系列
    QSplineSeries *tempSeries = new QSplineSeries();
    QSplineSeries *humiditySeries = new QSplineSeries();

    tempSeries->setName("温度");
    // 设置温度系列的数据
    tempSeries->append(0, 20);  // 时间点0，温度20°C
    tempSeries->append(1, 22);  // 时间点1，温度22°C
    tempSeries->append(2, 25);  // 时间点2，温度25°C
    tempSeries->append(3, 30);  // 时间点3，温度30°C
    tempSeries->append(4, 26);  // 时间点4，温度26°C
    tempSeries->append(5, 25);  // 时间点5，温度25°C
    tempSeries->append(6, 20);  // 时间点6，温度20°C
    tempSeries->append(7, 29);  // 时间点7，温度29°C

    humiditySeries->setName("湿度");
    humiditySeries->append(0, 60);  // 时间点0，湿度60%
    humiditySeries->append(1, 65);  // 时间点1，湿度65%
    humiditySeries->append(2, 70);  // 时间点2，湿度70%
    humiditySeries->append(3, 75);  // 时间点3，湿度75%
    humiditySeries->append(4, 80);  // 时间点4，湿度80%
    humiditySeries->append(5, 85);  // 时间点5，湿度85%
    humiditySeries->append(6, 90);  // 时间点6，湿度90%
    humiditySeries->append(7, 95);  // 时间点7，湿度95%

    // 将系列添加到图表
    m_chart->addSeries(tempSeries);
    m_chart->addSeries(humiditySeries);

    // 创建 X 轴
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("时间 (小时)");  // 设置X轴标题
    axisX->setLabelFormat("%d");  // 设置X轴标签格式
    axisX->setRange(0, 7);  // 设置X轴范围（0到7小时）

    // 创建 Y 轴（分别对应温度和湿度）
    QValueAxis *axisY1 = new QValueAxis();
    axisY1->setTitleText("温度 (°C)");  // 设置 Y 轴1 标题
    axisY1->setLabelFormat("%.1f");
    axisY1->setRange(15, 35); // 设置温度的显示范围

    QValueAxis *axisY2 = new QValueAxis();
    axisY2->setTitleText("湿度 (%)");  // 设置 Y 轴2 标题
    axisY2->setLabelFormat("%.1f");
    axisY2->setRange(50, 100); // 设置湿度的显示范围

    // 添加 X 轴和 Y 轴到图表
    m_chart->addAxis(axisX, Qt::AlignBottom);  // 添加 X 轴并设置对齐方式
    m_chart->addAxis(axisY1, Qt::AlignLeft);   // 添加第一个 Y 轴并设置对齐方式
    m_chart->addAxis(axisY2, Qt::AlignRight);  // 添加第二个 Y 轴并设置对齐方式

    // 将系列和对应的轴绑定
    tempSeries->attachAxis(axisX);
    tempSeries->attachAxis(axisY1);

    humiditySeries->attachAxis(axisX);
    humiditySeries->attachAxis(axisY2);


    // 创建 QChartView 显示图表
    m_charView = new QChartView(m_chart);
    m_charView->setRenderHint(QPainter::Antialiasing);  // 开启反走样，增强图表显示效果
    m_charView->setParent(m_parentWidget);  // 设置父控件
    m_charView->setGeometry(0, 0, 990, 400);  // 设置图表视图的大小

    // 显示图表
    m_charView->show();
}
