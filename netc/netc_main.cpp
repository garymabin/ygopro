#include "../common/common.h"

#include "scene_mgr.h"
#include "image_mgr.h"
#include "card_data.h"
#include "deck_data.h"
#include "sungui.h"
#include "bs/build_input_handler.h"
#include "bs/build_scene_handler.h"
#include "bs/build_scene.h"
#include "ds/duel_input_handler.h"
#include "ds/duel_scene_handler.h"
#include "ds/duel_network.h"
#include "ds/duel_scene.h"

#if defined _ANDROID
#include <AndroidGlueCompat.h>
#endif

using namespace ygopro;

static float xrate = 0.0f;
static float yrate = 0.0f;
static bool need_draw = true;

#if defined _ANDROID
void android_main(struct android_app* state) {
	LOGI("android_main:");
	AndGameEngine engine;
	init_android_world(state, &engine);
	LOGI("after init engine:");
    if(!commonCfg.LoadConfig(L"common.xml"))
        return;
    int width = commonCfg["window_width"];
    int height = commonCfg["window_height"];
	int fsaa = commonCfg["fsaa"];
    int vsync = commonCfg["vertical_sync"];

//    std::cout << "GL Vendor    : " << glGetString(GL_VENDOR) << std::endl;
//    std::cout << "GL Renderer  : " << glGetString(GL_RENDERER) << std::endl;
//    std::cout << "GL Version (string)  : " << glGetString(GL_VERSION) << std::endl;
//    std::cout << "GLSL Version : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
//    glGetError();
	ImageMgr::Get().InitTextures(commonCfg["image_path"]);
	if(!stringCfg.LoadConfig(commonCfg["string_conf"])
			|| DataMgr::Get().LoadDatas(commonCfg["database_file"])
			|| !ImageMgr::Get().LoadImageConfig(commonCfg["textures_conf"])
			|| !sgui::SGGUIRoot::GetSingleton().LoadConfigs(commonCfg["gui_conf"])) {
		LOGI("failed init configs:");
		return;
	}
	LimitRegulationMgr::Get().LoadLimitRegulation(commonCfg["limit_regulation"], stringCfg["eui_list_default"]);
	stringCfg.ForEach([](const std::string& name, ValueStruct& value) {
				if(name.find("setname_") == 0 ) {
					std::wstring setname = To<std::wstring>(name.substr(8));
					DataMgr::Get().RegisterSetCode(static_cast<unsigned int>(value), setname);
				}
			});
	LOGI("begin init SceneMgr:");
	SceneMgr::Get().Init(commonCfg["layout_conf"]);

    xrate = (float)engine.width / width;
    yrate = (float)engine.height / height;

    sgui::SGGUIRoot::GetSingleton().SetSceneSize({engine.width, engine.height});
    SceneMgr::Get().SetSceneSize({engine.width, engine.height});
    SceneMgr::Get().InitDraw();
    SceneMgr::Get().SetFrameRate((int)commonCfg["frame_rate"]);

    //auto sc = std::make_shared<BuildScene>();
    //SceneMgr::Get().SetScene(std::static_pointer_cast<Scene>(sc));
    auto sc = std::make_shared<DuelScene>();
    SceneMgr::Get().SetScene(std::static_pointer_cast<Scene>(sc));
    LOGI("before main loop:");
	while (1) {
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident = ALooper_pollAll(0, NULL,
								&events, (void**) &source)) >= 0) {

			// Process this event.
			if (source != NULL) {
				source->process(state, source);
			}

			// If a sensor has data, process it now.
			if (ident == LOOPER_ID_USER) {
				if (engine.accelerometerSensor != NULL) {
					ASensorEvent event;
					while (ASensorEventQueue_getEvents(
									engine.sensorEventQueue, &event, 1) > 0) {
					}
				}
			}

			// Check if we are exiting.
			if (state->destroyRequested != 0) {
				break;
			}
		}
		if (engine.animating) {
			SceneMgr::Get().CheckFrameRate();
			SceneMgr::Get().InitDraw();
			auto& shader = glbase::Shader::GetDefaultShader();
			shader.Use();
			shader.SetParam1i("texID", 0);
			if(!SceneMgr::Get().Update())
				break;
			shader.Unuse();
			SceneMgr::Get().Draw();
			sgui::SGGUIRoot::GetSingleton().Draw();

			eglSwapBuffers(engine.display, engine.surface);
		}
	}

    SceneMgr::Get().Uninit();
    sgui::SGGUIRoot::GetSingleton().Unload();
    ImageMgr::Get().UninitTextures();
    glbase::Shader::GetDefaultShader().Unload();

	engine_term_display(&engine);
}
#else
int main(int argc, char* argv[]) {
    if(!glfwInit())
        return 0;
    if(!commonCfg.LoadConfig(L"common.xml"))
        return 0;
    int width = commonCfg["window_width"];
    int height = commonCfg["window_height"];
	int fsaa = commonCfg["fsaa"];
    int vsync = commonCfg["vertical_sync"];
	if(fsaa)
		glfwWindowHint(GLFW_SAMPLES, fsaa);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow(width, height, "Ygopro", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 0;
    }
    glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);
    glfwShowWindow(window);
    glfwMakeContextCurrent(window);
	glewExperimental = true;
    glewInit();

    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cout << "GL Vendor    : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "GL Renderer  : " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GL Version (string)  : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GL Version (integer) : " << major << "." << minor << std::endl;
    std::cout << "GLSL Version : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    glGetError();
    
    ImageMgr::Get().InitTextures(commonCfg["image_path"]);
    if(!stringCfg.LoadConfig(commonCfg["string_conf"])
       || DataMgr::Get().LoadDatas(commonCfg["database_file"])
       || !ImageMgr::Get().LoadImageConfig(commonCfg["textures_conf"])
       || !sgui::SGGUIRoot::GetSingleton().LoadConfigs(commonCfg["gui_conf"])) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }
    LimitRegulationMgr::Get().LoadLimitRegulation(commonCfg["limit_regulation"], stringCfg["eui_list_default"]);
    stringCfg.ForEach([](const std::string& name, ValueStruct& value) {
        if(name.find("setname_") == 0 ) {
            std::wstring setname = To<std::wstring>(name.substr(8));
            DataMgr::Get().RegisterSetCode(static_cast<unsigned int>(value), setname);
        }
    });
    SceneMgr::Get().Init(commonCfg["layout_conf"]);
    
    int bwidth, bheight;
    glfwGetFramebufferSize(window, &bwidth, &bheight);
    xrate = (float)bwidth / width;
    yrate = (float)bheight / height;
    sgui::SGGUIRoot::GetSingleton().SetSceneSize({bwidth, bheight});
    SceneMgr::Get().SetSceneSize({bwidth, bheight});
    SceneMgr::Get().InitDraw();
    SceneMgr::Get().SetFrameRate((int)commonCfg["frame_rate"]);
//    auto sc = std::make_shared<BuildScene>();
//    auto ih = std::make_shared<BuildInputHandler>(sc);
//    auto sh = std::make_shared<BuildSceneHandler>(sc);
//    sc->SetInputHandler(ih);
//    sc->SetSceneHandler(sh);
//    SceneMgr::Get().SetScene(sc);
    
    auto sc = std::make_shared<DuelScene>();
    auto ih = std::make_shared<DuelInputHandler>(sc);
    auto sh = std::make_shared<DuelSceneHandler>(sc);
    auto ph = std::make_shared<DuelProtoTcp>();
    sh->SetProtoHandler(ph);
    sc->SetInputHandler(ih);
    sc->SetSceneHandler(sh);
    SceneMgr::Get().SetScene(sc);
    
    glfwSetKeyCallback(window, [](GLFWwindow* wnd, int key, int scan, int action, int mods) {
        if(action == GLFW_PRESS) {
            if(key == GLFW_KEY_GRAVE_ACCENT && (mods & GLFW_MOD_ALT))
                SceneMgr::Get().ScreenShot();
            if(!sgui::SGGUIRoot::GetSingleton().InjectKeyDownEvent({key, mods}))
                SceneMgr::Get().GetScene()->GetInputHandler()->KeyDown({key, mods});
        } else if(action == GLFW_RELEASE) {
            if(!sgui::SGGUIRoot::GetSingleton().InjectKeyUpEvent({key, mods}))
                SceneMgr::Get().GetScene()->GetInputHandler()->KeyUp({key, mods});
        }
    });
    glfwSetCharCallback(window, [](GLFWwindow* wnd, unsigned int unichar) {
        sgui::SGGUIRoot::GetSingleton().InjectCharEvent({unichar});
    });
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* wnd, int width, int height) {
        int x, y;
        glfwGetWindowSize(wnd, &x, &y);
        xrate = (float)width / x;
        yrate = (float)height / y;
        SceneMgr::Get().SetSceneSize(v2i{width, height});
        sgui::SGGUIRoot::GetSingleton().SetSceneSize(v2i{width, height});
    });
    glfwSetCursorEnterCallback(window, [](GLFWwindow* wnd, int enter) {
        if(enter == GL_TRUE)
            sgui::SGGUIRoot::GetSingleton().InjectMouseEnterEvent();
        else
            sgui::SGGUIRoot::GetSingleton().InjectMouseLeaveEvent();
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* wnd, double xpos, double ypos) {
        SceneMgr::Get().SetMousePosition({(int)(xpos * xrate), (int)(ypos * yrate)});
        if(!sgui::SGGUIRoot::GetSingleton().InjectMouseMoveEvent({(int)(xpos * xrate), (int)(ypos * yrate)}))
            SceneMgr::Get().GetScene()->GetInputHandler()->MouseMove({(int)(xpos * xrate), (int)(ypos * yrate)});
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* wnd, int button, int action, int mods) {
        double xpos, ypos;
        glfwGetCursorPos(wnd, &xpos, &ypos);
        xpos *= xrate;
        ypos *= yrate;
        SceneMgr::Get().SetMousePosition({(int)xpos, (int)ypos});
        if(action == GLFW_PRESS) {
            if(!sgui::SGGUIRoot::GetSingleton().InjectMouseButtonDownEvent({button, mods, (int)xpos, (int)ypos}))
                SceneMgr::Get().GetScene()->GetInputHandler()->MouseButtonDown({button, mods, (int)xpos, (int)ypos});
        } else {
            if(!sgui::SGGUIRoot::GetSingleton().InjectMouseButtonUpEvent({button, mods, (int)xpos, (int)ypos}))
                SceneMgr::Get().GetScene()->GetInputHandler()->MouseButtonUp({button, mods, (int)xpos, (int)ypos});
        }
    });
    glfwSetScrollCallback(window, [](GLFWwindow* wnd, double xoffset, double yoffset) {
        if(!sgui::SGGUIRoot::GetSingleton().InjectMouseWheelEvent({(float)xoffset, (float)yoffset}))
            SceneMgr::Get().GetScene()->GetInputHandler()->MouseWheel({(float)xoffset, (float)yoffset});
    });
    glfwSetWindowIconifyCallback(window, [](GLFWwindow* wnd, int iconified) {
        need_draw = (iconified == GL_FALSE);
        if(need_draw)
            SceneMgr::Get().SetFrameRate((int)commonCfg["frame_rate"]);
        else
            SceneMgr::Get().SetFrameRate(10);
    });
    if(vsync)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);
    while (!glfwWindowShouldClose(window)) {
        SceneMgr::Get().CheckFrameRate();
        SceneMgr::Get().InitDraw();
        auto& shader = glbase::Shader::GetDefaultShader();
        shader.Use();
        shader.SetParam1i("texID", 0);
        glfwPollEvents();
        if(!SceneMgr::Get().Update())
            break;
        shader.Unuse();
        if(need_draw) {
            SceneMgr::Get().Draw();
            sgui::SGGUIRoot::GetSingleton().Draw();
        }
        glfwSwapBuffers(window);
    }
    
    SceneMgr::Get().Uninit();
    sgui::SGGUIRoot::GetSingleton().Unload();
    ImageMgr::Get().UninitTextures();
    glbase::Shader::GetDefaultShader().Unload();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
#endif
