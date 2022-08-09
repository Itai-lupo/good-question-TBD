#include "log.hpp"
#include "osAPI.hpp"
#include "osAPI.hpp"



int main()
{
    logger::init("./logs/", "TBD");
    
    osAPI *a = osAPI::init();
    
    a->createWindow({"test 1", 64*5, 64*11});
    a->closeWindow(a->createWindow({"test 2", 800, 800}));
    a->createWindow({"test 3", 64*7, 64*11});
    a->createWindow({"test 4", 64*9, 64*11});
    
    
    
    while (a->pollEvents()) {
        /* This space deliberately left blank */
    }
    std::getchar();
    LOG_INFO("exits");
    
    logger::close();
    
    return 0;
}