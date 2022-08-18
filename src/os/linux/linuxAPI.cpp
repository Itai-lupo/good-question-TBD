#ifdef __linux__

#include "osAPI.hpp"
#include "linuxWindowAPI.hpp"
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



int osAPI::pollEvents()
{
    ZoneScoped;
    return wl_display_dispatch(linuxWindowAPI::getDisplay());
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


#endif