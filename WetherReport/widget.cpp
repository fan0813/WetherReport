#include "widget.h"
#include "ui_widget.h"

#include <QMouseEvent>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setFont(QFont("Arial",15));
    setFixedSize(384,791);
    setWindowFlag(Qt::FramelessWindowHint);

    menuQquit = new QMenu(this);
    menuQquit->setStyleSheet("QMenu::item{color:white}");
    QAction *closeAct = new QAction(QIcon(":/res/9.png"),tr("退出"),this);
    menuQquit->addAction(closeAct);
    connect(menuQquit,&QMenu::triggered,this,[=]{
                this->close();
            });
    //由QNetworkAccessManager发起get请求
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    //QNetworkRequest res(QUrl("http://gfeljm.tianqiapi.com/api?unescape=1&version=v61&appid=93421683&appsecret=2Ep9Vz3w"));
    QUrl tianqi ("http://gfeljm.tianqiapi.com/api?unescape=1&version=v61&appid=93421683&appsecret=2Ep9Vz3w");
    QNetworkRequest res(tianqi);
    reply= manager->get(res);
    //网络请求后进行数据读取
    connect(reply,&QNetworkReply::finished,this,&Widget::readHttpReply);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::mousePressEvent(QMouseEvent *event)
{
    //鼠标右键按下，弹出退出窗口，点击窗口就退出
    if(event->button() == Qt::RightButton){
        //qDebug() << "right mouse licked";
        menuQquit->exec(QCursor::pos());
    }

    //鼠标左键按住不动，拖动窗口
    //鼠标当前位置event->globalPos()
    //窗口当前位置this->pos()
    //窗口新位置event->globalPos() - mOffset
    if(event->button() == Qt::LeftButton){
        //qDebug() << event->globalPos() << this->pos();
        mOffset = event->globalPos() - this->pos();
    }


}

//根据鼠标移动偏移值，设置窗口新位置
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

void Widget::readHttpReply()
{
    int resCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << resCode;
    if(reply->error() == QNetworkReply::NoError && resCode == 200){
        //大多数服务器返回的都是utf-8格式
        QByteArray data = reply->readAll();
        qDebug() << QString::fromUtf8(data);
    }else{
        QMessageBox mes;
        mes.setWindowTitle("错误");
        mes.setText("网络请求失败");
        mes.setStyleSheet("QPushButton {color:red}");
        mes.exec();
    }

}



