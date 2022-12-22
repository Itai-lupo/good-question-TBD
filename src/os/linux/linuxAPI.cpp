#ifdef __linux__

#include "osAPI.hpp"
#include "linuxWindowAPI.hpp"
#include "keyboard.hpp"
#include "pointer.hpp"
#include "osEventsData.hpp"
#include "surface.hpp"
#include "cpuRendering.hpp"
#include "openGLRendering.hpp"
#include "toplevel.hpp"
#include "layer.hpp"

#include "log.hpp"
#include "Tracy.hpp"

osAPI::~osAPI()
{
    linuxWindowAPI::closeApi();    
}

osAPI::osAPI()
{    
    linuxWindowAPI::init();
}

windowId osAPI::createWindow(const windowSpec& windowToCreate)
{
    return linuxWindowAPI::createWindow(windowToCreate);
}

void osAPI::attachSubSurfaceToWindow(windowId id,  const subSurfaceSpec& subSurfaceToAttach)
{
    linuxWindowAPI::windowsInfo->getComponent(id)->subsurfaces[subSurfaceToAttach.subSurfaceSlot] = 
        surface::allocateSurface(id, {
            surfaceRule::subsurface,
            subSurfaceToAttach.renderAPI,
            subSurfaceToAttach.width,
            subSurfaceToAttach.height,
            subSurfaceToAttach.x,
            subSurfaceToAttach.y,
            .parentSurface = linuxWindowAPI::windowsInfo->getComponent(id)->topLevelSurface,
            .cpuRenderFunction = subSurfaceToAttach.cpuRenderFunction
        });    
}


bool osAPI::isWindowOpen(windowId winId)
{
    return linuxWindowAPI::isWindowOpen(winId);
}


void osAPI::closeWindow(windowId winId)
{
    linuxWindowAPI::closeWindow(winId);
}


void* osAPI::getProcAddress()
{
    return nullptr;    
}



std::string osAPI::getWindowTitle(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    return toplevel::getWindowTitle(id); 
    layer::getWindowTitle(id);
}

std::pair<uint32_t, uint32_t> osAPI::getWindowSize(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    return {surface::getWindowWidth(id), surface::getWindowHeight(id)};

}


// ################ set event listener ################################################################
void osAPI::setKeyPressEventListeners(windowId winId, void(*callback)(const keyData&))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    keyboard::setKeyPressEventListeners(id, callback);
}
void osAPI::setKeyReleasedEventListeners(windowId winId, void(*callback)(const keyData&))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    keyboard::setKeyReleasedEventListeners(id, callback);
}
void osAPI::setKeyRepeatEventListeners(windowId winId, void(*callback)(const keyData&))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    keyboard::setKeyRepeatEventListeners(id, callback);
}

void osAPI::setMouseButtonPressEventListeners(windowId winId, void(*callback)(const mouseButtonData&))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    pointer::setMouseButtonPressEventListeners(id, callback);
}
void osAPI::setMouseButtonReleasedEventListeners(windowId winId, void(*callback)(const mouseButtonData&))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    pointer::setMouseButtonReleasedEventListeners(id, callback);
}

void osAPI::setMouseMovedListeners(windowId winId, void(*callback)(const mouseMoveData&))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    pointer::setMouseMovedListeners(id, callback);
}
void osAPI::setMouseScrollListeners(windowId winId, void(*callback)(const mouseScrollData&))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    pointer::setMouseScrollListeners(id, callback);
}

void osAPI::setCloseEventeListeners(windowId winId, void(*callback)(surfaceId winId))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    toplevel::setCloseEventListener(id, callback);
    layer::setCloseEventListener(id, callback);
}

void osAPI::setResizeEventeListeners(windowId winId, void(*callback)(const windowResizeData&))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    toplevel::setResizeEventListener(id, callback);
    layer::setResizeEventListener(id, callback);
}

void osAPI::setGainFocusEventListeners(windowId winId, void(*callback)(surfaceId))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    keyboard::setGainFocusEventListeners(id, callback);
}

void osAPI::setLostFocusEventListeners(windowId winId, void(*callback)(surfaceId))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    keyboard::setLostFocusEventListeners(id, callback);
}

void osAPI::setRenderEventListeners(windowId winId, void(*callback)(const gpuRenderData&))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    switch (surface::surfacesInfo->getComponent(id)->rendererType)
    {
        case surfaceRenderAPI::openGL:
            openGLRendering::setRenderEventListeners(id, callback);
            break;

        
        default:
            break;
    }
}

void osAPI::setRenderEventListeners(windowId winId, void(*callback)(const cpuRenderData&))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    switch (surface::surfacesInfo->getComponent(id)->rendererType)
    {
        case surfaceRenderAPI::cpu:
            cpuRendering::setRenderEventListeners(id, callback);
            break;
        
        default:
            LOG_FATAL("render function and render api type mismatch");
            break;
    }
}

void osAPI::setsubSurfaceRenderEventListeners(windowId winId, int subSurfaceSlot, void(*callback)(const cpuRenderData&))
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->subsurfaces[subSurfaceSlot];
    cpuRendering::setRenderEventListeners(id, callback);
}  


// ################ unset event listener ################################################################
void osAPI::unsetKeyPressEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    keyboard::unsetKeyPressEventListeners(id);
}
void osAPI::unsetKeyReleasedEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    keyboard::unsetKeyReleasedEventListeners(id);
}
void osAPI::unsetKeyRepeatEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    keyboard::unsetKeyRepeatEventListeners(id);
}


void osAPI::unsetMouseButtonPressEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    pointer::unsetMouseButtonPressEventListeners(id);
}
void osAPI::unsetMouseButtonReleasedEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    pointer::unsetMouseButtonReleasedEventListeners(id);
}

void osAPI::unsetMouseMovedListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    pointer::unsetMouseMovedListeners(id);
}
void osAPI::unsetMouseScrollListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    pointer::unsetMouseScrollListeners(id);
}

void osAPI::unsetCloseEventeListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    toplevel::unsetCloseEventListener(id);
    layer::unsetCloseEventListener(id);
}

void osAPI::unsetResizeEventeListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    toplevel::unsetResizeEventListener(id);
    layer::unsetResizeEventListener(id);
}

void osAPI::unsetGainFocusEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    keyboard::unsetGainFocusEventListeners(id);
}

void osAPI::unsetLostFocusEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo->getComponent(winId)->topLevelSurface;
    keyboard::unsetLostFocusEventListeners(id);
}


#endif