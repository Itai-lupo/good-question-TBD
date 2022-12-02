#include "log.hpp"
#include "osAPI.hpp"

#include <Tracy.hpp>

#include <thread>
#include <functional>
#include <iostream>
#include <sys/mman.h>
#include <stb_image.h>


osAPI *a;
bool run = true;

constexpr float vertices3[] = {
    0.5f,    0.5f,    0.0f,          1.0f, 1.0f,
    0.5f,   -0.5f,    0.0f,          1.0f, 0,
    -0.5f,  -0.5f,   0.0f,           0, 0,
    -0.5f,   0.5f,   0.0f,           0.0f, 1.0f,
};  

constexpr float vertices1[] = {
    0.5f,    0.5f,    0.0f,           
    0.5f,   -0.5f,    0.0f,           
    -0.5f,  -0.5f,   0.0f,           
    -0.5f,   0.5f,   0.0f,            
};  

constexpr float vertices2[] = {
    1.0f, 1.0f,
    1.0f, 0,
    0, 0,
    0.0f, 1.0f,
};  

constexpr uint32_t indcies[] = {
    0, 1, 2,
    0, 3, 2
};

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
    {
        ZoneScoped;
        *offset += (sendor.deltaTime / 100.0 * 24);
        for (int y = 0; y < sendor.height; ++y) {
            for (int x = 0; x < sendor.width; x ++) 
            {

                ((uint8_t*)&sendor.data[y * sendor.width + x])[0] = ((y * sendor.width + x)*4 + *offset);
                ((uint8_t*)&sendor.data[y * sendor.width + x])[1] = 0;
                ((uint8_t*)&sendor.data[y * sendor.width + x])[2] = 0;
                ((uint8_t*)&sendor.data[y * sendor.width + x])[3] = 0xFF;
            }
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}


void gpuRender(double *offset, uint32_t size, const windowRenderData& sendor)
{
    {
        ZoneScoped;
        vaoId shapeToRender =  sendor.api->shapes->createVao();
        sendor.api->shapes->attachIndexBuffer(shapeToRender, indcies, 6);


        sendor.api->shapes->addVertexBufferBinding(shapeToRender, {
            0,
            vertices3,
            sizeof(vertices3),
            5 * sizeof(GLfloat)
        });


        sendor.api->shapes->addVertexBufferAttacment(shapeToRender, 0, 0, 3);
        sendor.api->shapes->addVertexBufferAttacment(shapeToRender, 1, 0, 2, 3 * sizeof(GLfloat));

        shaderId shaderToRender = sendor.api->shaders->createProgram("assets/shaders/vertex/texture.vert.spv", "assets/shaders/fragment/texture.frag.spv");
        
        

        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = nullptr;
        
        int width, height, channels;
        data = stbi_load("assets/textures/preview-simple-dungeon-crawler-set1.png", &width, &height, &channels, 0);
        textureId testTexture = sendor.api->textures->createTexture(textureFormat::RGB8, width, height);
        sendor.api->textures->loadBuffer(testTexture, 0, 0, width, height, textureFormat::RGB8, GL_UNSIGNED_BYTE, data);
            
        sendor.api->renderRequest({
        .frameBufferId = sendor.buffer, 
        .drawCalls = {{
                .shader = shaderToRender, 
                .vertexArrayId = shapeToRender, 
                .texturesIds = {[0] = testTexture,
                                [1 ... 31] = {255, 16777215}}, 
                .mode = renderMode::triangles} }
    });
    
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
    // winowsIds.push_back(a->createWindow({"test 5", 1025, 1025}));
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
    // winowsIds.push_back(a->createWindow({"test 2", 64*5, 64*11}));

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

        //to do fix the tearing that is coused from using the same variable for both top level rendring and sub surface
        double *temp =  new double(0);
        
        a->setRenderEventListeners(id, [=]( const windowRenderData& sendor){gpuRender(temp, (id.index + 1) * 16, sendor);});
        a->attachSubSurfaceToWindow(id, {0, 200, 200, 100, 100});
        a->setsubSurfaceRenderEventListeners(id, 0, [=]( const windowRenderData& sendor){cpuRender(temp, (id.index + 1) * 16, sendor);});
        
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
