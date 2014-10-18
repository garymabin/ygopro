package cn.garymb.common;

import android.annotation.TargetApi;
import android.os.AsyncTask;
import android.os.Build;

public class Utils {
	
	@SuppressWarnings({ "unchecked", "rawtypes" })
	@TargetApi(Build.VERSION_CODES.HONEYCOMB)
	public static void AsyncTaskExecute(AsyncTask task, Object... params) {
		if (Build.VERSION.SDK_INT >= 11) {
			task.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR, params);
		} else {
			task.execute(params);
		}
	}
}
