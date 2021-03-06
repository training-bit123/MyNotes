#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
extern "C"
{
	#include <libavformat/avformat.h>
}
using namespace std;
#ifdef _MSC_VER // Windows
	#pragma comment(lib, "avformat.lib")
	#pragma comment(lib, "avutil.lib")
	#pragma comment(lib, "avcodec.lib")
#elif __GNUC__ // Linux

#endif

void print_err(int& re)
{
	if (0 != re)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf));
		cout << buf << endl;
		getchar();
	}
}

int main()
{
	const char* inUrl = "C:/Users/Dejan/Videos/test.flv";
	const char* outUrl = "rtmp://192.168.163.131/live";
	// 初始化所有的封装和解封装 flv mp4 mov mp3 ..
	av_register_all();

	// 初始化网络库
	avformat_network_init();
	///////////////////////////////////////////////////////////
	// 1.打开文件, 解封装
	// 输入封装上下文
	AVFormatContext *ictx = NULL;

	// 打开文件, 解封文件头
	int re = avformat_open_input(&ictx, inUrl, 0, 0);
	if (0 != re)
	{
		print_err(re);
		return -1;
	}
	cout << "open file " << inUrl << "Success" << endl;

	// 获取音频视频流信息, h264 flv
	re = avformat_find_stream_info(ictx, 0);
	if (0 != re)
	{
		print_err(re);
		return -1;
	}

	av_dump_format(ictx, 0, inUrl, 0);
	//////////////////////////////////////////////////////

	//////////////////////////////////////////////////////
	// 输出流

	// 创建输出流上下文
	AVFormatContext *octx = NULL;
	avformat_alloc_output_context2(&octx, 0, "flv", outUrl);
	if (!octx)
	{
		print_err(re);
		return -1;
	}
	cout << "octx create success!" << endl;

	// 配置输出流
	// 遍历输入的AVStream
	for (int i = 0; i < ictx->nb_streams; i++)
	{
		// 创建输出流
		AVStream *out = avformat_new_stream(octx, ictx->streams[i]->codec->codec);
		if (!out)
		{
			printf("error: avformat_new_stream");
			return -1;
		}
		// -复制配置信息, 用于MP4
		//re = avcodec_copy_context(out->codec, ictx->streams[i]->codec);
		// -End
		re = avcodec_parameters_copy(out->codecpar, ictx->streams[i]->codecpar);
		out->codec->codec_tag = 0;
	}
	av_dump_format(octx, 0, outUrl, 1);


	//////////////////////////////////////////////////////

	cout << "file to rtmp test" << endl;
	getchar();
	return 0;
}