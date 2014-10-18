package cn.garymb.common;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.content.Context;
import android.content.res.AssetManager;
import android.text.TextUtils;

public class FileOpsUtils {
	public static void assetsCopy(Context context, String assetsPath,
			String dirPath, boolean isSmart) throws IOException {
		AssetManager am = context.getAssets();
		String[] list = am.list(assetsPath);
		if (list.length == 0) {
			File file = new File(dirPath);
			if (!isSmart || !file.exists()) {
				file.getParentFile().mkdirs();
				file.createNewFile();
				InputStream in = am.open(assetsPath);
				FileOutputStream fout = new FileOutputStream(file);
				write(in, fout);
			}
		} else {
			for (String path : list) {
				assetsCopy(context, join(assetsPath, path),
						join(dirPath, path), isSmart);
			}
		}
	}

	private static void write(InputStream in, OutputStream out)
			throws IOException {
		byte[] buf = new byte[Constants.IO_BUFFER_SIZE];
		int count;
		try {
			while ((count = in.read(buf)) != -1) {
				out.write(buf, 0, count);
			}
			out.flush();
		} finally {
			in.close();
			out.close();
		}
	}

	// Joins two path components, adding a separator only if necessary.
	private static String join(String prefix, String suffix) {
		if (TextUtils.isEmpty(prefix)) {
			return suffix;
		}
		int prefixLength = prefix.length();
		boolean haveSlash = (prefixLength > 0 && prefix
				.charAt(prefixLength - 1) == File.separatorChar);
		if (!haveSlash) {
			haveSlash = (suffix.length() > 0 && suffix.charAt(0) == File.separatorChar);
		}
		return haveSlash ? (prefix + suffix)
				: (prefix + File.separatorChar + suffix);
	}
}
