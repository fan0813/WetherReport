#include "widget.h"
#include "ui_widget.h"

#include <QMouseEvent>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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
    manager = new QNetworkAccessManager(this);
    //QNetworkRequest res(QUrl("http://gfeljm.tianqiapi.com/api?unescape=1&version=v61&appid=93421683&appsecret=2Ep9Vz3w"));
    strUrl = "http://gfeljm.tianqiapi.com/api?unescape=1&version=v61&appid=93421683&appsecret=2Ep9Vz3w";
    QUrl tianqi (strUrl);
    QNetworkRequest res(tianqi);
    reply= manager->get(res);
    //网络请求后进行数据读取
    connect(manager,&QNetworkAccessManager::finished,this,&Widget::readHttpReply);
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

void Widget::parseWeatherJsonData(QByteArray rawData)
{
    QJsonDocument jsonObj = QJsonDocument::fromJson(rawData);
    if(!jsonObj.isNull() && jsonObj.isObject()){
        QJsonObject objRoot = jsonObj.object();
        //解析日期
        QString date = objRoot["date"].toString();
        QString week = objRoot["week"].toString();
        ui->labelCurrentData->setText(date+" "+week);
        //解析当前城市
        QString cityname = objRoot["city"].toString();
        ui->labelCity->setText(cityname+"市");
        //解析当前温度
        QString currentTemp = objRoot["tem"].toString();
        ui->labelTmp->setText(currentTemp+"℃");
        ui->labelTmpRange->setText(objRoot["tem2"].toString() + "℃" + "~" + objRoot["tem1"].toString() + "℃");
        //解析天气类型
        ui->labelWeatherType->setText(objRoot["wea"].toString());
        //感冒指数
        ui->labelGanmao->setText(objRoot["air_tips"].toString());
        //风向
        ui->label_2->setText(objRoot["win"].toString());
        //风力
         ui->label_3->setText(objRoot["win_speed"].toString());
         //PM2.5
         ui->label_11->setText(objRoot["air_pm25"].toString());
         //湿度
         ui->label_17->setText(objRoot["humidity"].toString());
         //空气质量
         ui->label_20->setText(objRoot["air_level"].toString());
    }
}


void Widget::readHttpReply(QNetworkReply *reply)
{
    int resCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << resCode;
    if(reply->error() == QNetworkReply::NoError && resCode == 200){
        //大多数服务器返回的都是utf-8格式        
        QByteArray data = reply->readAll();
        parseWeatherJsonData(data);

        qDebug() << QString::fromUtf8(data);
    }else{
        QMessageBox mes;
        mes.setWindowTitle("错误");
        mes.setText("网络请求失败");
        mes.setStyleSheet("QPushButton {color:red}");
        mes.setStandardButtons(QMessageBox::Ok);
        mes.exec();
    }
}

void Widget::on_pushButton_clicked()
{
    QString citynamefromUser = ui->lineEditCity->text();
    QString cityCode = cityCodeUtils.getCityCodeFromName(citynamefromUser);
    if(cityCode != NULL){
        strUrl += "&cityid=" + cityCode;
        manager->get(QNetworkRequest(QUrl(strUrl)));



    }else{
        QMessageBox mes;
        mes.setWindowTitle("错误");
        mes.setText("请输入正确城市名称");
        mes.setStyleSheet("QPushButton {color:red}");
        mes.setStandardButtons(QMessageBox::Ok);
        mes.exec();
    }

}
