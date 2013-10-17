package com.example.testffmpeg;

import java.util.concurrent.ConcurrentLinkedQueue;

import android.util.Log;

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
			m_FrameDatas.add(pBufferData);
			/// 打印日志
			Log.d("e_AddDataToQueue", "Data Input To Queue Success! BufferSize = " + 
					pBufferData.length + " FramQueue Size = " + m_FrameDatas.size());
		}
	}
}
