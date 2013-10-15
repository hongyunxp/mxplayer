package com.example.testffmpeg;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.util.DisplayMetrics;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends Activity {
	
	private TextView textVersion = null;
	private Button btnGetVersion = null;
	private Button btnStartPlay = null;
	private Button btnStopPlay = null;	
	private CMediaProcess m_MediaProcess = null;
	private String m_strRTSPUrl = "";
	private EditText EdtUrl = null;
	
	private int m_MeidaWith = 288;
	private int m_nScale = 0;
	
	public CFFmpegJni m_FFmpegJni = null;
	
	/// 加载要调用的动态库
	static 
	{
		System.loadLibrary("ffmpeg");
		System.loadLibrary("FFmpegJni");
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		/// 实例化FFmpegJni解码器
		m_FFmpegJni = new CFFmpegJni();
		/// 获取视图对象
		textVersion = (TextView)findViewById(R.id.TestVersion);
		btnGetVersion = (Button)findViewById(R.id.btnGetVersion);
		btnStartPlay = (Button)findViewById(R.id.btnStartPlay);
		btnStopPlay = (Button)findViewById(R.id.btnStopPlay);
		EdtUrl = (EditText)findViewById(R.id.EdtRTSPURL);
		/// 设置按钮监听
		btnGetVersion.setOnClickListener(new onBtnClickListener());
		btnStartPlay.setOnClickListener(new onBtnClickListener());
		btnStopPlay.setOnClickListener(new onBtnClickListener());
	}

	class onBtnClickListener implements OnClickListener
	{
		@Override
		public void onClick(View v) {
			
			int nBtnID = v.getId();
			
			switch (nBtnID) {
			case R.id.btnGetVersion:
				/// 获取版本号
				int nVersion = m_FFmpegJni.IGetVersion();
				/// 设置获取版本号
				textVersion.setText("Version:" + nVersion);
				break;
			case R.id.btnStartPlay:
				/// 如果线程未启动
				if(null == m_MediaProcess && null != m_FFmpegJni)
				{
					/// 实例化媒体处理对象
					m_MediaProcess = new CMediaProcess(m_FFmpegJni);
					if(null != m_MediaProcess)
					{
						/// 获取URL地址
						m_strRTSPUrl = EdtUrl.getText().toString();
						
						/// 如果不是网络播放找到文件路径
						if(false == m_strRTSPUrl.startsWith("http://") && false == m_strRTSPUrl.startsWith("rtsp://"))
						{
							String strFileName = Environment.getExternalStorageDirectory().toString() + "/" + m_strRTSPUrl;					
							m_strRTSPUrl = strFileName;
						}
						
						DisplayMetrics dm = new DisplayMetrics();
						MainActivity.this.getWindowManager().getDefaultDisplay().getMetrics(dm);				
						int VideoHeight = 0;
					    if(0 == m_nScale)
					    {
					    	VideoHeight = (dm.widthPixels *3) /4;
					    }
					    else
					    {
					    	VideoHeight = (dm.widthPixels *9) /16;
					    }
						
						/// 初始化播放信息
						if(0 == m_MediaProcess.e_Init(m_strRTSPUrl, CMediaProcess.Net_TCP_Type, 0, m_MeidaWith, VideoHeight))
						{
							/// 开始播放
							m_MediaProcess.e_Start();
						}
					}
				}
				break;
			case R.id.btnStopPlay:
				if(null != m_MediaProcess)
				{
					/// 停止播放
					m_MediaProcess.e_Stop();
					m_MediaProcess = null;
				}				
				break;
			default:
				break;
			}
		}
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
}