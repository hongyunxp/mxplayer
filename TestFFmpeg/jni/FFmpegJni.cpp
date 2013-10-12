#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/log.h"

#ifdef __cplusplus
}
#endif// end of extern C

#include "FFmpegJni.h"

#include <android/log.h>

#define FFMPEG_TAG "FFMpegAndroid"

#define MYLOG_TAG "TestNDK_DEBUG"

/// 定义全局变量
AVFormatContext* m_pFormatCtx = NULL;
AVDictionary* m_pDOptions = NULL;

char m_szURLPath[512];
int m_nWith = 0;

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


void SaveFrame(AVFrame* pFrame, int width, int height, int iFrame)
{
    FILE *pFile;
    char szFilename[32];
    int  y;

	/////////////////////
	BITMAPFILEHEADER m_fileHeader;
	BITMAPINFOHEADER m_infoHeader;
	unsigned int uiTmp, uiTmp2;
	unsigned char *ucTmp = NULL;
	unsigned char ucRGB;
	int i;

	uiTmp = (width*3+3)/4*4*height;
	uiTmp2 = width*height*3;


	//文件标识"BM"（即0x4D42）表示位图
	m_fileHeader.bfType = 0x4D42;
	//整个文件的大小（单位：字节）
	m_fileHeader.bfSize = sizeof(m_fileHeader) + sizeof(m_infoHeader) + uiTmp;
	//保留。设置为0
	m_fileHeader.bfReserved1 = 0;
	//保留。设置为0
	m_fileHeader.bfReserved2 = 0;
	//从文件开始到位图数据的偏移量（单位：字节）
	m_fileHeader.bfOffBits = sizeof(m_fileHeader) + sizeof(m_infoHeader);

	//信息头长度（单位：字节）。典型值为28
	m_infoHeader.biSize = 0x28;
	//位图宽度（单位：像素）
	m_infoHeader.biWidth = width;
	//位图高度（单位：像素）。若其为正，表示倒向的位图。若为负，表示正向的位图
	m_infoHeader.biHeight = height;
	//位图的面数（为1）
	m_infoHeader.biPlanes = 1;
	//每个像素的位数
	m_infoHeader.biBitCount = 24;
	//压缩说明。0(BI_RGB)表示不压缩
	m_infoHeader.biCompression = 0;
	//用字节数表示的位图数据的大小（为4的位数）
	m_infoHeader.biSizeImage = uiTmp;
	//水平分辨率（单位：像素/米）
	m_infoHeader.biXPelsPerMeter = 0;
	//垂直分辨率（单位：像素/米）
	m_infoHeader.biYPelsPerMeter = 0;
	//位图使用的颜色数
	m_infoHeader.biClrUsed = 0;
	//重要的颜色数
	m_infoHeader.biClrImportant = 0;
	/////////////////////

    // Open file
	sprintf(szFilename, "frame%d.bmp", iFrame);
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
        return;
    // Write header
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);
    // Write pixel data
	fwrite(&m_fileHeader, sizeof(m_fileHeader), 1, pFile);
	fwrite(&m_infoHeader, sizeof(m_infoHeader), 1, pFile);
    for(y=height-1; y>=0; y--)
    {
		if(ucTmp != NULL) {
			delete []ucTmp;
			ucTmp = NULL;
		}
		ucTmp = new unsigned char[width*3];
		memcpy(ucTmp, pFrame->data[0]+y*pFrame->linesize[0], width*3);
		for(i = 0; i < width; i++) {
			ucRGB = ucTmp[3*i];
			ucTmp[3*i] = ucTmp[3*i+2];
			ucTmp[3*i+2] = ucRGB;
		}
		ucRGB = 0;
        fwrite(ucTmp, 1, width*3, pFile);
		fwrite(&ucRGB, 1, (uiTmp-uiTmp2)/height , pFile);
	}
    // Close file
    // fclose(pFile);
}

/*
 * Class:     com_example_testffmpeg_FFmpegJni
 * Method:    IGetVersion
 * Signature: ()I
 */
jint Java_com_example_testffmpeg_FFmpegJni_IGetVersion(JNIEnv *env, jobject thiz)
{
	/// 设置日志信息回调
	av_log_set_callback(e_PrintFFmpeg);

	/// 打印日志
	LOGD("Set FFMpeg Log Call Back Success. ");

	/// 获取版本信息测试使用
	int nRet = avcodec_version();

	return nRet;
}

/*
 * Class:     com_example_testffmpeg_FFmpegJni
 * Method:    IInit
 * Signature: (Ljava/lang/String;IIII)I
 */
jint Java_com_example_testffmpeg_FFmpegJni_IInit(JNIEnv *env, jobject thiz, jstring jstrRTSPUrl,
		jint jnMeidaType, jint jnSawle, jint jnWith, jint jnHeight)
{
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
		av_dict_set(&m_pDOptions, "rtsp_transport", "tcp", 0);
	}
	/// 初始化网络
	int nRet = avformat_network_init();
	return nRet;
}

/*
 * Class:     com_example_testffmpeg_FFmpegJni
 * Method:    IResize
 * Signature: (II)V
 */
void Java_com_example_testffmpeg_FFmpegJni_IResize(JNIEnv *env, jobject thiz, jint jnWith, jint jnHeight)
{

}

/*
 * Class:     com_example_testffmpeg_FFmpegJni
 * Method:    IPlay
 * Signature: ()I
 */
jint Java_com_example_testffmpeg_FFmpegJni_IPlay(JNIEnv *env, jobject thiz)
{
	/// 定义返回值
	int nRet = -1;
	/// 打开文件
	if(NULL == m_pFormatCtx)
	{
		m_pFormatCtx = avformat_alloc_context();

		if(NULL != m_pFormatCtx && NULL != m_pDOptions)
		{
			/// 打开文件
			nRet = avformat_open_input(&m_pFormatCtx, m_szURLPath, 0, &m_pDOptions);
		}

		if(0 > avformat_find_stream_info(m_pFormatCtx, &m_pDOptions))
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
		if(0 > avcodec_open2(pCodecCtx, pCodec, &m_pDOptions))
		{
			LOGD("Could not open codec.");
			return -1;
		}

		AVFrame	*pFrame,*pFrameYUV;
		pFrame = avcodec_alloc_frame();
		pFrameYUV = avcodec_alloc_frame();
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

		while(av_read_frame(m_pFormatCtx, packet)>=0)
		{
			if(nVideoIndex == packet->stream_index)
			{
				nCodecRet = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
				if(0 < nCodecRet)
				{
					/// 格式化像素为YUV格式
					img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width,
							pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
					sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize,
							0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);


					/*img_convert_ctx = sws_getContext(	pAVCodecCtx->width,
														pAVCodecCtx->height,
														pAVCodecCtx->pix_fmt,
														pAVCodecCtx->width,
														pAVCodecCtx->height,
														PIX_FMT_RGB24,
														SWS_BICUBIC,
														NULL, NULL, NULL);
					if(img_convert_ctx == NULL)
						LOGE("could not initialize conversion context\n");

					sws_scale(	img_convert_ctx,
								(const uint8_t* const*)pAVFrame->data,
								pAVFrame->linesize,
								0,
								pAVCodecCtx->height,
								pAVFrameRGB->data,
								pAVFrameRGB->linesize);*/
				}
				else
				{
					LOGD("解码错误-------------------------->");
					return -1;
				}
			}
		}
	}
	return nRet;
}

/*
 * Class:     com_example_testffmpeg_FFmpegJni
 * Method:    IStop
 * Signature: ()I
 */
jint Java_com_example_testffmpeg_FFmpegJni_IStop(JNIEnv *env, jobject thiz)
{
	/// 关闭打开的文件
	avformat_close_input(&m_pFormatCtx);
	return 0;
}
