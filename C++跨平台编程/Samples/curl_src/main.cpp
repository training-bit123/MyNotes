#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <curl/curl.h>
using namespace std;

#ifdef _WIN64
#pragma comment(lib, "libcurl_x64.lib")
#else
#pragma comment(lib, "libcurl_x86.lib")
#endif // _WIN64
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib,"msvcrtd.lib")


static size_t downloadCallback(void *buffer, size_t sz, size_t nmemb, void *writer)
{
	string* psResponse = (string*)writer;
	size_t size = sz * nmemb;
	psResponse->append((char*)buffer, size);

	return sz * nmemb;
}

int main(int argc, char **argv)
{

	string strUrl = "http://www.baidu.com";
	string strTmpStr;
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strTmpStr);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	string strRsp;
	if (res != CURLE_OK)
	{
		strRsp = "error";
	}
	else
	{
		strRsp = strTmpStr;
	}

	printf("strRsp is |%s|\n", strRsp.c_str());

	getchar();
	return 0;
}