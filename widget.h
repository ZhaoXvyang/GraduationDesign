#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>
#include <QLabel>
#include <QTimer>
#include "mqtt.h"
#include "drawdata.h"

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

private slots:
    void on_pushButton_connect_clicked();

    void on_pushButton_subscribe_clicked();

    void on_pushButton_publish_clicked();

    void on_pushButton_disconnect_clicked();

private:
    Ui::Widget *ui;

    DrawData* drawData;
};
#endif // WIDGET_H
