#ifndef MYTOOLS_H
#define MYTOOLS_H
#include <QLabel>
#include <QTimer>
#include <QDateTime>

class LabelUtils {
public:
    static void setLabelAsRealTimeClock(QLabel* label) {
        if (!label) return; // 确保指针有效

        // 创建定时器（堆内存，避免静态函数结束时被销毁）
        QTimer* timer = new QTimer(label); // 绑定到label，随label销毁自动释放
        QObject::connect(timer, &QTimer::timeout, [label]() {
            // 获取当前时间并格式化
            QString timeStr = QDateTime::currentDateTime().toString("MM-dd HH:mm:ss");
            label->setText(timeStr);
        });

        timer->start(1000); // 每秒更新一次
    }
};
#endif // MYTOOLS_H
