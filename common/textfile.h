#ifndef _TEXTFILE_H_
#define _TEXTFILE_H_

#if defined _ANDROID
#include <AndroidGlueCompat.h>
#endif
    
class TextFile {
public:
    TextFile(const std::string& filename) {
#if defined _ANDROID
    	if (filename[0] == '/') {
    		std::ifstream txtfile(filename, std::ios::binary);
			if(txtfile) {
				txtfile.seekg(0, txtfile.end);
				file_size = txtfile.tellg();
				txtfile.seekg(0, txtfile.beg);
				datas.resize(file_size + 1);
				txtfile.read(&datas[0], file_size);
				datas[file_size] = 0;
			} else {
				file_size = 0;
				datas.push_back(0);
			}
    	} else {
    		std::string handled_str = filename;
    		AAssetDir* assetDir = NULL;
    		if (handled_str[0] == '.' && handled_str[1] == '/') {
    			handled_str = handled_str.substr(2, handled_str.length() - 2);
    		}
			int pathSeparatorIndex = handled_str.find_last_of('/');
			if (pathSeparatorIndex != std::string::npos) {
				assetDir = AAssetManager_openDir(::mainApp->activity->assetManager,
						handled_str.substr(0, pathSeparatorIndex).c_str());
				LOGI("try to open asset dir: %s", handled_str.substr(0, pathSeparatorIndex).c_str());
				handled_str = handled_str.substr(pathSeparatorIndex + 1, handled_str.length() - pathSeparatorIndex - 1);
			}
			LOGI("try to open asset file: %s", handled_str.c_str());
			//if file name does not start with file seperator, just regard it as an asset relative path
			AAsset* asset = AAssetManager_open(::mainApp->activity->assetManager, handled_str.c_str(), AASSET_MODE_BUFFER);
			if (asset) {
				file_size = AAsset_getLength(asset);
				datas.resize(file_size + 1);
				if (AAsset_read(asset, &datas[0], file_size) <= 0) {
					file_size = 0;
					datas.push_back(0);
				} else {
					datas[file_size] = 0;
				}
				AAsset_close(asset);
			} else {
				LOGW("can not open asset file: %s", filename.c_str());
			}
			if (assetDir != NULL) {
				AAssetDir_close(assetDir);
			}
    	}
#else
        std::ifstream txtfile(filename, std::ios::binary);
        if(txtfile) {
            txtfile.seekg(0, txtfile.end);
            file_size = txtfile.tellg();
            txtfile.seekg(0, txtfile.beg);
            datas.resize(file_size + 1);
            txtfile.read(&datas[0], file_size);
            datas[file_size] = 0;
        } else {
            file_size = 0;
            datas.push_back(0);
        }
#endif
    }
    
    inline char* Data() {
        return &datas[0];
    }
    
private:
    size_t file_size = 0;
    std::vector<char> datas;
};

#endif
