#include "log.hpp"
#include "osAPI.hpp"

#include <Tracy.hpp>

#include <thread>
#include <functional>
#include <iostream>
#include <sys/mman.h>

osAPI *a;
bool run = true;

void keyListener(windowId winId, const keyData& sendor)
{
    // LOG_INFO(winId.index << ", " << sendor.utf8Buffer << ", " << keyCodeToString(sendor.value) << "(" << (int)sendor.value << ")");
}


void mouseMove(windowId winId, const mouseMoveData& sendor)
{
    // LOG_INFO("pointer motion (" << a->getWindowTitle(winId) <<"): "<< sendor.x << ", " << sendor.y);
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

void windowResize(windowId winId, const windowResizeData& sendor)
{
    // LOG_INFO(sendor.height << ", " << sendor.width << ", " << (int)sendor.state)

}


void windowClose(windowId winId)
{
    LOG_INFO("close window(" << a->getWindowTitle(winId) <<")")
    run = false;
}
void focusSwap(windowId winId)
{
    // LOG_INFO("focus swap window(" << a->getWindowTitle(winId) <<")")
}

void cpuRender(windowId winId, double *offset, uint32_t size, const windowRenderData& sendor)
{
    ZoneScoped;

    *offset += (sendor.deltaTime / 1000.0 * 24);
    if(*offset > size * 2)
        *offset =  0;

    for (int y = 0; y < sendor.height; ++y) {
        for (int x = 0; x < sendor.width; ++x) {
            if ((x + (uint32_t)(*offset) + y / size * size) % (size * 2) < size)
                sendor.data[y * sendor.width + x] = 0xFFEEEEEE;
            else
                sendor.data[y * sendor.width + x] = 0xFF111111;
        }
    }
}

void consoleKeyWait()
{
    std::getchar();
    run = false;
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
        a->setResizeEventeListenrs(id, std::bind(windowResize, id, std::placeholders::_1));
        a->setCloseEventeListenrs(id, std::bind(windowClose, id));
        a->setGainFocusEventListeners(id, std::bind(focusSwap, id));
        a->setLostFocusEventListeners(id, std::bind(focusSwap, id));
        a->setRenderEventListeners(id, std::bind(cpuRender, id, new double(0), (id.index + 1) * 16, std::placeholders::_1));
    }
    
    std::thread(consoleKeyWait).detach();
    while(run){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::putchar('a');
    delete a;
    LOG_INFO("exits");
    
    logger::close();
    return 0;
}
