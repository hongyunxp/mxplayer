#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)




void H264_Init()
{
	av_register_all();
}



BOOL YUVtoRGB(AVFrame * pAVFrame, int width, int height, char *filename)
{
	struct SwsContext *img_convert_ctx;
	strcat(filename, ".bmp");
	AVFrame *pAVFrameRGB;
	pAVFrameRGB = avcodec_alloc_frame();
	if (pAVFrameRGB == NULL)
	{
		return FALSE;
	}
	int numBytes = avpicture_get_size(PIX_FMT_RGB24, width, height);
	uint8_t  *buffer = (UINT8 *)av_malloc(numBytes*sizeof(uint8_t));

	avpicture_fill((AVPicture*)pAVFrameRGB, buffer, PIX_FMT_RGB24, width, height);

	img_convert_ctx = sws_getContext(
		width, height, PIX_FMT_YUVJ420P, width, height, PIX_FMT_RGB24,
		SWS_BICUBIC, NULL, NULL, NULL);
	sws_scale(img_convert_ctx, pAVFrame->data, pAVFrame->linesize,
		0, height, pAVFrameRGB->data, pAVFrameRGB->linesize);
	sws_freeContext(img_convert_ctx);
	av_free(buffer);

	DWORD w = width;
	DWORD h = height;
	DWORD BufferLen = WIDTHBYTES(w*24)*h;
	BYTE *pBuffer = new BYTE[BufferLen];
	memset(pBuffer, 0 , BufferLen);

	for (int i=0; i<height; i++)
	{
		//将图像转为bmp存到内存中,这里的图像是倒立的
		memcpy(pBuffer +(WIDTHBYTES(w*24)*i),pAVFrameRGB->data[0]+i*pAVFrameRGB->linesize[0],width*3);
	}

	LPBYTE pbmp = new BYTE[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPFILEHEADER)+BufferLen];

	BITMAPFILEHEADER bitmapfileheader;
	memset( &bitmapfileheader, 0, sizeof( BITMAPFILEHEADER ) );     
	bitmapfileheader.bfType = 'MB';    
	bitmapfileheader.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+BufferLen ;   
	bitmapfileheader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);    
	memcpy(pbmp,&bitmapfileheader,sizeof(BITMAPFILEHEADER));    

	BITMAPINFOHEADER fmtFrame; memset(&fmtFrame, 0, sizeof(fmtFrame));     
	fmtFrame.biSize = sizeof(fmtFrame);  
	fmtFrame.biPlanes  = 1;  
	fmtFrame.biBitCount = 24;  
	fmtFrame.biWidth =   w;   
	fmtFrame.biHeight =  -h;//注意，这里的bmpinfo.bmiHeader.biHeight变量的正负决定bmp文件的存储方式，如果为负值，表示像素是倒过来的*/
	fmtFrame.biSizeImage = BufferLen;  
	memcpy(pbmp+sizeof(BITMAPFILEHEADER),&fmtFrame,sizeof(BITMAPINFOHEADER));   
	memcpy(pbmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER),pBuffer, BufferLen);

	FILE * f = fopen(filename,"w+b");
	fwrite(pbmp,1,sizeof(BITMAPFILEHEADER)+sizeof(fmtFrame)+BufferLen, f );  
	fclose(f) ; 

	delete [] pbmp; pbmp = NULL;
	delete [] pBuffer; pBuffer = NULL; 
	return TRUE;
}
//void log_picture(AVPicture *pic, int width, int height, char *filename,int pixel_format);

BOOL H264_Decode(const PBYTE pSrcData, const DWORD dwDataSize, char *pFileName, int height, int width)
{
	AVFormatContext *pFormatCtx = NULL;
	AVCodecContext        *pCodecCtx = NULL;
	AVCodec                        *pCodec = NULL;
	//AVOutputFormat        fmt;
	AVFrame                        *pFrame = NULL;
	AVPicture                *pPic = NULL;
	AVFrame                        *pFrameRGB = NULL;
	AVPacket                pkt;
	int                                numBytes;
	BOOL                        nGot;
	int                                deCodePicSize;

	//初始化
	pCodec = avcodec_find_decoder(CODEC_ID_H264);
	if (!pCodec)
	{
		fprintf(stderr, "Codec not found\n");
		return FALSE;
	}
	pCodecCtx = avcodec_alloc_context();
	if (!pCodecCtx)
	{
		return FALSE;
	}
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;
	pCodecCtx->bit_rate = 0;
	pCodecCtx->frame_number = 1;
	pCodecCtx->width = width;
	pCodecCtx->height = height;
	pCodecCtx->pix_fmt = PIX_FMT_YUVJ420P;
	//打开解码器
	if (avcodec_open(pCodecCtx, pCodec) < 0)
	{
		return FALSE;
	}
	pFrame = avcodec_alloc_frame();
	//初始化packet
	av_init_packet(&pkt);
	pkt.data = (unsigned char *)pSrcData;
	pkt.size = dwDataSize;

	//开始解码
	if(avcodec_decode_video2(pCodecCtx, pFrame, &deCodePicSize, &pkt) > 0)
	{
		//转换为RGB24
		if(FALSE == YUVtoRGB(pFrame, width, height, pFileName))
	}
}