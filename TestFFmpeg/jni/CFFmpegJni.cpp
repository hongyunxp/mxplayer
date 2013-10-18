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

int iWidth = 0, iHeight = 0;

int *colortab=NULL;
int *u_b_tab=NULL;
int *u_g_tab=NULL;
int *v_g_tab=NULL;
int *v_r_tab=NULL;

unsigned int *rgb_2_pix=NULL;
unsigned int *r_2_pix=NULL;
unsigned int *g_2_pix=NULL;
unsigned int *b_2_pix=NULL;

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
		 	/// 构造数组
		 	jbyteArray byteArray = env->NewByteArray(nSize);
		    /// 拷贝数据
		 	env->SetByteArrayRegion(byteArray, 0, nSize, (jbyte* )pszBuffer);
		 	/// 回调java中的方法
		 	env->CallStaticVoidMethod(m_CallBackClass, m_CallBackQueueMethod , byteArray);
		 	/// 释放本地数组引用
		 	env->DeleteLocalRef(byteArray);
		}
	}
}

void e_SaveFrameToBMP(JNIEnv *env, AVFrame* pFrameRGB, int nWidth, int nHeight)
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
    /*LOGD("SaveFrame 1--------------->");
	/// 回调数据到Java
	e_DisplayCallBack(env, pBMPData, nBMPBufferLen);

	LOGD("SaveFrame 2--------------->");*/
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
	LOGD("SaveFrame finshed--------------->");
}


void e_Decode_YUV420_ARGB888(JNIEnv* env, BYTE* pYUVBuffer, int nWidth, int nHeight)
{
	int nRGBSize = nWidth * nHeight;
	/// 新图像像素值
	int nArrayBuffer[nRGBSize];

	int i = 0, j = 0, yp = 0;
	int uvp = 0, u = 0, v = 0;
	for (j = 0, yp = 0; j < nHeight; j++)
	{
		uvp = nRGBSize + (j >> 1) * nWidth;
		u = 0;
		v = 0;

		for (i = 0; i < nWidth; i++, yp++)
		{
			int y = (0xff & ((int) pYUVBuffer[yp])) - 16;
			if (y < 0)
				y = 0;
			if ((i & 1) == 0)
			{
				v = (0xff & pYUVBuffer[uvp++]) - 128;
				u = (0xff & pYUVBuffer[uvp++]) - 128;
			}

			int y1192 = 1192 * y;
			int r = (y1192 + 1634 * v);
			int g = (y1192 - 833 * v - 400 * u);
			int b = (y1192 + 2066 * u);

			if (r < 0) r = 0; else if (r > 262143) r = 262143;
			if (g < 0) g = 0; else if (g > 262143) g = 262143;
			if (b < 0) b = 0; else if (b > 262143) b = 262143;

			nArrayBuffer[yp] = 0xff000000 | ((r << 6) & 0xff0000) | ((g >> 2) & 0xff00) | ((b >> 10) & 0xff);
		}
	}
	/// 回调数据
	e_DisplayCallBack(env, (BYTE* )nArrayBuffer, nRGBSize * sizeof(int));
}

int g_v_table[256],g_u_table[256],y_table[256];
int r_yv_table[256][256],b_yu_table[256][256];
int inited = 0;

void initTable()
{
	if(inited == 0)
	{
		inited = 1;
		int m = 0,n=0;
		for (; m < 256; m++)
		{
			g_v_table[m] = 833 * (m - 128);
			g_u_table[m] = 400 * (m - 128);
			y_table[m] = 1192 * (m - 16);
		}
		int temp = 0;
		for (m = 0; m < 256; m++)
		{
			for (n = 0; n < 256; n++)
			{
				temp = 1192 * (m - 16) + 1634 * (n - 128);
				if (temp < 0) temp = 0; else if (temp > 262143) temp = 262143;
				r_yv_table[m][n] = temp;

				temp = 1192 * (m - 16) + 2066 * (n - 128);
				if (temp < 0) temp = 0; else if (temp > 262143) temp = 262143;
				b_yu_table[m][n] = temp;
			}
		}
	}
}

void DeleteYUVTab()
{
	av_free(colortab);
	colortab = NULL;

	av_free(rgb_2_pix);
	rgb_2_pix = NULL;
	inited = 0;
}


void CreateYUVTab_16()
{
	if(inited == 0)
	{
		int i;
		int u, v;

		colortab = (int *)av_malloc(4*256*sizeof(int));
		u_b_tab = &colortab[0*256];
		u_g_tab = &colortab[1*256];
		v_g_tab = &colortab[2*256];
		v_r_tab = &colortab[3*256];

		for (i=0; i<256; i++)
		{
			u = v = (i-128);

			u_b_tab[i] = (int) ( 1.772 * u);
			u_g_tab[i] = (int) ( 0.34414 * u);
			v_g_tab[i] = (int) ( 0.71414 * v);
			v_r_tab[i] = (int) ( 1.402 * v);
		}

		rgb_2_pix = (unsigned int *)av_malloc(3*768*sizeof(unsigned int));

		r_2_pix = &rgb_2_pix[0*768];
		g_2_pix = &rgb_2_pix[1*768];
		b_2_pix = &rgb_2_pix[2*768];

		for(i=0; i<256; i++)
		{
			r_2_pix[i] = 0;
			g_2_pix[i] = 0;
			b_2_pix[i] = 0;
		}

		for(i=0; i<256; i++)
		{
			r_2_pix[i+256] = (i & 0xF8) << 8;
			g_2_pix[i+256] = (i & 0xFC) << 3;
			b_2_pix[i+256] = (i ) >> 3;
		}

		for(i=0; i<256; i++)
		{
			r_2_pix[i+512] = 0xF8 << 8;
			g_2_pix[i+512] = 0xFC << 3;
			b_2_pix[i+512] = 0x1F;
		}

		r_2_pix += 256;
		g_2_pix += 256;
		b_2_pix += 256;
	}
}

void DisplayYUV_16(unsigned int* pdst1, unsigned char* y, unsigned char* u, unsigned char* v,
		int width, int height, int src_ystride, int src_uvstride, int dst_ystride)
{
	int i, j;
	int r, g, b, rgb;

	int yy, ub, ug, vg, vr;

	unsigned char* yoff;
	unsigned char* uoff;
	unsigned char* voff;

	unsigned int* pdst=pdst1;

	int width2 = width/2;
	int height2 = height/2;

	if(width2>iWidth/2)
	{
		width2=iWidth/2;

		y+=(width-iWidth)/4*2;
		u+=(width-iWidth)/4;
		v+=(width-iWidth)/4;
	}

	if(height2>iHeight)
		height2=iHeight;

	for(j=0; j<height2; j++) // 一次2x2共四个像素
	{
		yoff = y + j * 2 * src_ystride;
		uoff = u + j * src_uvstride;
		voff = v + j * src_uvstride;

		for(i=0; i<width2; i++)
		{
			yy  = *(yoff+(i<<1));
			ub = u_b_tab[*(uoff+i)];
			ug = u_g_tab[*(uoff+i)];
			vg = v_g_tab[*(voff+i)];
			vr = v_r_tab[*(voff+i)];

			b = yy + ub;
			g = yy - ug - vg;
			r = yy + vr;

			rgb = r_2_pix[r] + g_2_pix[g] + b_2_pix[b];

			yy = *(yoff+(i<<1)+1);
			b = yy + ub;
			g = yy - ug - vg;
			r = yy + vr;

			pdst[(j*dst_ystride+i)] = (rgb)+((r_2_pix[r] + g_2_pix[g] + b_2_pix[b])<<16);

			yy = *(yoff+(i<<1)+src_ystride);
			b = yy + ub;
			g = yy - ug - vg;
			r = yy + vr;

			rgb = r_2_pix[r] + g_2_pix[g] + b_2_pix[b];

			yy = *(yoff+(i<<1)+src_ystride+1);
			b = yy + ub;
			g = yy - ug - vg;
			r = yy + vr;

			pdst [((2*j+1)*dst_ystride+i*2)>>1] = (rgb)+((r_2_pix[r] + g_2_pix[g] + b_2_pix[b])<<16);
		}
	}
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
	int nRet = avcodec_version();
	return nRet;
}

/*
 * Class:     com_example_testffmpeg_CFFmpegJni
 * Method:    IInit
 * Signature: (Ljava/lang/String;IIII)I
 */
jint Java_com_example_testffmpeg_CFFmpegJni_IInit(JNIEnv *env, jobject thiz, jstring jstrRTSPUrl, jint jnMeidaType)
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
	/// 设置rtsp为TCP方式
	if(1 == jnMeidaType)
	{
		av_dict_set(&m_pDictOptions, "rtsp_transport", "tcp", 0);
	}
	/// 初始化网络
	nRet = avformat_network_init();

	CreateYUVTab_16();
	return nRet;
}

/*
 * Class:     com_example_testffmpeg_CFFmpegJni
 * Method:    IResize
 * Signature: (II)V
 */
void Java_com_example_testffmpeg_CFFmpegJni_IResize(JNIEnv *env, jobject thiz, jint jnWith, jint jnHeight)
{
	/// 设置显示的大小
	iWidth = jnWith;
	iHeight = jnHeight;
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

	if(0 > avcodec_open2(pCodecCtx, pCodec, NULL))
	{
		LOGD("Could not open codec.");
		return -1;
	}

	/// 声明数据帧变量
	AVFrame	*pFrame = NULL, *pFrameYUV = NULL;
	pFrame = avcodec_alloc_frame();
	pFrameYUV = avcodec_alloc_frame();
	/// 创建数据帧缓存
	int nPicDataSize = iWidth * iHeight * 2;
	uint8_t* pPicbuffer = new uint8_t[nPicDataSize];
	memset(pPicbuffer, 0x00, nPicDataSize);

	//	int nConvertSize = avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	//	uint8_t* pConvertbuffer = new uint8_t[nConvertSize];
	//	avpicture_fill((AVPicture *)pFrameYUV, pConvertbuffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

	/// 声明解码参数
	int nCodecRet, nHasGetPicture;
	/// 声明格式转换参数
	struct SwsContext* img_convert_ctx = NULL;
	LOGD("Test3 pCodecCtx Info Width:%d  Height:%d ------------>",
			pCodecCtx->width, pCodecCtx->height);
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
				/// 格式化像素格式为YUV
				/*img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
					pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
				/// 转换格式为YUV
				sws_scale(img_convert_ctx, (const uint8_t* const* )pFrame->data, pFrame->linesize,
						0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
				/// 释放个格式化信息
				sws_freeContext(img_convert_ctx);
				/// 显示或者保存数据
				/// e_SaveFrame(env, pFrameRGB, pCodecCtx->width, pCodecCtx->height);
				/// e_Decode_YUV420_ARGB888(env, pPicbuffer, pCodecCtx->width, pCodecCtx->height);

				/*img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
						pCodecCtx->width, pCodecCtx->height, PIX_FMT_RGB565LE, SWS_BICUBIC, NULL, NULL, NULL);
				/// 转换格式为YUV
				sws_scale(img_convert_ctx, (const uint8_t* const* )pFrame->data, pFrame->linesize,
						0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);*/

				DisplayYUV_16((unsigned int * )pPicbuffer, pFrame->data[0],
						pFrame->data[1], pFrame->data[2],
						pCodecCtx->width, pCodecCtx->height,
						pFrame->linesize[0], pFrame->linesize[1], iWidth);

				e_DisplayCallBack(env, pPicbuffer, nPicDataSize);
			}
		}
//		else
//		{
//			LOGD("stream_index = %d, The Frame Is Not Video Frame, May be It's Audio Frame ?...", pAVPacket->stream_index);
//		}
		/// 释放解码包，此数据包，在 av_read_frame 调用时被创建
		av_free_packet(pAVPacket);
	}

	/// 释放图片数据缓存
	delete[] pPicbuffer;
	pPicbuffer = NULL;
	/// 释放YUV表
	DeleteYUVTab();
	/// 释放转换图片缓存
	/// delete[] pConvertbuffer;
	/// pConvertbuffer = NULL;
	/// 释放数据帧对象指针
	av_free(pFrame); pFrame = NULL;
	av_free(pFrameYUV); pFrameYUV = NULL;

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
