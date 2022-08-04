#include "log.hpp"
#include "osAPI.hpp"
#include "osAPI.hpp"

#include <wayland-client.h>
#include <xdg-shell-client-protocol.h>


int main()
{
    logger::init("./logs/", "TBD");
    
    osAPI *a = osAPI::init();
    
    a->createWindow(new windowSpec{"test 1", 800, 800});
    a->createWindow(new windowSpec{"test 2", 200, 800});
    
    while (a->pollEvents()) {
        /* This space deliberately left blank */
    }
    std::getchar();
    LOG_INFO("exits");
    
    logger::close();
    
    return 0;
}