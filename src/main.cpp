#include "log.hpp"
#include "osAPI.hpp"

#include <Tracy.hpp>
#include <thread>

void keyListener(const keyData& sendor)
{
    LOG_INFO(sendor.utf8Buffer << ", " << keyCodeToString(sendor.value) << "(" << (int)sendor.value << ")");

}

int main()
{
    ZoneScoped;
    logger::init("./logs/", "TBD");
    
    osAPI *a = new osAPI();
    
    a->closeWindow(a->createWindow({"test 2", 800, 800}));
    a->createWindow({"test 3", 64*7, 64*11});
    a->createWindow({"test 4", 64*9, 64*11});
    windowId win1Id =  a->createWindow({"test 1", 64*5, 64*11});
     
    a->setKeyPressEventListenrs(win1Id, keyListener);
    a->setKeyReleasedEventListenrs(win1Id, keyListener);
    a->setKeyRepeatEventListenrs(win1Id, keyListener);
    
    std::getchar();
    delete a;
    LOG_INFO("exits");
    
    logger::close();
    
    return 0;
}
