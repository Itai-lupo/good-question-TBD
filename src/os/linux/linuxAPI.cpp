#ifdef __linux__

#include "linuxAPI.hpp"
#include "log.hpp"

osAPI *osAPI::init()
{
    return new linuxAPI();
}

linuxAPI::~linuxAPI()
{
    delete windowApi;
}

linuxAPI::linuxAPI()
{
    
    windowApi = new linuxWindowAPI();
}

windowId linuxAPI::createWindow(const windowSpec& windowToCreate)
{
    return windowApi->createWindow(windowToCreate);
}



int linuxAPI::pollEvents()
{
    return wl_display_dispatch(windowApi->getDisplay());
}

bool linuxAPI::isWindowOpen(windowId winId)
{
    
}

void linuxAPI::setVSyncForCurrentContext(bool enabled)
{
    
}

void linuxAPI::makeContextCurrent(windowId winId)
{
    
}

void linuxAPI::closeWindow(windowId winId)
{
    windowApi->closeWindow(winId);
}

void linuxAPI::swapBuffers(windowId winId)
{
    
}

windowId linuxAPI::getCurrentContextWindowId()
{
    
}

void* linuxAPI::getProcAddress()
{
    
}


#endif