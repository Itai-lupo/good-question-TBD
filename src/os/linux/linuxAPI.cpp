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

uint64_t linuxAPI::createWindow(windowSpec *windowToCreate)
{
    return windowApi->createWindow(windowToCreate);
}



int linuxAPI::pollEvents()
{
    return wl_display_dispatch(windowApi->display);
}

bool linuxAPI::isWindowOpen(uint64_t winId)
{
    
}

void linuxAPI::setVSyncForCurrentContext(bool enabled)
{
    
}

void linuxAPI::makeContextCurrent(uint64_t winId)
{
    
}

void linuxAPI::closeWindow(uint64_t winId)
{
    
}

void linuxAPI::swapBuffers(uint64_t winId)
{
    
}

uint64_t linuxAPI::getCurrentContextWindowId()
{
    
}

void* linuxAPI::getProcAddress()
{
    
}


#endif