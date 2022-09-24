#ifdef __linux__

#include "osAPI.hpp"
#include "linuxWindowAPI.hpp"
#include "keyboard.hpp"
#include "pointer.hpp"
#include "osEventsData.hpp"
#include "surface.hpp"
#include "cpuRendering.hpp"
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
    linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[id.index].index].subsurfaces[subSurfaceToAttach.subSurfaceSlot] = 
        surface::allocateSurface(id, {
            surfaceRule::subsurface,
            subSurfaceToAttach.width,
            subSurfaceToAttach.height,
            subSurfaceToAttach.x,
            subSurfaceToAttach.y,
            .parentSurface = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[id.index].index].topLevelSurface
        });    
}


bool osAPI::isWindowOpen(windowId winId)
{
    return linuxWindowAPI::isWindowOpen(winId);
}

void osAPI::setVSyncForCurrentContext(bool enabled)
{
    
}

void osAPI::makeContextCurrent(windowId winId)
{
    
}

void osAPI::closeWindow(windowId winId)
{
    linuxWindowAPI::closeWindow(winId);
}

void osAPI::swapBuffers(windowId winId)
{
    
}

windowId osAPI::getCurrentContextWindowId()
{
    
}

void* osAPI::getProcAddress()
{
    
}



std::string osAPI::getWindowTitle(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    return layer::getWindowTitle(id);
}

std::pair<uint32_t, uint32_t> osAPI::getWindowSize(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    return {surface::getWindowWidth(id), surface::getWindowHeight(id)};

}


// ################ set event listener ################################################################
void osAPI::setKeyPressEventListeners(windowId winId, std::function<void(const keyData&)> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    keyboard::setKeyPressEventListeners(id, callback);
}
void osAPI::setKeyReleasedEventListeners(windowId winId, std::function<void(const keyData&)> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    keyboard::setKeyReleasedEventListeners(id, callback);
}
void osAPI::setKeyRepeatEventListeners(windowId winId, std::function<void(const keyData&)> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    keyboard::setKeyRepeatEventListeners(id, callback);
}

void osAPI::setMouseButtonPressEventListeners(windowId winId, std::function<void(const mouseButtonData&)> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    pointer::setMouseButtonPressEventListeners(id, callback);
}
void osAPI::setMouseButtonReleasedEventListeners(windowId winId, std::function<void(const mouseButtonData&)> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    pointer::setMouseButtonReleasedEventListeners(id, callback);
}

void osAPI::setMouseMovedListeners(windowId winId, std::function<void(const mouseMoveData&)> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    pointer::setMouseMovedListeners(id, callback);
}
void osAPI::setMouseScrollListeners(windowId winId, std::function<void(const mouseScrollData&)> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    pointer::setMouseScrollListeners(id, callback);
}

void osAPI::setCloseEventeListeners(windowId winId, std::function<void()> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    layer::setCloseEventListener(id, callback);
}

void osAPI::setResizeEventeListeners(windowId winId, std::function<void(const windowResizeData&)> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    layer::setResizeEventListener(id, callback);
}

void osAPI::setGainFocusEventListeners(windowId winId, std::function<void()> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    keyboard::setGainFocusEventListeners(id, callback);
}

void osAPI::setLostFocusEventListeners(windowId winId, std::function<void()> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    keyboard::setLostFocusEventListeners(id, callback);
}

void osAPI::setRenderEventListeners(windowId winId, std::function<void(const windowRenderData&)> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    cpuRendering::setRenderEventListeners(id, callback);
}

void osAPI::setsubSurfaceRenderEventListeners(windowId winId, int subSurfaceSlot, std::function<void(const windowRenderData&)> callback)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].subsurfaces[subSurfaceSlot];
    cpuRendering::setRenderEventListeners(id, callback);
}  


// ################ unset event listener ################################################################
void osAPI::unsetKeyPressEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    keyboard::unsetKeyPressEventListeners(id);
}
void osAPI::unsetKeyReleasedEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    keyboard::unsetKeyReleasedEventListeners(id);
}
void osAPI::unsetKeyRepeatEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    keyboard::unsetKeyRepeatEventListeners(id);
}


void osAPI::unsetMouseButtonPressEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    pointer::unsetMouseButtonPressEventListeners(id);
}
void osAPI::unsetMouseButtonReleasedEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    pointer::unsetMouseButtonReleasedEventListeners(id);
}

void osAPI::unsetMouseMovedListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    pointer::unsetMouseMovedListeners(id);
}
void osAPI::unsetMouseScrollListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    pointer::unsetMouseScrollListeners(id);
}

void osAPI::unsetCloseEventeListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    layer::unsetCloseEventListener(id);
}

void osAPI::unsetResizeEventeListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    layer::unsetResizeEventListener(id);
}

void osAPI::unsetGainFocusEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    keyboard::unsetGainFocusEventListeners(id);
}

void osAPI::unsetLostFocusEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    keyboard::unsetLostFocusEventListeners(id);
}

void osAPI::unsetRenderEventListeners(windowId winId)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].topLevelSurface;
    cpuRendering::unsetRenderEventListeners(id);
}

void osAPI::unsetsubSurfaceRenderEventListeners(windowId winId, int subSurfaceSlot)
{
    surfaceId id = linuxWindowAPI::windowsInfo[linuxWindowAPI::idToIndex[winId.index].index].subsurfaces[subSurfaceSlot];
    cpuRendering::unsetRenderEventListeners(id);
}

#endif