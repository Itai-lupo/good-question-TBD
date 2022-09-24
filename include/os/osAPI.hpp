#pragma once
#include "core.hpp"
#include "osEventsData.hpp"

#include <string>
#include <functional>
#include <vector>
#include <glm/glm.hpp>
#include <functional>


struct windowSpec
{
	windowSpec(std::string title, int w, int h): w(w), h(h), title(title){}
	int w, h;
	std::string title;
};

struct subSurfaceSpec
{
    uint8_t subSurfaceSlot;
    int x, y;
    int width, height;
};


class osAPI
{
	public:
		osAPI();
		~osAPI();
		windowId createWindow(const windowSpec& windowToCreate);
        void attachSubSurfaceToWindow(windowId id,  const subSurfaceSpec& subSurfaceToAttach);
		
        bool isWindowOpen(windowId winId);
        void setVSyncForCurrentContext(bool enabled);
        void makeContextCurrent(windowId winId);
        void closeWindow(windowId winId);
        void swapBuffers(windowId winId);
        windowId getCurrentContextWindowId();

        void *getProcAddress();

        // ################################ get window info #######################
        std::string getWindowTitle(windowId winId);
        std::pair<uint32_t, uint32_t> getWindowSize(windowId winId);
        

        
        // ################ set event listener ################################################################
        void setKeyPressEventListeners(windowId winId, std::function<void(const keyData&)> callback);
        void setKeyReleasedEventListeners(windowId winId, std::function<void(const keyData&)> callback);
        void setKeyRepeatEventListeners(windowId winId, std::function<void(const keyData&)> callback);

        void setMouseButtonPressEventListeners(windowId winId, std::function<void(const mouseButtonData&)> callback);
        void setMouseButtonReleasedEventListeners(windowId winId, std::function<void(const mouseButtonData&)> callback);
        
        void setMouseMovedListeners(windowId winId, std::function<void(const mouseMoveData&)> callback);
        void setMouseScrollListeners(windowId winId, std::function<void(const mouseScrollData&)> callback);

        void setCloseEventeListeners(windowId winId, std::function<void()> callback);
        void setResizeEventeListeners(windowId winId, std::function<void(const windowResizeData&)> callback);
        void setGainFocusEventListeners(windowId winId, std::function<void()> callback);
        void setLostFocusEventListeners(windowId winId, std::function<void()> callback);
        void setRenderEventListeners(windowId winId, std::function<void(const windowRenderData&)> callback);
        void setsubSurfaceRenderEventListeners(windowId winId, int subSurfaceSlot, std::function<void(const windowRenderData&)> callback);
        // ################ unset event listener ################################################################
        void unsetKeyPressEventListeners(windowId winId);
        void unsetKeyReleasedEventListeners(windowId winId);
        void unsetKeyRepeatEventListeners(windowId winId);

        void unsetMouseButtonPressEventListeners(windowId winId);
        void unsetMouseButtonReleasedEventListeners(windowId winId);
        
        void unsetMouseMovedListeners(windowId winId);
        void unsetMouseScrollListeners(windowId winId);

        void unsetCloseEventeListeners(windowId winId);
        void unsetResizeEventeListeners(windowId winId);
        void unsetGainFocusEventListeners(windowId winId);
        void unsetLostFocusEventListeners(windowId winId);
        void unsetRenderEventListeners(windowId winId);
        void unsetsubSurfaceRenderEventListeners(windowId winId, int subSurfaceSlot);
};

