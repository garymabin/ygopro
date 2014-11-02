/*
 * YGOProApp.h
 *
 *  Created on: 2014年11月1日
 *      Author: mabin
 */

#ifndef YGOPROAPP_H_
#define YGOPROAPP_H_
#include <NvAppBase/NvAppBase.h>
#include <AndroidGlueCompat.h>
#include "../../netc/bs/build_scene.h"
#include "../../netc/ds/duel_scene.h"

class YGOProApp : public NvAppBase {
public:
	YGOProApp(NvPlatformContext* platform, const char* appTitle = NULL);
	virtual ~YGOProApp();

protected:
	virtual void initRendering(void);
    void configurationCallback(NvEGLConfiguration& config);
	virtual void shutdownRendering(void);
	virtual bool handlePointerInput(NvInputDeviceType::Enum device,
			NvPointerActionType::Enum action, uint32_t modifiers, int32_t count,
			NvPointerEvent* points, int64_t timestamp = 0);
	virtual bool handleKeyInput(uint32_t code, NvKeyActionType::Enum action);
	virtual bool handleCharacterInput(uint8_t c);

	virtual bool pointerInput(NvInputDeviceType::Enum device,
	                                NvPointerActionType::Enum action,
	                                uint32_t modifiers,
	                                int32_t count, NvPointerEvent* points,
	                                int64_t timestamp=0);
	bool keyInput(uint32_t code, NvKeyActionType::Enum action);
	bool characterInput(uint8_t c);
	bool gamepadChanged(uint32_t changedPadFlags);
	virtual void draw(void);
private:
	int m_configWidth;
	int m_configHeight;
	float m_xRate;
	float m_yRate;

	std::string m_searchPath;
	std::shared_ptr<ygopro::BuildScene> m_pBuildScene;
	std::shared_ptr<ygopro::DuelScene> m_pDuelScene;
};

#endif /* YGOPROAPP_H_ */
