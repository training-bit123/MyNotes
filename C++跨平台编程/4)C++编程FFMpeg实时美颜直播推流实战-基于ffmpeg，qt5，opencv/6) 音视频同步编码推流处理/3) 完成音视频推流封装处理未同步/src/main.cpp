#include <QtCore/QCoreApplication>
#include <iostream>
#include <QThread>
#include "XMediaEncode.h"
#include "XRtmp.h"
#include "XAudioRecord.h"
#include "XVideoCapture.h"
using namespace std;
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	
	const char *outUrl = "rtmp://192.168.163.131/live";
	int ret = 0;
	int sampleRate = 44100;
	int channels = 2;
	int sampleByte = 2;
	int nbSamples = 1024;
	///打开摄像机
	XVideoCapture *xv = XVideoCapture::Get();
	if (!xv->Init(0))
	{
		printf("open camera failed!\n");
		getchar();
		return -1;
	}
	printf("open camera success!\n");
	xv->Start();


	///1 qt音频开始录制
	XAudioRecord *ar = XAudioRecord::Get();
	ar->sampleRate = sampleRate;
	ar->channels = channels;
	ar->sampleByte = sampleByte;
	ar->nbSamples = nbSamples;
	if (!ar->Init())
	{
		printf("XAudioRecord Init() failed!\n");
		getchar();
		return -1;
	}
	ar->Start();

	///音视频编码类
	XMediaEncode *xe = XMediaEncode::Get();

	/// 2 初始化格式转换上下文
	/// 3 初始化输出的数据结构
	xe->inWidth = xv->width;
	xe->inHeight = xv->height;
	xe->outWidth = xv->width;
	xe->outHeight = xv->height;
	if (!xe->InitScale())
	{
		getchar();
		return -1;
	}
	printf("初始化视频像素转换上下文成功!\n");

	///2 音频重采样 上下文初始化
	xe->channels = channels;
	xe->nbSamples = nbSamples;
	xe->sampleRate = sampleRate;
	xe->inSampleFmt = XSampleFMT::X_S16;
	xe->outSampleFmt = XSampleFMT::X_FLTP;
	if (!xe->InitResample())
	{
		getchar();
		return -1;
	}

	///4 初始化音频编码器
	if (!xe->InitAudioCodec())
	{
		getchar();
		return -1;
	}
	/// 初始化视频编码器
	if (!xe->InitVideoCodec())
	{
		getchar();
		return -1;
	}

	///5 输出封装器和音频流配置
	// a 创建输出封装器上下文
	XRtmp *xr = XRtmp::Get(0);
	if (!xr->Init(outUrl))
	{
		getchar();
		return -1;
	}

	// b 添加视频流
	int vindex = 0;
	vindex = xr->AddStream(xe->vc);
	if (vindex < 0)
	{
		getchar();
		return -1;
	}

	// b 添加音频流
	int aindex = xr->AddStream(xe->ac);
	if (aindex < 0)
	{
		getchar();
		return -1;
	}

	///6 打开rtmp的网络输出IO
	// 写入封装头
	if (!xr->SendHead())
	{
		getchar();
		return -1;
	}


	//一次读取一帧音频的字节数
	for (;;)
	{
		//一次读取一帧音频
		XData ad = ar->Pop();
		XData vd = xv->Pop();
		if (ad.size <= 0 && vd.size <= 0)
		{
			QThread::msleep(1);
			continue;
		}

		//处理音频
		if (ad.size > 0)
		{
			//重采样源数据
			AVFrame *pcm = xe->Resample(ad.data);
			ad.Drop();
			AVPacket *pkt = xe->EncodeAudio(pcm);
			if (pkt)
			{
				//// 推流
				if (xr->SendFrame(pkt, aindex))
				{
					cout << "#" << flush;
				}
			}
			
		}

		//处理视频
		if (vd.size > 0)
		{
			AVFrame *yuv = xe->RGBToYUV(vd.data);
			vd.Drop();
			AVPacket *pkt = xe->EncodeVideo(yuv);
			if (pkt)
			{
				//// 推流
				if (xr->SendFrame(pkt, vindex))
				{
					cout << "@" << flush;
				}
			}
		}

		

	}
	

	getchar();
	return a.exec();
}
