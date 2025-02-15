#include "widget.h"
#include "ui_widget.h"
#include "drawdata.h"
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

    DrawData* dd = new DrawData(ui->widget);

}

Widget::~Widget()
{
    delete ui;
}

