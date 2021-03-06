#include "XVideoUI.h"
#include <QFileDialog>
#include <string>
#include <iostream>
#include <QMessageBox>
#include "XVideoThread.h"
using namespace std;

XVideoUI::XVideoUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
	// 隐藏窗口顶部标题栏
	setWindowFlags(Qt::FramelessWindowHint);
	// Qt原本是不支持cv::Mat作为信号和槽的传递参数, 所以这里需要注册一下才能使用
	qRegisterMetaType<cv::Mat>("cv::Mat");
	QObject::connect(XVideoThread::Get(),
		SIGNAL(ViewImage1(cv::Mat)),
		ui.src1,
		SLOT(SetImage(cv::Mat))
	);
}

void XVideoUI::Open()
{
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("请选择视频文件"));
	if (name.isEmpty()) return;
	string file = name.toLocal8Bit().data();
	//QMessageBox::information(0, "", name);
	//cout << file << endl;
	if (!XVideoThread::Get()->Open(file))
	{
		cout << "open file failed!" << endl;
		return;
	}
}
