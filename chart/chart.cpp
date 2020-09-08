#include "chart.h"
#include "serial.h"
#include "ui_chart.h"


Chart::Chart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chart)
{
    ui->setupUi(this);
    // 初始化成员变量
    m_pauseTime = 0;
    m_vecPidLastData.resize(5); // 存放5个通道的PID上一次结果值
    m_ismoving = false;

    // 初始化图表
    initChart(ui->chartWidget);

    // 初始化串口
    m_serial = new Serial;
    // 寻找可用串口
    QStringList serialStrList;
    serialStrList = m_serial->scanSerial();
    for (int i=0; i<serialStrList.size(); i++)
    {
        ui->portComboBox->addItem(serialStrList[i]); // 在comboBox那添加串口号
    }
    // 默认设置波特率为115200（第5项）
    ui->baudComboBox->setCurrentIndex(5);
    // 当下位机中有数据发送过来时就会响应这个槽函数
    connect(m_serial, SIGNAL(readOneFrame()), this, SLOT(readFromSerial()));
    // 处理串口错误
    connect(m_serial, SIGNAL(errorSignal()), this, SLOT(handleSerialError()));
    // 连接onNewSerialPort信号槽，定时获取串口列表
    connect(m_serial, SIGNAL(onNewSerialPort(QStringList)),this, SLOT(onNewPortList(QStringList)));
}

void Chart::initChart(QCustomPlot *ChartWidget)
{
    // 初始化图表实例指针
    m_customPlot = ChartWidget;
    // 将customPlot的mouseMove信号连接到mouseCoordConver函数
//    connect(m_customPlot, &QCustomPlot::mouseMove, this, &Chart::mouseCoordConver);
    // 安装事件过滤器
    m_customPlot->installEventFilter(this);
    // 设置背景色
    m_customPlot->setBackground(QBrush(QColor(0, 0, 0)));
    // 设置x/y轴刻度标签颜色
    m_customPlot->yAxis->setTickLabelColor(QColor(255, 255, 255));
    m_customPlot->xAxis->setTickLabelColor(QColor(255, 255, 255));
    // 支持鼠标拖拽轴的范围、滚动缩放轴的范围，左键点选图层（每条曲线独占一个图层）
    m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // 添加通道1曲线
    m_customPlot->addGraph(); // 向绘图区域QCustomPlot添加一条曲线
    m_customPlot->graph(0)->setPen(QPen(Qt::red, 3)); // 设置曲线颜色
    // 添加通道2曲线
    m_customPlot->addGraph();
    m_customPlot->graph(1)->setPen(QPen(Qt::magenta, 3));
    // 添加通道3曲线
    m_customPlot->addGraph();
    m_customPlot->graph(2)->setPen(QPen(Qt::green, 3));
    // 添加通道4曲线
    m_customPlot->addGraph();
    m_customPlot->graph(3)->setPen(QPen(Qt::blue, 3));
    // 添加通道5曲线
    m_customPlot->addGraph();//添加graph等价于添加新曲线
    m_customPlot->graph(4)->setPen(QPen(Qt::cyan, 3));

    // 最开始设置Y轴的范围为0-1000
    m_customPlot->yAxis->setRange(0, 1000);

    // 创建时间坐标轴
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%s");
    timeTicker->setTickCount(10); // 设置越大，同屏显示的时间栅格越多
    m_customPlot->xAxis->setTicker(timeTicker);

    for(int i=0; i<CHANNEL_COUNT; i++)
    {
        // 自动设定graph曲线x和y轴的范围
        m_customPlot->graph(i)->rescaleAxes(true);
        // 解决点击曲线，曲线变蓝色的问题
        m_customPlot->graph(i)->setSelectable(QCP::stNone);
    }

    // 立即刷新图表
    m_customPlot->replot();
}

// 读取来自串口类的数据
void Chart::readFromSerial()
{
    QByteArray readBuf = m_serial->getReadBuf();
    qDebug()<<readBuf;
    // 清除读取数据缓冲区
    m_serial->clearReadBuf();
}


// 处理串口错误
void Chart::handleSerialError()
{
//    MyMessageBox *msgBox = new MyMessageBox();
//    msgBox->showMyMessageBox(nullptr, tr("错误提示"), tr("串口连接中断，请检查是否正确连接!"), MESSAGE_WARNNING,
//                                        BUTTON_OK, true);

    // 再次触发"打开串口"按钮槽函数，以关闭串口
    ui->openSerialButton->setText(tr("关闭串口"));
    emit ui->openSerialButton->clicked();
}

// 用于响应SerialPortList信号，周期获取串口列表
void Chart::onNewPortList(QStringList portName)
{
    ui->portComboBox->clear();
    ui->portComboBox->addItems(portName);
}


Chart::~Chart()
{
    delete ui;
}
