#ifdef __linux__

#include "osAPI.hpp"
#include "linuxWindowAPI.hpp"
#include "keyboard.hpp"
#include "pointer.hpp"
#include "osEventsData.hpp"

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
    return linuxWindowAPI::getWindowTitle(winId);
}

std::pair<uint32_t, uint32_t> osAPI::getWindowSize(windowId winId)
{
    return linuxWindowAPI::getWindowSize(winId);

}


// ################ set event listener ################################################################
void osAPI::setKeyPressEventListeners(windowId winId, std::function<void(const keyData&)> callback)
{
    keyboard::setKeyPressEventListeners(winId, callback);
}
void osAPI::setKeyReleasedEventListeners(windowId winId, std::function<void(const keyData&)> callback)
{
    keyboard::setKeyReleasedEventListeners(winId, callback);
}
void osAPI::setKeyRepeatEventListeners(windowId winId, std::function<void(const keyData&)> callback)
{
    keyboard::setKeyRepeatEventListeners(winId, callback);
}

void osAPI::setMouseButtonPressEventListeners(windowId winId, std::function<void(const mouseButtonData&)> callback)
{
    pointer::setMouseButtonPressEventListeners(winId, callback);
}
void osAPI::setMouseButtonReleasedEventListeners(windowId winId, std::function<void(const mouseButtonData&)> callback)
{
    pointer::setMouseButtonReleasedEventListeners(winId, callback);
}

void osAPI::setMouseMovedListeners(windowId winId, std::function<void(const mouseMoveData&)> callback)
{
    pointer::setMouseMovedListeners(winId, callback);
}
void osAPI::setMouseScrollListeners(windowId winId, std::function<void(const mouseScrollData&)> callback)
{
    pointer::setMouseScrollListeners(winId, callback);
}

void osAPI::setCloseEventeListeners(windowId winId, std::function<void()> callback)
{
    linuxWindowAPI::setCloseEventeListeners(winId, callback);
}

void osAPI::setResizeEventeListeners(windowId winId, std::function<void(const windowResizeData&)> callback)
{
    linuxWindowAPI::setResizeEventeListeners(winId, callback);
}

void osAPI::setGainFocusEventListeners(windowId winId, std::function<void()> callback)
{
    keyboard::setGainFocusEventListeners(winId, callback);
}

void osAPI::setLostFocusEventListeners(windowId winId, std::function<void()> callback)
{
    keyboard::setLostFocusEventListeners(winId, callback);
}

void osAPI::setRenderEventListeners(windowId winId, std::function<void(const windowRenderData&)> callback)
{
    linuxWindowAPI::setRenderEventListeners(winId, callback);
}

        


// ################ unset event listener ################################################################
void osAPI::unsetKeyPressEventListeners(windowId winId)
{
    keyboard::unsetKeyPressEventListeners(winId);
}
void osAPI::unsetKeyReleasedEventListeners(windowId winId)
{
    keyboard::unsetKeyReleasedEventListeners(winId);
}
void osAPI::unsetKeyRepeatEventListeners(windowId winId)
{
    keyboard::unsetKeyRepeatEventListeners(winId);
}


void osAPI::unsetMouseButtonPressEventListeners(windowId winId)
{
    pointer::unsetMouseButtonPressEventListeners(winId);
}
void osAPI::unsetMouseButtonReleasedEventListeners(windowId winId)
{
    pointer::unsetMouseButtonReleasedEventListeners(winId);
}

void osAPI::unsetMouseMovedListeners(windowId winId)
{
    pointer::unsetMouseMovedListeners(winId);
}
void osAPI::unsetMouseScrollListeners(windowId winId)
{
    pointer::unsetMouseScrollListeners(winId);
}

void osAPI::unsetCloseEventeListeners(windowId winId)
{
    linuxWindowAPI::unsetCloseEventeListeners(winId);
}

void osAPI::unsetResizeEventeListeners(windowId winId)
{
    linuxWindowAPI::unsetResizeEventeListeners(winId);
}

void osAPI::unsetGainFocusEventListeners(windowId winId)
{
    keyboard::unsetGainFocusEventListeners(winId);
}

void osAPI::unsetLostFocusEventListeners(windowId winId)
{
    keyboard::unsetLostFocusEventListeners(winId);
}

void osAPI::unsetRenderEventListeners(windowId winId)
{
    linuxWindowAPI::unsetRenderEventListeners(winId);
}



#endif