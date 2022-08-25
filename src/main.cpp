#include "log.hpp"
#include "osAPI.hpp"

#include <Tracy.hpp>

#include <thread>
#include <functional>

osAPI *a;

void keyListener(windowId winId, const keyData& sendor)
{
    LOG_INFO(winId.index << ", " << sendor.utf8Buffer << ", " << keyCodeToString(sendor.value) << "(" << (int)sendor.value << ")");
}


void mouseMove(windowId winId, const mouseMoveData& sendor)
{
    LOG_INFO("pointer motion (" << a->getWindowTitle(winId) <<"): "<< sendor.x << ", " << sendor.y);
}

void mousePress(windowId winId, const mouseButtonData& sendor)
{
    LOG_INFO("pointer button press (" <<  a->getWindowTitle(winId) << "): " << mouseButtonToString(sendor.value));
}

void mouseReleased(windowId winId, const mouseButtonData& sendor)
{
    LOG_INFO("pointer button released (" <<  a->getWindowTitle(winId) <<"): " << mouseButtonToString(sendor.value));
}


void mouseScroll(windowId winId, const mouseScrollData& sendor)
{
    LOG_INFO("pointer axis " << mouseAxisToString(sendor.axis) << "from source " << mouseAxisSourceToString(sendor.axisSource) << " = " << sendor.discreteValue << " or " << sendor.value);
}


int main()
{
    ZoneScoped;
    logger::init("./logs/", "TBD");
    
    a = new osAPI();

    std::vector<windowId> winowsIds;
    a->closeWindow(a->createWindow({"test 2", 800, 800}));
    winowsIds.push_back(a->createWindow({"test 3", 64*7, 64*11}));
    winowsIds.push_back(a->createWindow({"test 4", 64*9, 64*11}));
    winowsIds.push_back( a->createWindow({"test 1", 64*5, 64*11}));
    
    for(auto& id: winowsIds)
    {
        a->setKeyPressEventListenrs(id, std::bind(keyListener, id, std::placeholders::_1));
        a->setKeyReleasedEventListenrs(id, std::bind(keyListener, id, std::placeholders::_1));
        a->setKeyRepeatEventListenrs(id, std::bind(keyListener, id, std::placeholders::_1));
        a->setMouseMovedListenrs(id, std::bind(mouseMove, id, std::placeholders::_1));
        a->setMouseButtonPressEventListenrs(id, std::bind(mousePress, id, std::placeholders::_1));
        a->setMouseButtonReleasedEventListenrs(id, std::bind(mouseReleased, id, std::placeholders::_1));
        a->setMouseScrollListenrs(id, std::bind(mouseScroll, id, std::placeholders::_1));
    }
    
    std::getchar();
    delete a;
    LOG_INFO("exits");
    
    logger::close();
    
    return 0;
}
