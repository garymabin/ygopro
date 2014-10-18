package cn.garymb.ygopro;

import android.app.Application;
import android.os.Environment;

public class StaticApplication extends Application {

	private static StaticApplication INSTANCE;
	
	private static String mConfigDir;

	@Override
	public void onCreate() {
		super.onCreate();
		INSTANCE = this;
		if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())) {
			mConfigDir = getExternalFilesDir("").toString();
		} else {
			mConfigDir = getFilesDir().toString();
		}
	}

	public static StaticApplication peekInstance() {
		return INSTANCE;
	}
	
	public String getCoreConfigDir() {
		return mConfigDir;
	}

}
