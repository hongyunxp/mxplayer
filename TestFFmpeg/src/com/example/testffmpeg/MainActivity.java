package com.example.testffmpeg;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
	
	public TextView textVersion = null;
	public Button btnGetVersion = null;
	
	/// 加载要调用的动态库
	static 
	{
		System.loadLibrary("ffmpeg");
		System.loadLibrary("FFmpegJni");
	}
	
	public FFmpegJni ffmpegjni = null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		ffmpegjni = new FFmpegJni();
		textVersion = (TextView)findViewById(R.id.TestVersion);
		btnGetVersion = (Button)findViewById(R.id.btnGetVersion);
		/// 设置按钮监听
		btnGetVersion.setOnClickListener(new onGerVersionClickListener());
	}

	class onGerVersionClickListener implements OnClickListener
	{
		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			Toast.makeText(MainActivity.this, "test change", Toast.LENGTH_LONG).show();
			
			Log.d("TESTLOGD", "Test Log Debug");
			
			/// 获取版本号
			int nVersion = ffmpegjni.IGetVersion();
			/// 设置获取版本号
			textVersion.setText("Version:" + nVersion);
		}		
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
}