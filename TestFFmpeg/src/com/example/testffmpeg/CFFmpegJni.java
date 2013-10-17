package com.example.testffmpeg;

public class CFFmpegJni {
	
	/***
	 * 获取版本号
	 * @return
	 */
	public native int IGetVersion();
	
	/***
	 * 初始化解码器，请求连接流媒体服务器
	 * @param strRTSPUrl
	 * @param nMediaType
	 * @param nScale
	 * @param nwidth
	 * @param nHeight
	 * @return
	 */
	public native int IInit(String strRTSPUrl, int nMediaType);

	/***
	 * 重设解码大小
	 * @param nWidth
	 * @param nHeight
	 */
	public native void IResize(int nWidth, int nHeight);
	
	/***
	 * 开始播放
	 */
	public native int IPlay();
	
	/***
	 * 停止播放
	 */
	public native int IStop();
}
