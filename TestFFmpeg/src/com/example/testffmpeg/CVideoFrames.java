package com.example.testffmpeg;

import java.util.concurrent.ConcurrentLinkedQueue;

public class CVideoFrames {
	
	/// 声明数据队列
	public static ConcurrentLinkedQueue<byte[]>  m_FrameDatas = new ConcurrentLinkedQueue<byte[]>();
	
	/***
	 * 静态方法 FFmpegJni 回调此方法，增加要显示的数据帧到队列
	 * @param pBufferData
	 */
	public static void e_AddDataToQueue(byte[] pBufferData)
	{
		if(null != pBufferData)
		{
			/// 增加要绘制的数据到Buffer
			m_FrameDatas.add(pBufferData);
		}
	}
}
