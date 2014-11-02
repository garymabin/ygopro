/*
 * YGOProApp.cpp
 *
 *  Created on: 2014年11月1日
 *      Author: mabin
 */

#include "../../common/common.h"
#include "../../netc/scene_mgr.h"
#include "../../netc/image_mgr.h"
#include "../../netc/card_data.h"
#include "../../netc/deck_data.h"
#include "../../netc/sungui.h"
#include "../../netc/ds/duel_network.h"
#include "YGOProApp.h"
#include "../../netc/ds/duel_input_handler.h"
#include "../../netc/ds/duel_scene_handler.h"
#include "../../netc/bs/build_input_handler.h"
#include "../../netc/bs/build_scene_handler.h"

using namespace ygopro;
using namespace std;

PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC) eglGetProcAddress("glGenVertexArraysOES");
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC) eglGetProcAddress("glBindVertexArrayOES");
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC) eglGetProcAddress("glDeleteVertexArraysOES");

YGOProApp::YGOProApp(NvPlatformContext* platform, const char* appTitle) :
		NvAppBase(platform, appTitle), m_configHeight(0), m_configWidth(0), m_xRate(
				0.0f), m_yRate(0.0f), m_pBuildScene(NULL), m_pDuelScene(NULL) {
	vector<string> args = getPlatformContext()->getCommandLine();
	int argPos = 0;
	for (vector<string>::iterator it = args.begin(); it != args.end();
			++it, argPos++) {
	if (argPos == 0) {
		m_searchPath = (*it);
		LOGI("current working path: %s", m_searchPath.c_str());
		chdir(m_searchPath.c_str());
	}
}
	if (!commonCfg.LoadConfig(L"common.xml")) {
		LOGW("load common config failed");
	}
	LimitRegulationMgr::Get().LoadLimitRegulation(commonCfg["limit_regulation"],
			stringCfg["eui_list_default"]);
	stringCfg.ForEach(
			[](const std::string& name, ValueStruct& value) {
				if(name.find("setname_") == 0 ) {
					std::wstring setname = To<std::wstring>(name.substr(8));
					DataMgr::Get().RegisterSetCode(static_cast<unsigned int>(value), setname);
				}
			});
	m_configHeight = commonCfg["window_width"];
	m_configWidth = commonCfg["window_height"];
	SceneMgr::Get().Init(commonCfg["layout_conf"]);
}

YGOProApp::~YGOProApp() {
}

bool YGOProApp::handlePointerInput(NvInputDeviceType::Enum device,
		NvPointerActionType::Enum action, uint32_t modifiers, int32_t count,
		NvPointerEvent* points, int64_t timestamp) {
	return false;
}

bool YGOProApp::pointerInput(NvInputDeviceType::Enum device,
		NvPointerActionType::Enum action, uint32_t modifiers, int32_t count,
		NvPointerEvent* points, int64_t timestamp) {
	LOGI("motion touch input received");
	return false;
}

bool YGOProApp::handleKeyInput(uint32_t code, NvKeyActionType::Enum action) {
	return false;
}

bool YGOProApp::keyInput(uint32_t code, NvKeyActionType::Enum action) {
	LOGI("key input received");
	return false;
}

bool YGOProApp::handleCharacterInput(uint8_t c) {
	return false;
}

bool YGOProApp::characterInput(uint8_t c) {
	LOGI("character input received");
	return false;
}

bool YGOProApp::gamepadChanged(uint32_t changedPadFlags) {
	return false;
}

void YGOProApp::configurationCallback(NvEGLConfiguration& config) {
    config.depthBits   = 16;
    config.stencilBits = 0;
    config.apiVer      = NvGfxAPIVersionES2();
}

void YGOProApp::draw(void) {
	SceneMgr::Get().CheckFrameRate();
	SceneMgr::Get().InitDraw();
	auto& shader = glbase::Shader::GetDefaultShader();
	shader.Use();
	shader.SetParam1i("texID", 0);
	if (!SceneMgr::Get().Update())
		return;
	shader.Unuse();
	SceneMgr::Get().Draw();
	sgui::SGGUIRoot::GetSingleton().Draw();
}


void YGOProApp::initRendering(void) {
	LOGI("initRendering: Enter");
	char* extensionList = (char*) glGetString(GL_EXTENSIONS);
	LOGI("surpported extensions: %s", extensionList);
	ImageMgr::Get().InitTextures(commonCfg["image_path"]);
	ImageMgr::Get().LoadImageConfig(commonCfg["textures_conf"]);
	int screenWidth = getGLContext()->width();
	int screenHeight = getGLContext()->height();
	sgui::SGGUIRoot::GetSingleton().LoadConfigs(commonCfg["gui_conf"]);
	sgui::SGGUIRoot::GetSingleton().SetSceneSize(
			{ screenWidth, screenHeight });
	SceneMgr::Get().SetSceneSize( { screenWidth, screenHeight });
	SceneMgr::Get().SetFrameRate((int) commonCfg["frame_rate"]);

	m_pBuildScene = std::make_shared<BuildScene>();
	auto bih = std::make_shared<BuildInputHandler>(m_pBuildScene);
	auto bsh = std::make_shared<BuildSceneHandler>(m_pBuildScene);
	m_pBuildScene->SetInputHandler(bih);
	m_pBuildScene->SetSceneHandler(bsh);

	m_pDuelScene = std::make_shared<DuelScene>();
	auto dih = std::make_shared<DuelInputHandler>(m_pDuelScene);
	auto dsh = std::make_shared<DuelSceneHandler>(m_pDuelScene);
	auto ph = std::make_shared<DuelProtoTcp>();
	m_pDuelScene->SetInputHandler(dih);
	m_pDuelScene->SetSceneHandler(dsh);
	dsh->SetProtoHandler(ph);

	SceneMgr::Get().SetScene(m_pBuildScene);
}

void YGOProApp::shutdownRendering(void) {
	LOGI("shutdownRendering: Enter");
	SceneMgr::Get().Uninit();
	ImageMgr::Get().UninitTextures();
	sgui::SGGUIRoot::GetSingleton().Unload();
	glbase::Shader::GetDefaultShader().Unload();
	m_pBuildScene.reset();
	m_pDuelScene.reset();
}

NvAppBase* NvAppFactory(NvPlatformContext* platform) {
	return new YGOProApp(platform);
}
