package cn.garymb.ygopro;

import cn.garymb.common.ResourceCopyTask;
import cn.garymb.common.ResourceCopyTask.ResCopyListener;
import cn.garymb.common.Utils;
import android.support.v4.app.FragmentActivity;
import android.widget.Toast;
import android.annotation.TargetApi;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;

public class LauncherActivity extends FragmentActivity implements
		ResCopyListener {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_launcher);
		Utils.AsyncTaskExecute(new ResourceCopyTask(StaticApplication
				.peekInstance().getCoreConfigDir(), this));
	}

	@TargetApi(Build.VERSION_CODES.HONEYCOMB)
	@Override
	public void onCopyFinish(int code) {
		if (code == ResourceCopyTask.RES_COPY_RESULT_SUCCESS) {
			finish();
			Intent intent = new Intent();
			intent.setClass(this, MainActivity.class);
			intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
			intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
			intent.putExtra("arguments", StaticApplication
					.peekInstance().getCoreConfigDir());
			startActivity(intent);
		} else {
			Toast.makeText(this, R.string.copy_res_failed_hint,
					Toast.LENGTH_SHORT).show();
			finish();
		}
	}
}
