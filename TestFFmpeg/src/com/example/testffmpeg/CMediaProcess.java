package com.example.testffmpeg;

import android.util.Log;

public class CMediaProcess implements Runnable{
	
	/// 定义常量网络类型
	public static final int Net_UDP_Type = 0;
	public static final int Net_TCP_Type = 1;	
	/// 定义要获取的获取FFmpeg操作对象
	private CFFmpegJni m_FFMpegJni = null;
	/// 定义播放状态
	private boolean m_bIsPlaying = false;
	/// 定义解码线程对象
	private Thread m_DecoderTread = null;
	
	public CMediaProcess(CFFmpegJni FFmpegJni)
	{
		/// 赋值FFmepg解码器
		m_FFMpegJni = FFmpegJni;
	}
	
	public int e_Init(String strRTSPUrl, int nMediaType)
	{
		/// 初始化媒体对象
		return m_FFMpegJni.IInit(strRTSPUrl, nMediaType);
	}
	
	public void e_IResize(int nDecodeWidth, int nDecodeHeight)
	{
		/// 设置显示像素
		m_FFMpegJni.IResize(nDecodeWidth, nDecodeHeight);
	}

	public void e_Start()
	{
		/// 如果播放状态为停止播放状态，播放媒体文件
		if(false == m_bIsPlaying)
		{
			/// 赋值正在播放状态
			this.m_bIsPlaying = true;
			/// 播放线程启动
			m_DecoderTread = new Thread(this);
			m_DecoderTread.start();
		}
	}
	
	public void e_Stop()
	{
		/// 如果正在播放
		if(true == m_bIsPlaying)
		{
			/// 调用Jni停止播放视频
			m_FFMpegJni.IStop();
			/// 赋值播放状态为：未播放状态
			m_bIsPlaying = false;
			/// 播放线程如果在播放过程中，停止线程运行
			if(null != m_DecoderTread && m_DecoderTread.isAlive())
			{
				Log.d("Thread Test", "T Alive --------------------");
				m_DecoderTread.interrupt();
			}			
			/// 清空数据帧队列
			CVideoFrames.m_FrameDatas.clear();
		}
	}

	@Override
	public void run()
	{
		/// 开始播放
		m_FFMpegJni.IPlay();
		m_bIsPlaying = false;
	}
}