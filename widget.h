#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>
#include <QLabel>
#include <QTimer>
#include "mqtt.h"
#include "drawdata.h"
#include "data.h"
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void getLabelPositions();
    void setConnectionStatus(const QString &status);

public:
    QVector<QLabel*> m_labels;
    MQTTClient* client;
    template<typename T>
    void sendThreshold(const QString &key, T value);

    void updateAllStatuses(const data::Data &labelsData);
    void updatePMStatus(float pm);
    void updatePressureStatus(float pressure);
    void updateAirQualityStatus(int airQuality);
    void updateHumidityStatus(int humidity);
    void updateTempStatus(float temp);
    void checkConnectionStatus();
private slots:
    void slots_updateLabels(data::Data labelsData);
    void slots_updateThresholdLabels(data::Data labelsData);

private slots:
    void on_pushButton_connect_clicked();

    void on_pushButton_disconnect_clicked();

    void on_pushButton_set_tempThresholed_clicked();

    void on_pushButton_set_humiThresholed_clicked();

    void on_pushButton_set_PMThresholed_clicked();

    void on_pushButton_set_pressThresholed_clicked();

    void on_pushButton_set_airQThresholed_clicked();

private:
    Ui::Widget *ui;

    DrawData* drawData;
    QTimer* m_timeoutTimer;
};
#endif // WIDGET_H
