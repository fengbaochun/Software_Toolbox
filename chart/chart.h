#ifndef CHART_H
#define CHART_H

#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTime>
#include <QDebug>
#include "chart/qcustomplot.h"
#include "chart/serial.h"

namespace Ui {
class Chart;
}
#define CHANNEL_COUNT 5 // 通道数量
class Chart : public QWidget
{
    Q_OBJECT

public:
    explicit Chart(QWidget *parent = nullptr);
    ~Chart();
    void initChart(QCustomPlot *ChartWidget); // 初始化图表

private slots:
    void readFromSerial(); // 读取来自串口类的数据
    void handleSerialError(); // 处理串口错误
    void onNewPortList(QStringList portName); // 用于响应SerialPortList信号，周期获取串口列表

private:
    QCustomPlot *m_customPlot; // 图表实例指针
    QVector<int> m_vecPidLastData; // 存放5个通道的PID上一次结果值
    QMap<double, int> m_mapChannel[5]; // 本地保存各通道的数据
    bool m_ismoving; // 鼠标左键是否移动和滚轮是否使用的标志位

    QTime m_pauseTimer; // 暂停时间计时器
    int m_pauseTime; // 暂停时间
    QTime m_serialTime; // 串口打开后的计时器
    Serial *m_serial; // 自定义串口类实例指针

    Ui::Chart *ui;
};

#endif // CHART_H
