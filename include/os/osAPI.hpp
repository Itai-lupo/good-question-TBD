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
    openGL,
    vulkan
};


struct windowSpec
{
	
	std::string title;
    int w, h;
    surfaceRenderAPI renderAPI =  surfaceRenderAPI::vulkan;

    void (*cpuRenderFunction)(const cpuRenderData &);
    void (*gpuRenderFunction)(const gpuRenderData &);
};

struct subSurfaceSpec
{
    uint8_t subSurfaceSlot;
    int x, y;
    int width, height;
    void (*cpuRenderFunction)(const cpuRenderData &);
    void (*gpuRenderFunction)(const gpuRenderData &);
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
        void closeWindow(windowId winId);
        
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
        void setRenderEventListeners(windowId winId, void(*callback)(const gpuRenderData&));
        void setRenderEventListeners(windowId winId, void(*callback)(const cpuRenderData&));
        void setsubSurfaceRenderEventListeners(windowId winId, int subSurfaceSlot, void(*callback)(const gpuRenderData&));
        void setsubSurfaceRenderEventListeners(windowId winId, int subSurfaceSlot, void(*callback)(const cpuRenderData&));
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

