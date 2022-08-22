#ifdef __linux__

#include "osAPI.hpp"
#include "linuxWindowAPI.hpp"
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
    linuxWindowAPI::isWindowOpen(winId);
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



// ################ set event listener ################################################################
void osAPI::setKeyPressEventListenrs(windowId winId, std::function<void(const keyData&)> callback)
{
    linuxWindowAPI::setKeyPressEventListenrs(winId, callback);
}
void osAPI::setKeyReleasedEventListenrs(windowId winId, std::function<void(const keyData&)> callback)
{
    linuxWindowAPI::setKeyReleasedEventListenrs(winId, callback);
}
void osAPI::setKeyRepeatEventListenrs(windowId winId, std::function<void(const keyData&)> callback)
{
    linuxWindowAPI::setKeyRepeatEventListenrs(winId, callback);
}
void osAPI::setKeyTypedEventListenrs(windowId winId, std::function<void(const KeyTypedData&)> callback)
{
    linuxWindowAPI::setKeyTypedEventListenrs(winId, callback);
}

void osAPI::setMouseButtonPressEventListenrs(windowId winId, std::function<void(const mouseButtonData&)> callback)
{
    linuxWindowAPI::setMouseButtonPressEventListenrs(winId, callback);
}
void osAPI::setMouseButtonReleasedEventListenrs(windowId winId, std::function<void(const mouseButtonData&)> callback)
{
    linuxWindowAPI::setMouseButtonReleasedEventListenrs(winId, callback);
}

void osAPI::setMouseMovedListenrs(windowId winId, std::function<void(const mouseMoveData&)> callback)
{
    linuxWindowAPI::setMouseMovedListenrs(winId, callback);
}
void osAPI::setMouseScrollListenrs(windowId winId, std::function<void(const mouseScrollData&)> callback)
{
    linuxWindowAPI::setMouseScrollListenrs(winId, callback);
}


// ################ unset event listener ################################################################
void osAPI::unsetKeyPressEventListenrs(windowId winId)
{
    linuxWindowAPI::unsetKeyPressEventListenrs(winId);
}
void osAPI::unsetKeyReleasedEventListenrs(windowId winId)
{
    linuxWindowAPI::unsetKeyReleasedEventListenrs(winId);
}
void osAPI::unsetKeyRepeatEventListenrs(windowId winId)
{
    linuxWindowAPI::unsetKeyRepeatEventListenrs(winId);
}
void osAPI::unsetKeyTypedEventListenrs(windowId winId)
{
    linuxWindowAPI::unsetKeyTypedEventListenrs(winId);
}

void osAPI::unsetMouseButtonPressEventListenrs(windowId winId)
{
    linuxWindowAPI::unsetMouseButtonPressEventListenrs(winId);
}
void osAPI::unsetMouseButtonReleasedEventListenrs(windowId winId)
{
    linuxWindowAPI::unsetMouseButtonReleasedEventListenrs(winId);
}

void osAPI::unsetMouseMovedListenrs(windowId winId)
{
    linuxWindowAPI::unsetMouseMovedListenrs(winId);
}
void osAPI::unsetMouseScrollListenrs(windowId winId)
{
    linuxWindowAPI::unsetMouseScrollListenrs(winId);
}


#endif