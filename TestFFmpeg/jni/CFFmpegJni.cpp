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

#define FFMPEG_TAG "FFMpegAndroid"

#define MYLOG_TAG "TestNDK_DEBUG"

#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)

/// 定义全局变量
AVFormatContext* m_pFormatCtx = NULL;

char m_szURLPath[512];
int m_nWith = 0;

jclass		m_CallBackClass = NULL;
jmethodID 	m_CallBackQueueMethod = NULL;

/// 定义日志回调函数
static void e_PrintFFmpeg(void* ptr, int level, const char* fmt, va_list vl)
{
	switch(level)
	{
	case AV_LOG_PANIC:
		__android_log_print(ANDROID_LOG_ERROR, FFMPEG_TAG, "AV_LOG_PANIC: %s", fmt);
		break;

	case AV_LOG_FATAL:
		__android_log_print(ANDROID_LOG_ERROR, FFMPEG_TAG, "AV_LOG_FATAL: %s", fmt);
		break;

	case AV_LOG_ERROR:
		__android_log_print(ANDROID_LOG_ERROR, FFMPEG_TAG, "AV_LOG_ERROR: %s", fmt);
		break;

	case AV_LOG_WARNING:
		__android_log_print(ANDROID_LOG_ERROR, FFMPEG_TAG, "AV_LOG_WARNING: %s", fmt);
		break;

	case AV_LOG_INFO:
		__android_log_print(ANDROID_LOG_INFO, FFMPEG_TAG, "AV_LOG_INFO: %s", fmt);
		break;

	case AV_LOG_DEBUG:
		__android_log_print(ANDROID_LOG_DEBUG, FFMPEG_TAG, "AV_LOG_DEBUG: %s", fmt);
		break;
	}
}

/// 定义打印信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, MYLOG_TAG, __VA_ARGS__)


void e_SaveFrame(AVFrame* pFrameRGB, int nWidth, int nHeight)
{
	int w = nWidth;
	int h = nHeight;
	int nBufferLen = WIDTHBYTES(w * 24) * h;
	BYTE* pBuffer = new BYTE[nBufferLen];
	memset(pBuffer, 0 , nBufferLen);

	for (int i = 0; i < h; i++)
	{
		//将图像转为bmp存到内存中, 这里的图像是倒立的
		memcpy(pBuffer + (WIDTHBYTES(w * 24) * i), pFrameRGB->data[0] + i * pFrameRGB->linesize[0], w * 3);
	}

	BYTE* pBMPData = new BYTE[sizeof(BmpHead) + sizeof(InfoHead) + nBufferLen];

    BmpHead m_BMPHeader;
    m_BMPHeader.bfType = 'BM';
    m_BMPHeader.imageSize = 3 * nWidth*nHeight + sizeof(BmpHead) + sizeof(InfoHead);
    m_BMPHeader.blank = 0;
    m_BMPHeader.startPosition=sizeof(BmpHead) + sizeof(InfoHead);
    memcpy(pBMPData, &m_BMPHeader, sizeof(BmpHead));

    InfoHead  m_BMPInfoHeader;
    m_BMPInfoHeader.Length=40;
    m_BMPInfoHeader.width= nWidth;
    //注意，这里的bmpinfo.bmiHeader.biHeight变量的正负决定bmp文件的存储方式，如果为负值，表示像素是倒过来的
    m_BMPInfoHeader.height= -nHeight;
    m_BMPInfoHeader.colorPlane = 1;
    m_BMPInfoHeader.bitColor=24;
    m_BMPInfoHeader.zipFormat=0;
    m_BMPInfoHeader.realSize=3 * nWidth * nHeight;
    m_BMPInfoHeader.xPels=0;
    m_BMPInfoHeader.yPels=0;
    m_BMPInfoHeader.colorUse=0;
    m_BMPInfoHeader.colorImportant=0;
    memcpy(pBMPData+sizeof(BmpHead),&m_BMPInfoHeader,sizeof(InfoHead));
    memcpy(pBMPData+sizeof(BmpHead)+sizeof(InfoHead), pBuffer, nBufferLen);

    /// 释放数据缓冲
	delete [] pBuffer; pBuffer = NULL;
	delete [] pBMPData; pBMPData = NULL;
}

//---------------------------------------------------------------

void e_DisplayCallBack(JNIEnv *env, char* pszBuffer, int nSize)
{
	/*m_CallBackClass = env->FindClass("com/example/testffmpeg/CVideoFrames");
	if(NULL != m_CallBackClass)
	{
		/// 再找类中的静态方法
		m_CallBackQueueMethod = env->GetStaticMethodID(m_CallBackClass, "e_AddDataToQueue", "([B)V");
		if(NULL != m_CallBackQueueMethod)
		{
		 	/// 构造数组
		 	jbyteArray bytecarr = env->NewByteArray(nSize);
		     /// 拷贝数据
		 	env->SetByteArrayRegion(bytecarr, 0, nSize, (jbyte*)pszBuffer);
		 	//回调java中的方法
		 	env->CallStaticVoidMethod(m_CallBackClass, m_CallBackQueueMethod , bytecarr);
		}
	}*/

	if(NULL != m_CallBackClass && NULL != m_CallBackQueueMethod)
	{
	 	/// 构造数组
	 	jbyteArray bytecarr = env->NewByteArray(nSize);
	     /// 拷贝数据
	 	env->SetByteArrayRegion(bytecarr, 0, nSize, (jbyte*)pszBuffer);
	 	//回调java中的方法
	 	env->CallStaticVoidMethod(m_CallBackClass, m_CallBackQueueMethod , bytecarr);
	 	/// 释放本地数组引用
	 	env->DeleteLocalRef(bytecarr);
	}
}

/*
 * Class:     com_example_testffmpeg_CFFmpegJni
 * Method:    IGetVersion
 * Signature: ()I
 */
jint Java_com_example_testffmpeg_CFFmpegJni_IGetVersion(JNIEnv *env, jobject thiz)
{
	/// 设置日志信息回调
	av_log_set_callback(e_PrintFFmpeg);

	/// 打印日志
	LOGD("Set FFMpeg Log Call Back Success. ");

	/// 获取版本信息测试使用
	int nRet = 0;
	nRet = avcodec_version();

	/// 如果回调函数没有设置，那么获取回调函数
	if(NULL == m_CallBackQueueMethod)
	{
	    //找到java中的类
		m_CallBackClass = env->FindClass("com/example/testffmpeg/CVideoFrames");
		/// JVM的局部引用改成全局引用
		m_CallBackClass = env->NewGlobalRef(m_CallBackClass);

		if(NULL != m_CallBackClass)
		{
			/// 再找类中的静态方法
			m_CallBackQueueMethod = env->GetStaticMethodID(m_CallBackClass, "e_AddDataToQueue", "([B)V");
			/// JVM的局部引用改成全局引用
			m_CallBackQueueMethod = env->NewGlobalRef(m_CallBackQueueMethod);
		}
	}

	/// 判断回调方法是否获取成功
    if(NULL == m_CallBackQueueMethod || NULL == m_CallBackClass)
    {
        LOGD("Get CallBack Function Fail.");
        return 0;
    }

	/// 测试回调
	char szTemp[128];
	strcpy(szTemp, "哈哈, 这是回调消息");
	e_DisplayCallBack(env, szTemp, strlen(szTemp));
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
	avcodec_register_all();

	const char* pstrRTSPUrl = (env)->GetStringUTFChars(jstrRTSPUrl, 0);

	/// 赋值RTSP网络地址
	strcpy(m_szURLPath, pstrRTSPUrl);

	LOGD("%s", m_szURLPath);

	m_nWith = jnWith;

	/// 设置rtsp为TCP方式
	if(1 == jnMeidaType)
	{
		av_dict_set(NULL, "rtsp_transport", "tcp", 0);
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
	if(NULL == m_pFormatCtx)
	{
		m_pFormatCtx = avformat_alloc_context();
	}

	if(NULL != m_pFormatCtx)
	{
		/// 打开文件
		nRet = avformat_open_input(&m_pFormatCtx, m_szURLPath, 0, NULL);
	}

	if(0 > avformat_find_stream_info(m_pFormatCtx, NULL))
	{
		LOGD("Couldn't find stream information.");
		return -1;
	}

	int i = 0, nVideoIndex = -1;
	for(i = 0; i < m_pFormatCtx->nb_streams; i++)
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
	if(pCodec==NULL)
	{
		LOGD("Codec not found.");
		return -1;
	}
	if(0 > avcodec_open2(pCodecCtx, pCodec, NULL))
	{
		LOGD("Could not open codec.");
		return -1;
	}

	AVFrame	*pFrame = NULL, *pFrameYUV = NULL, *pFrameRGB = NULL;
	pFrame = avcodec_alloc_frame();
	pFrameYUV = avcodec_alloc_frame();
	pFrameRGB = avcodec_alloc_frame();
	uint8_t *out_buffer = NULL;
	out_buffer = new uint8_t[avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height)];
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

	int nCodecRet, got_picture;
	static struct SwsContext *img_convert_ctx;
	int y_size = pCodecCtx->width * pCodecCtx->height;

	AVPacket* packet = (AVPacket *)malloc(sizeof(AVPacket));
	av_new_packet(packet, y_size);
	/// 输出一下信息-----------------------------
	LOGD("文件信息-----------------------------------------");
	av_dump_format(m_pFormatCtx, 0, m_szURLPath, 0);
	while(0 <= av_read_frame(m_pFormatCtx, packet))
	{
		if(nVideoIndex == packet->stream_index)
		{
			nCodecRet = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if(0 < nCodecRet)
			{
				/// 格式化像素为YUV格式
				/*img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width,
						pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize,
						0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);*/

				/// 格式化像素格式为BMP
				img_convert_ctx = sws_getContext(	pCodecCtx->width,
						pCodecCtx->height,
						pCodecCtx->pix_fmt,
						pCodecCtx->width,
						pCodecCtx->height,
						PIX_FMT_RGB32,
						SWS_BICUBIC,
						NULL, NULL, NULL);
				/// 转换格式为RGB
				sws_scale(img_convert_ctx,
						(const uint8_t* const*)pFrame->data,
						pFrame->linesize,
						0,
						pCodecCtx->height,
						pFrameRGB->data,
						pFrameRGB->linesize);

				/// 释放个格式化信息
				sws_freeContext(img_convert_ctx);


				e_SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height);
			}
			else
			{
				LOGD("解码错误-------------------------->");
				return -1;
			}
		}
		/// 释放解码包
		av_free_packet(packet);
	}

	delete[] out_buffer; out_buffer = NULL;
	av_free(pFrame); pFrame = NULL;
	av_free(pFrameYUV); pFrameYUV = NULL;
	av_free(pFrameRGB); pFrameRGB = NULL;
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
	/// 关闭打开的文件
	avformat_close_input(&m_pFormatCtx);
	/// 释放数据
	av_free(m_pFormatCtx);
	m_pFormatCtx = NULL;
	return 0;
}
