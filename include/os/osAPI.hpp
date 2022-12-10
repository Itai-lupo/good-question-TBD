#pragma once
#include "core.hpp"
#include "osEventsData.hpp"

#include <string>
#include <functional>
#include <vector>
#include <glm/glm.hpp>
#include <functional>


enum class surfaceRenderAPI
{
    cpu,
    openGL
};


struct windowSpec
{
	windowSpec(std::string title, int w, int h, surfaceRenderAPI renderAPI = surfaceRenderAPI::openGL): w(w), h(h), title(title), renderAPI(renderAPI){}
	
    int w, h;
	std::string title;
    surfaceRenderAPI renderAPI;
};

struct subSurfaceSpec
{
    uint8_t subSurfaceSlot;
    int x, y;
    int width, height;
    surfaceRenderAPI renderAPI = surfaceRenderAPI::cpu;
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
        void setKeyPressEventListeners(windowId winId, void(*callback)(const keyData&));
        void setKeyReleasedEventListeners(windowId winId, void(*callback)(const keyData&));
        void setKeyRepeatEventListeners(windowId winId, void(*callback)(const keyData&));

        void setMouseButtonPressEventListeners(windowId winId, void(*callback)(const mouseButtonData&));
        void setMouseButtonReleasedEventListeners(windowId winId, void(*callback)(const mouseButtonData&));
        
        void setMouseMovedListeners(windowId winId, void(*callback)(const mouseMoveData&));
        void setMouseScrollListeners(windowId winId, void(*callback)(const mouseScrollData&));

        void setCloseEventeListeners(windowId winId, void(*callback)(surfaceId));
        void setResizeEventeListeners(windowId winId, void(*callback)(const windowResizeData&));
        void setGainFocusEventListeners(windowId winId, void(*callback)(surfaceId));
        void setLostFocusEventListeners(windowId winId, void(*callback)(surfaceId));
        void setRenderEventListeners(windowId winId, void(*callback)(const windowRenderData&));
        void setsubSurfaceRenderEventListeners(windowId winId, int subSurfaceSlot, void(*callback)(const windowRenderData&));
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

