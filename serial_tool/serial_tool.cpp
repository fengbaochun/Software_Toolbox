#include "serial_tool/serial_tool.h"
#include "ui_serial_tool.h"

serial_tool::serial_tool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::serial_tool)
{
    ui->setupUi(this);

    //按钮填充
    btn[0]=ui->openButton;
    btn[1]=ui->sendButton;
    btn[2]=ui->clearButton;

    //连接信号槽
    connect(ui->openButton,&QPushButton::clicked,this,&serial_tool::but_manage);
    connect(ui->sendButton,&QPushButton::clicked,this,&serial_tool::but_manage);
    connect(ui->clearButton,&QPushButton::clicked,this,&serial_tool::but_manage);

    //链接信号槽
//    for(int i=0;i<3;i++)
//    {
//        //连接信号槽
//        connect(btn[i],SIGNAL(clicked()),this,SLOT(but_manage()));
//    }

    //波特率下拉列表设置
    QStringList baudList;
    baudList << u8"4800" << u8"9600" << u8"38400" << u8"115200" << u8"230400" << u8"460800";
    ui->baudBox->addItems(baudList);        // 添加列表到控件中
    ui->baudBox->setCurrentText(baudList[3]);//设置当前选项

    //校检下拉列表设置
    QStringList xcheckList;
    xcheckList << u8"无校检" << u8"奇校检" << u8"偶校检" << u8"1校检" << u8"0校检" ;
    ui->xcheckBox->addItems(xcheckList);        // 添加列表到控件中
    ui->xcheckBox->setCurrentText(xcheckList[0]);//设置当前选项

    //停止位下拉列表设置
    QStringList stopList;
    stopList << u8"0" << u8"1" << u8"2" ;
    ui->stopBox->addItems(stopList);        // 添加列表到控件中
    ui->stopBox->setCurrentText(stopList[0]);//设置当前选项

    //停止位下拉列表设置
    QStringList dataList;
    dataList << u8"5" << u8"6" << u8"7" << u8"8" ;
    ui->dataBox->addItems(dataList);        // 添加列表到控件中
    ui->dataBox->setCurrentText(dataList[3]);//设置当前选项

    ui->hex_show_checkBox->setEnabled(true);//设置这个框是否可选
    ui->hex_show_checkBox->setCheckState(Qt::Unchecked);//设置checkbox 状态
    ui->rev_sw_checkBox->setCheckState(Qt::Checked);

    qDebug()<<ui->hex_show_checkBox->checkState();//输出当前状态
    ui->hex_show_checkBox->isChecked();

    //ui->hex_show_checkBox->setCheckState(Qt::Unchecked);

    /**********************************************************************/
    //获取当前下拉列表的内容
    QString current_dataBox=ui->dataBox->currentText();//获取当前combox的文本
    int dataBox_index=ui->dataBox->currentIndex();//获取当前combox的文本

    qDebug()<<dataBox_index;  //打印数据位下拉列表的索引
    qDebug()<<dataList[dataBox_index];  //打印数据位下拉列表的内容
    qDebug()<<current_dataBox;  //打印数据位下拉列表的内容

    /**********************************************************************/

    //ui->Rev_textBrowser->insertPlainText((tr("Welcome to use Serial Assistant!!!")));
    ui->Rev_textBrowser->setText("Welcome to use Serial Assistant!!!");

    find_portinfo();

}

serial_tool::~serial_tool()
{
    delete ui;
}


void serial_tool::but_manage()
{
    //发送信号者的对象
    QPushButton *optBtn = qobject_cast<QPushButton *>(sender());
    //发送信号者的对象名
    QString name = sender()->objectName();

    if(btn[0] == optBtn)
    {
        open_port();
    }
    else if(btn[1] == optBtn)
    {
        send_data();
    }
    else if(btn[2] == optBtn)
    {
        clear_send_buf();
    }

}
void serial_tool::find_portinfo()
{
    //查找可用的串口
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))//依次打开可读可写的所有设备
        {
            //ui->comBox->addItem(serial.portName()+" "+info.description());
            ui->comBox->addItem(serial.portName());

            serial_num++;

            serial.close();
        }
    }
    qDebug() << u8"串口个数" << serial_num;
}


void serial_tool::open_port()
{
    //判断按键状态
   if(ui->openButton->text() == tr(u8"打开串口"))
   {
       //设置串口名字
       serial.setPortName(ui->comBox->currentText());
       //打开串口(读写模式)
       //serial.open(QIODevice::ReadWrite);
       if(serial.open(QIODevice::ReadWrite))
       {
           qDebug()<< ui->comBox->currentText() <<u8"空闲";
       }
       else
       {
           qDebug()<< ui->comBox->currentText() <<u8"忙碌，请先关闭再打开";
           return ;
       }
       //设置波特率
       serial.setBaudRate(ui->baudBox->currentText().toInt());
       //设置数据位
       switch(ui->dataBox->currentText().toInt())
       {
           case 5: serial.setDataBits(QSerialPort::Data5); break;
           case 6: serial.setDataBits(QSerialPort::Data6); break;
           case 7: serial.setDataBits(QSerialPort::Data7); break;
           case 8: serial.setDataBits(QSerialPort::Data8); break;

       }
       //设置校检位
       serial.setParity(QSerialPort::NoParity);
       //设置停止位
       switch (ui->stopBox->currentIndex())
       {
           case 1: serial.setStopBits(QSerialPort::OneStop); break;
           case 2: serial.setStopBits(QSerialPort::TwoStop); break;
       }
       //设置流控制
       serial.setFlowControl(QSerialPort::NoFlowControl);
       //设置打开按钮的文本
       ui->openButton->setText(tr(u8"关闭串口"));
       qDebug()<< ui->comBox->currentText() <<u8"已打开";
       qDebug()<< u8"串口状态"<<serial.isOpen();

      //开启定时器
       connect(update_data,&QTimer::timeout,this,&serial_tool::show_rev_data);
       update_data->start(5); //5ms定时器
       qDebug() << u8"定时器 ID:"<<update_data->timerId();
   }
   else
   {
       //判断串口确实打开了
       if(serial.isOpen())
       {
           serial.clear();
           serial.close();
           //serial.deleteLater();
           ui->openButton->setText(tr(u8"打开串口"));
           qDebug()<< ui->comBox->currentText() <<u8"已关闭";
           update_data->stop();//停止定时器
           qDebug()<<u8"定时器已停止";
       }
   }
}

//清空发送区
void serial_tool::clear_send_buf()
{
    ui->Send_textEdit->clear();
    qDebug()<< u8"已清空";
}

//读取数据
QByteArray serial_tool::read_data()
{
    QByteArray buf;
    //一次性读取所有数据
    buf=serial.readAll();
    return buf;
}

//显示接收数据
void serial_tool::show_rev_data()
{
    QByteArray buf=read_data();
    if(!buf.isEmpty())//判断是否有数据
    {
        ui->Rev_textBrowser->append(tr(buf));
    }
    buf.clear();
}

void serial_tool::send_data()
{
    temp_str = ui->Send_textEdit->toPlainText();
    qDebug()<< u8"字符串长度："<<temp_str.length();
    //长度>0 才发送数据
    if(temp_str.length()&& serial.isOpen())
    {
        serial.write(temp_str.toUtf8());//发送数据
        qDebug()<< temp_str.toUtf8() <<u8"已发送";
    }
}


