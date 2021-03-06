#include <QtCore/QCoreApplication>
#include <QaudioInput>
#include <iostream>
#include <QThread>
#include "XMediaEncode.h"
#include "XRtmp.h"
using namespace std;
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	
	const char *outUrl = "rtmp://192.168.163.131/live";
	int ret = 0;
	int sampleRate = 44100;
	int channels = 2;
	int sampleByte = 2;

	///1 qt音频开始录制
	QAudioFormat fmt;
	fmt.setSampleRate(sampleRate);
	fmt.setChannelCount(channels);
	fmt.setSampleSize(sampleByte * 8);
	fmt.setCodec("audio/pcm");
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
	if (!info.isFormatSupported(fmt))
	{
		cout << "Audio format not support!" << endl;
		fmt = info.nearestFormat(fmt);
	}
	QAudioInput *input = new QAudioInput(fmt);
	//开始录制音频
	QIODevice *io = input->start();

	///2 音频重采样 上下文初始化
	XMediaEncode *xe = XMediaEncode::Get();
	xe->channels = channels;
	xe->nbSamples = 1024;
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

	///5 输出封装器和音频流配置
	// a 创建输出封装器上下文
	XRtmp *xr = XRtmp::Get(0);
	if (!xr->Init(outUrl))
	{
		getchar();
		return -1;
	}

	// b 添加音频流
	if (!xr->AddStream(xe->ac))
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
	int readSize = xe->nbSamples * channels * sampleByte;
	char *buf = new char[readSize];
	for (;;)
	{
		//一次读取一帧音频
		if (input->bytesReady() < readSize)
		{
			QThread::msleep(1);
			continue;
		}
		int size = 0;
		while (size != readSize)
		{
			int len = io->read(buf + size, readSize - size);
			if (len < 0) break;
			size += len;
		}

		if (size != readSize) continue;

		//已经读一帧源数据
		//重采样源数据
		AVFrame *pcm = xe->Resample(buf);
		
		//pts 运算
		// nb_sample / sample_rate = 一帧音频的秒数sec
		// timebase pts = sec * timebase.den
		AVPacket *pkt = xe->EncodeAudio(pcm);
		if (!pkt) continue;
		//cout << pkt->size << " " << flush;
		//// 推流
		xr->SendFrame(pkt);
		if (ret == 0)
		{
			cout << "#" << flush;
		}

	}
	delete buf;

	getchar();
	return a.exec();
}
