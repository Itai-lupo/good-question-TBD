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
    LOG_INFO(winId.index << ", " << sendor.utf8Buffer << ", " << keyCodeToString(sendor.value) << "(" << (int)sendor.value << ")");
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
    LOG_INFO("focus swap window(" << a->getWindowTitle(winId) <<")")
}

void cpuRender(double *offset, uint32_t size, const windowRenderData& sendor)
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
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

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
    std::vector<double *> offsets;
    
    winowsIds.push_back(a->createWindow({"test 3", 64*7, 64*11}));
    winowsIds.push_back(a->createWindow({"test 4", 64*9, 64*11}));
    // for (size_t i = 0; i < 10; i++)
    // {    
    //     std::vector<windowId> tempWinowsIds;
    //     for (size_t j = 0; j < 10; j++)
    //     {
    //         tempWinowsIds.push_back(a->createWindow({"test 2", 800, 800}));
    //     }
        
    //     for (size_t j = 0; j < 10; j++)
    //         a->closeWindow(tempWinowsIds[j]);

    //     std::vector<windowId> tempWinowsIds2;
    //     for (size_t j = 0; j < 10; j++)
    //     {
    //         tempWinowsIds2.push_back(a->createWindow({"test 2", 800, 800}));
    //     }
        
    //     for (int j = 9; j >= 0; j--)
    //         a->closeWindow(tempWinowsIds2[j]);
    // }
    winowsIds.push_back(a->createWindow({"test 2", 64*5, 64*11}));

    for(auto& id: winowsIds)
    {
        a->setKeyPressEventListeners(id, std::bind(keyListener, id, std::placeholders::_1));
        a->setKeyReleasedEventListeners(id, std::bind(keyListener, id, std::placeholders::_1));
        a->setKeyRepeatEventListeners(id, std::bind(keyListener, id, std::placeholders::_1));
        a->setMouseMovedListeners(id, std::bind(mouseMove, id, std::placeholders::_1));
        a->setMouseButtonPressEventListeners(id, std::bind(mousePress, id, std::placeholders::_1));
        a->setMouseButtonReleasedEventListeners(id, std::bind(mouseReleased, id, std::placeholders::_1));
        a->setMouseScrollListeners(id, std::bind(mouseScroll, id, std::placeholders::_1));
        a->setResizeEventeListeners(id, std::bind(windowResize, id, std::placeholders::_1));
        a->setCloseEventeListeners(id, std::bind(windowClose, id));
        a->setGainFocusEventListeners(id, std::bind(focusSwap, id));
        a->setLostFocusEventListeners(id, std::bind(focusSwap, id));
        double *temp =  new double(0);
        a->setRenderEventListeners(id, [=]( const windowRenderData& sendor){cpuRender(temp, (id.index + 1) * 16, sendor);});
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
