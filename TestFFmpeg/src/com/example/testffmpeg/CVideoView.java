package com.example.testffmpeg;

import java.nio.ByteBuffer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

public class CVideoView  extends View  implements Runnable  {
	
	/// 定义显示视频的宽度和高度
	private int m_nDisplayWidth;
	private int m_nDisplayHeight;
	/// 定义绘制的Bitmap
	private Bitmap m_BitMap;
	
	private byte[]	m_NativePixels = null;
	private ByteBuffer m_BmpBuffer;
	private Paint m_Paint = null;
	
	@SuppressWarnings("unused")
	private Context m_Context;

	public CVideoView(Context context)
	{
		super(context);
		m_Paint = new Paint();
		// TODO Auto-generated constructor stub
	}
	
	public CVideoView(Context context, AttributeSet attrs)
	{
		super(context, attrs);
		m_Paint = new Paint();
	}
	
	public CVideoView(Context context, int nDisplayWidth, int nDisplayHeight)
	{
		super(context);
		m_Paint = new Paint();
		// TODO Auto-generated constructor stub
		m_nDisplayWidth = nDisplayWidth;
		m_nDisplayHeight = nDisplayHeight;
	}
	
	public void e_SetContext(Context context)
	{
		m_Context = context;
	}
	
	public int e_GetDisplayWidth()
	{
		return m_nDisplayWidth;
	}

	public void e_SetDisplayWidth(int nDisplayWidth)
	{
		m_nDisplayWidth = nDisplayWidth;
	}

	public int e_GetDisplayHeight()
	{
		return m_nDisplayHeight;
	}

	public void e_SetDisplayHeight(int nDisplayHeight)
	{
		m_nDisplayHeight = nDisplayHeight;
	}

	public void e_Play()
	{
		if(null == m_BitMap)
		{
			m_BitMap = Bitmap.createBitmap(m_nDisplayWidth, m_nDisplayHeight, Bitmap.Config.ARGB_8888);
			
			Log.i("TestBMP", "m_nDisplayWidth = " + m_nDisplayWidth + ", m_nDisplayHeight = "
					+ m_nDisplayHeight + ", BitMapCount = " + m_BitMap.getByteCount());
		}
		/// 启动绘制线程
	    new Thread(this).start();
	}
	
	public void e_Stop()
	{
		/// 停止绘制线程		
	}
	
	@Override
	protected void onDraw(Canvas canvas)
	{		
		super.onDraw(canvas);
		
		if(null == m_BitMap)
		{
			canvas.drawColor(Color.BLACK);
			return;
		}
		/// 获取数据帧
		m_NativePixels = CVideoFrames.m_FrameDatas.poll();
    	if(null != m_NativePixels && 0 < m_NativePixels.length)
    	{
    		m_BmpBuffer = ByteBuffer.wrap(m_NativePixels);
        	m_BitMap.copyPixelsFromBuffer(m_BmpBuffer);
    	}
    	
        canvas.drawBitmap(m_BitMap, 0, 0, m_Paint);
	}

	@Override
	public void run()
	{
		// TODO Auto-generated method stub
		while(false == Thread.currentThread().isInterrupted())
		{
            try
            {
                Thread.sleep(1);
            }
            catch(InterruptedException e) 
            {
                Thread.currentThread().interrupt();
            }
            /// 通知onDraw 函数绘制
            postInvalidate();
		}
	}
}
