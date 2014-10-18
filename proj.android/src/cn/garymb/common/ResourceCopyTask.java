package cn.garymb.common;

import java.io.File;
import java.io.IOException;

import cn.garymb.ygopro.StaticApplication;

import android.content.Context;
import android.os.AsyncTask;

public class ResourceCopyTask extends AsyncTask<Object, Integer, Integer> {

	public interface ResCopyListener {
		void onCopyFinish(int code);
	}

	private static final String[] CORE_CONFIG_FILES = {
			Constants.CORE_FILE_NAME_COMMON_XML,
			Constants.CORE_FILE_NAME_CONFIG_DIR };

	public static final int RES_COPY_RESULT_SUCCESS = 0;
	public static final int RES_COPY_RESULT_FAILED = -1;

	private static final int MAX_RETRY_COUNT = 3;

	private String mTargetFileDirs;

	private ResCopyListener mListener;

	private int mRetryCount = 0;

	public ResourceCopyTask(String targetFileDirs, ResCopyListener listener) {
		mTargetFileDirs = targetFileDirs;
		mListener = listener;
	}

	@Override
	protected Integer doInBackground(Object... params) {
		Integer result = RES_COPY_RESULT_SUCCESS;
		if (mTargetFileDirs != null) {
			boolean isCopyNeeded = false;
			for (String file : CORE_CONFIG_FILES) {
				File config = new File(mTargetFileDirs, file);
				if (!config.exists()) {
					isCopyNeeded = true;
					break;
				}
			}
			if (isCopyNeeded) {
				final Context context = StaticApplication.peekInstance();
				while (mRetryCount++ < MAX_RETRY_COUNT) {
					try {
						FileOpsUtils.assetsCopy(context, "", mTargetFileDirs,
								true);
					} catch (IOException e) {
						if (mRetryCount >= MAX_RETRY_COUNT) {
							result = RES_COPY_RESULT_FAILED;
						}
						e.printStackTrace();
						continue;
					}
					break;
				}
			}
		}
		return result;
	}

	@Override
	protected void onPostExecute(Integer result) {
		super.onPostExecute(result);
		if (mListener != null) {
			mListener.onCopyFinish(result);
		}
	}

}
