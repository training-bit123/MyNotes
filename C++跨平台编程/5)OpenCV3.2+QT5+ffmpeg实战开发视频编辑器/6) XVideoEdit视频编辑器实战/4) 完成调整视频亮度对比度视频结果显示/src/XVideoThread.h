#pragma once
#include <QThread>
#include <QMutex>
#include <opencv2/core.hpp>
class XVideoThread:public QThread
{
	Q_OBJECT

public:
	int fps = 0;

	static XVideoThread* Get()
	{
		static XVideoThread vt;
		return &vt;
	}
	// 打开一号视频源文件
	bool Open(const std::string file);
	
	// 返回当前播放的位置
	double GetPos();

	// 跳转视频
	///@para frame int 帧位置
	bool Seek(int frame);
	bool Seek(double pos);

	~XVideoThread();

	// 线程入口函数
	void run();

signals:
	//显示原视频1图像
	void ViewImage1(cv::Mat mat);

	//显示生成后图像
	void ViewDes(cv::Mat mat);

protected:
	QMutex mutex;
	XVideoThread();
};

