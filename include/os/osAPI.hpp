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


class osAPI
{
	public:
		osAPI();
		~osAPI();
		windowId createWindow(const windowSpec& windowToCreate);
		
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
        void setKeyPressEventListenrs(windowId winId, std::function<void(const keyData&)> callback);
        void setKeyReleasedEventListenrs(windowId winId, std::function<void(const keyData&)> callback);
        void setKeyRepeatEventListenrs(windowId winId, std::function<void(const keyData&)> callback);

        void setMouseButtonPressEventListenrs(windowId winId, std::function<void(const mouseButtonData&)> callback);
        void setMouseButtonReleasedEventListenrs(windowId winId, std::function<void(const mouseButtonData&)> callback);
        
        void setMouseMovedListenrs(windowId winId, std::function<void(const mouseMoveData&)> callback);
        void setMouseScrollListenrs(windowId winId, std::function<void(const mouseScrollData&)> callback);

        void setCloseEventeListenrs(windowId winId, std::function<void()> callback);
        void setResizeEventeListenrs(windowId winId, std::function<void(const windowResizeData&)> callback);
        void setGainFocusEventListeners(windowId winId, std::function<void()> callback);
        void setLostFocusEventListeners(windowId winId, std::function<void()> callback);
        void setRenderEventListeners(windowId winId, std::function<void(const windowRenderData&)> callback);
        
        // ################ unset event listener ################################################################
        void unsetKeyPressEventListenrs(windowId winId);
        void unsetKeyReleasedEventListenrs(windowId winId);
        void unsetKeyRepeatEventListenrs(windowId winId);

        void unsetMouseButtonPressEventListenrs(windowId winId);
        void unsetMouseButtonReleasedEventListenrs(windowId winId);
        
        void unsetMouseMovedListenrs(windowId winId);
        void unsetMouseScrollListenrs(windowId winId);

        void unsetCloseEventeListenrs(windowId winId);
        void unsetResizeEventeListenrs(windowId winId);
        void unsetGainFocusEventListeners(windowId winId);
        void unsetLostFocusEventListeners(windowId winId);
        void unsetRenderEventListeners(windowId winId);

};

