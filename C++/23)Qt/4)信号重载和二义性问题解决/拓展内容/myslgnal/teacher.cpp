#include "teacher.h"
#include<QDebug>

Teacher::Teacher(QWidget *parent) : QWidget(parent)
{

}

void Teacher::test()
{

    qDebug()<<"自定义槽函数";
}

void Teacher::test2(QString str)
{
    qDebug()<<"带参数的槽函数"<<str;
}

void Teacher::test(QString str2)
{
    qDebug()<<"带参数的重载的槽函数"<<str2;
}

void Teacher::test3()
{
    qDebug()<<"按键发送的信号";
}


