#include "widget.h"

#include <QApplication>
#include <thread>
#include <chrono>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    // 加载 QSS 样式表
    QFile file(":/qss/style.qss"); // 使用 Qt 资源文件方式
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = file.readAll();
        a.setStyleSheet(styleSheet); // 设置整个应用的样式
        file.close();
    }

    Widget w;
    w.show();
    return a.exec();
}
