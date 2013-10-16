#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CRGB2BMP.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/log.h"

#ifdef __cplusplus
}
#endif// end of extern C

#include "CFFmpegJni.h"

#include <android/log.h>

#define FFMPEG_TAG "FFMPEG_TAG"
#define MYLOG_TAG "TestNDK_DEBUG"

#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)

/// 定义全局变量
AVFormatContext* m_pFormatCtx = NULL;

AVDictionary* m_pDictOptions = NULL;

bool m_bIsPlaying = false;

int nDecodeNum = 0;

char m_szURLPath[512];
int m_nWith = 0;

static jclass		m_CallBackClass = NULL;
static jmethodID 	m_CallBackQueueMethod = NULL;

/// 定义日志回调函数
static void e_PrintFFmpeg(void* ptr, int level, const char* fmt, va_list vl)
{
	switch(level)
	{
	case AV_LOG_PANIC:
		__android_log_print(ANDROID_LOG_ERROR, FFMPEG_TAG, fmt, vl);
		break;

	case AV_LOG_FATAL:
		__android_log_print(ANDROID_LOG_ERROR, FFMPEG_TAG, fmt, vl);
		break;

	case AV_LOG_ERROR:
		__android_log_print(ANDROID_LOG_ERROR, FFMPEG_TAG, fmt, vl);
		break;

	case AV_LOG_WARNING:
		__android_log_print(ANDROID_LOG_ERROR, FFMPEG_TAG, fmt, vl);
		break;

	case AV_LOG_INFO:
		__android_log_print(ANDROID_LOG_INFO, FFMPEG_TAG, fmt, vl);
		break;

	case AV_LOG_DEBUG:
		__android_log_print(ANDROID_LOG_DEBUG, FFMPEG_TAG, fmt, vl);
		break;
	}
}

/// 定义打印信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, MYLOG_TAG, __VA_ARGS__)


/****
 * 回调数据方法
 */
void e_DisplayCallBack(JNIEnv *env, BYTE* pszBuffer, int nSize)
{
	m_CallBackClass = env->FindClass("com/example/testffmpeg/CVideoFrames");
	if(NULL != m_CallBackClass)
	{
		/// 再找类中的静态方法
		m_CallBackQueueMethod = env->GetStaticMethodID(m_CallBackClass, "e_AddDataToQueue", "([B)V");
		if(NULL != m_CallBackQueueMethod)
		{
			LOGD("Call BACK Data0..................");
		 	/// 构造数组
		 	jbyteArray byteArray = env->NewByteArray(nSize);
		    /// 拷贝数据
		 	env->SetByteArrayRegion(byteArray, 0, nSize, (jbyte* )pszBuffer);
		 	LOGD("Call BACK Data1..................");
		 	//回调java中的方法
		 	env->CallStaticVoidMethod(m_CallBackClass, m_CallBackQueueMethod , byteArray);
		 	/// 释放本地数组引用
		 	env->DeleteLocalRef(byteArray);
		 	LOGD("Call BACK Data2..................");
		}
	}
}

void e_SaveFrame(JNIEnv *env, AVFrame* pFrameRGB, int nWidth, int nHeight)
{
	/// 获取宽度的设置
	int nWidthBytes = nWidth * 4;/// WIDTHBYTES(nWidth * 24);
	int nBufferLen =  nWidthBytes * nHeight;

	LOGD("%s, DateLen = %d", "SaveFrame 0--------------->", nBufferLen);
	int nBMPHeadLen = sizeof(BmpHead) + sizeof(InfoHead);
	int nBMPBufferLen = nBMPHeadLen + nBufferLen;
	BYTE* pBMPData = new BYTE[nBMPBufferLen];
	memset(pBMPData, 0x00, nBMPBufferLen);
	/// 获取数据内容部分的指针
	BYTE* pBuffer = pBMPData + nBMPHeadLen;
	/// 将图像转为bmp存到内存中, 这里的图像是倒立的
	for(int i = 0; i < nHeight; i++)
	{
		memcpy(pBuffer + nWidthBytes * i, pFrameRGB->data[0] + i * pFrameRGB->linesize[0], nWidthBytes);
	}

	/// 赋值BMP图片头
    BmpHead m_BMPHeader;
    /// 高地位转换
    m_BMPHeader.bfType = ('M' << 8) | 'B';
    m_BMPHeader.imageSize = nBMPBufferLen;
    m_BMPHeader.blank = 0;
    m_BMPHeader.startPosition = sizeof(BmpHead) + sizeof(InfoHead);
    memcpy(pBMPData, &m_BMPHeader, sizeof(BmpHead));

    /// 赋值BMP图片信息头
    InfoHead  m_BMPInfoHeader;
    m_BMPInfoHeader.Length = sizeof(InfoHead);
    m_BMPInfoHeader.width = nWidth;
    /// = 注意，这里的bmpinfo.bmiHeader.biHeight 变量的正负
    /// = 决定bmp文件的存储方式，如果为负值，表示像素是倒过来的
    m_BMPInfoHeader.height = -nHeight;
    m_BMPInfoHeader.colorPlane = 1;
    m_BMPInfoHeader.bitColor = 32;
    m_BMPInfoHeader.zipFormat = 0;
    m_BMPInfoHeader.realSize = nBufferLen;
    m_BMPInfoHeader.xPels = 0;
    m_BMPInfoHeader.yPels = 0;
    m_BMPInfoHeader.colorUse = 0;
    m_BMPInfoHeader.colorImportant = 0;
    memcpy(pBMPData + sizeof(BmpHead), &m_BMPInfoHeader, sizeof(InfoHead));

    LOGD("SaveFrame 1--------------->");
	/// 回调数据到Java
	/// e_DisplayCallBack(env, pBMPData, nBMPBufferLen);

	FILE* pFile = NULL;
	char szFilename[32];
	int y;
	sprintf(szFilename, "/mnt/sdcard/Frame%d.bmp", nDecodeNum);
	pFile = fopen(szFilename, "wb");
	if(NULL == pFile)
	{
		return;
	}
	fwrite(pBMPData, 1, nBMPBufferLen, pFile);
	fclose(pFile);
    /// 释放数据缓冲
	delete[] pBMPData;
	pBMPData = NULL;
}

/*
 * Class:     com_example_testffmpeg_CFFmpegJni
 * Method:    IGetVersion
 * Signature: ()I
 */
jint Java_com_example_testffmpeg_CFFmpegJni_IGetVersion(JNIEnv *env, jobject thiz)
{
	/// 设置FFmpeg日志信息回调
	av_log_set_callback(e_PrintFFmpeg);
	LOGD("Set FFMpeg Log Call Back Success. ");
	/// 获取版本信息测试使用
	int nRet = 0;
	nRet = avcodec_version();
	/// 测试回调
	char szTemp[128];
	strcpy(szTemp, "哈哈, 这是回调消息");
	e_DisplayCallBack(env, (BYTE* )szTemp, strlen(szTemp));
	return nRet;
}

/*
 * Class:     com_example_testffmpeg_CFFmpegJni
 * Method:    IInit
 * Signature: (Ljava/lang/String;IIII)I
 */
jint Java_com_example_testffmpeg_CFFmpegJni_IInit(JNIEnv *env, jobject thiz, jstring jstrRTSPUrl,
		jint jnMeidaType, jint jnSawle, jint jnWith, jint jnHeight)
{
	int nRet = 0;
	/// 注册解码器
	av_register_all();
	/// 注册解码器
	avcodec_register_all();
	/// 获取RTSP路径
	const char* pstrRTSPUrl = (env)->GetStringUTFChars(jstrRTSPUrl, 0);
	/// 赋值RTSP网络地址
	strcpy(m_szURLPath, pstrRTSPUrl);
	m_nWith = jnWith;
	/// 设置rtsp为TCP方式
	if(1 == jnMeidaType)
	{
		av_dict_set(&m_pDictOptions, "rtsp_transport", "tcp", 0);
	}
	/// 初始化网络
	nRet = avformat_network_init();
	return nRet;
}

/*
 * Class:     com_example_testffmpeg_CFFmpegJni
 * Method:    IResize
 * Signature: (II)V
 */
void Java_com_example_testffmpeg_CFFmpegJni_IResize(JNIEnv *env, jobject thiz, jint jnWith, jint jnHeight)
{

}

/*
 * Class:     com_example_testffmpeg_CFFmpegJni
 * Method:    IPlay
 * Signature: ()I
 */
jint Java_com_example_testffmpeg_CFFmpegJni_IPlay(JNIEnv *env, jobject thiz)
{
	/// 定义返回值
	int nRet = -1;
	/// 打开文件
	if(NULL != m_pFormatCtx)
	{
		avformat_close_input(&m_pFormatCtx);
		/// 释放数据
		av_free(m_pFormatCtx);
		m_pFormatCtx = NULL;
	}

	if(NULL == m_pFormatCtx && NULL != m_pDictOptions)
	{
		/// 打开文件
		if(0 != (nRet = avformat_open_input(&m_pFormatCtx, m_szURLPath, 0, &m_pDictOptions)))
		{
			char szTemp[256];
			memset(szTemp, 0x00, sizeof(szTemp));
			av_strerror(nRet, szTemp, 255);
			/// 打印错误信息
			LOGD("%s, Error Code = %d, %s, Error = %s", m_szURLPath, nRet,
					" The Error URL Or Path--------------->", szTemp);
			return nRet;
		}
	}

	// m_pFormatCtx->max_analyze_duration = 1000;
	// m_pFormatCtx->probesize = 2048;
	if(0 > avformat_find_stream_info(m_pFormatCtx, NULL))
	{
		LOGD("Couldn't find stream information.");
		return -1;
	}

	int nVideoIndex = -1;
	for(int i = 0; i < m_pFormatCtx->nb_streams; i++)
	{
		if(AVMEDIA_TYPE_VIDEO == m_pFormatCtx->streams[i]->codec->codec_type)
		{
			nVideoIndex = i;
			break;
		}
	}
	if(-1 == nVideoIndex)
	{
		LOGD("Didn't find a video stream.");
		return -1;
	}

	AVCodecContext* pCodecCtx = m_pFormatCtx->streams[nVideoIndex]->codec;
	AVCodec* pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if(NULL == pCodec)
	{
		LOGD("Codec not found.");
		return -1;
	}

	LOGD("Test1 pCodecCtx Info Width:%d  Height:%d ------------>", pCodecCtx->width, pCodecCtx->height);
	if(0 > avcodec_open2(pCodecCtx, pCodec, &m_pDictOptions))
	{
		LOGD("Could not open codec.");
		return -1;
	}

	/// 声明数据帧变量
	AVFrame	*pFrame = NULL, *pFrameRGB = NULL;
	pFrame = avcodec_alloc_frame();
	pFrameRGB = avcodec_alloc_frame();

	LOGD("Test2 pCodecCtx Info Width:%d  Height:%d ------------>", pCodecCtx->width, pCodecCtx->height);
	/// 创建数据帧缓存
	uint8_t* pPicbuffer = NULL;
	pPicbuffer = new uint8_t[avpicture_get_size(PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height)];
	avpicture_fill((AVPicture *)pFrameRGB, pPicbuffer, PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height);

	/// 声明解码参数
	int nCodecRet, nHasGetPicture;
	/// 声明格式转换参数
	struct SwsContext* img_convert_ctx = NULL;
	LOGD("Test3 pCodecCtx Info Width:%d  Height:%d ------------>", pCodecCtx->width, pCodecCtx->height);
	/// 声明数据帧解码数据包
	int nPackgeSize  = pCodecCtx->width * pCodecCtx->height;
	AVPacket* pAVPacket = (AVPacket *)malloc(sizeof(AVPacket));
	av_new_packet(pAVPacket, nPackgeSize);

	/// 列出输出文件的相关流信息
	av_dump_format(m_pFormatCtx, 0, m_szURLPath, 0);
	/// 设置播放状态
	m_bIsPlaying = true;
	/// 读取数据帧
	while(0 <= av_read_frame(m_pFormatCtx, pAVPacket) && true == m_bIsPlaying)
	{
		/// 判断是否是视频数据流
		if(nVideoIndex == pAVPacket->stream_index)
		{
			/// 解码数据包
			nCodecRet = avcodec_decode_video2(pCodecCtx, pFrame, &nHasGetPicture, pAVPacket);
			if(0 < nHasGetPicture)
			{
				LOGD("Num:%d  Width:%d  Height:%d StreamNum:%d nCodecRet=%d...", nDecodeNum++, pCodecCtx->width,
						pCodecCtx->height, pFrame->coded_picture_number, nCodecRet);
				/// 格式化像素格式为RGB
				img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
					pCodecCtx->width, pCodecCtx->height, PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
				/// 转换格式为RGB
				sws_scale(img_convert_ctx, (const uint8_t* const* )pFrame->data, pFrame->linesize,
						0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
				/// 释放个格式化信息
				sws_freeContext(img_convert_ctx);
				/// 显示或者保存数据
				e_SaveFrame(env, pFrameRGB, pCodecCtx->width, pCodecCtx->height);
			}
		}
		/// 释放解码包，此数据包，在 av_read_frame 调用是被创建
		av_free_packet(pAVPacket);
	}

	/// 释放图片数据缓存
	delete[] pPicbuffer;
	pPicbuffer = NULL;
	/// 释放数据帧对象指针
	av_free(pFrame); pFrame = NULL;
	av_free(pFrameRGB); pFrameRGB = NULL;
	/// 释放解码信息对象
	avcodec_close(pCodecCtx); pCodecCtx = NULL;
	avformat_close_input(&m_pFormatCtx);
	/// 释放数据
	av_free(m_pFormatCtx);
	m_pFormatCtx = NULL;
	return nRet;
}

/*
 * Class:     com_example_testffmpeg_CFFmpegJni
 * Method:    IStop
 * Signature: ()I
 */
jint Java_com_example_testffmpeg_CFFmpegJni_IStop(JNIEnv *env, jobject thiz)
{
	/// 赋值停止播放标识
	m_bIsPlaying = false;
	return 0;
}
