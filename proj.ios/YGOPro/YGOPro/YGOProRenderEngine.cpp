//
//  YGOProRenderEngine.cpp
//  YGOPro
//
//  Created by 马彬 on 14-10-25.
//  Copyright (c) 2014年 garymabin@gmail.com. All rights reserved.
//

#include "YGOProRenderEngine.h"
#include <Foundation/NSObjCRuntime.h>
#include <scene_mgr.h>
#include <common.h>
#include <image_mgr.h>
#include <card_data.h>
#include <deck_data.h>
#include <sungui.h>
#include <bs/build_input_handler.h>
#include <bs/build_scene_handler.h>
#include <ds/duel_input_handler.h>
#include <ds/duel_scene_handler.h>
#include <ds/duel_network.h>
#include <ds/duel_scene.h>

namespace ygopro {
    
    void YGOProRenderEngine::Initialize(int bwidth, int bheight) {
        if(!commonCfg.LoadConfig(L"common.xml")) {
            NSLog(@"failed to load common.xml");
            return;
        }
        int width = commonCfg["window_width"];
        int height = commonCfg["window_height"];
        int fsaa = commonCfg["fsaa"];
        int vsync = commonCfg["vertical_sync"];
        
        ImageMgr::Get().InitTextures(commonCfg["image_path"]);
        if(!stringCfg.LoadConfig(commonCfg["string_conf"])
           || DataMgr::Get().LoadDatas(commonCfg["database_file"])
           || !ImageMgr::Get().LoadImageConfig(commonCfg["textures_conf"])
           || !sgui::SGGUIRoot::GetSingleton().LoadConfigs(commonCfg["gui_conf"])) {
            NSLog(@"init common utils failed");
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
    }
    
    void  YGOProRenderEngine::Render() {
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
    }
}