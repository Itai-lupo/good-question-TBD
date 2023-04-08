#include "log.hpp"
#include "osAPI.hpp"
#include "renderApi.hpp"

#include <Tracy.hpp>

#include <thread>
#include <functional>
#include <iostream>
#include <sys/mman.h>
#include "stb_image.h"


osAPI *a;
bool run = true;

float *vertices3 = new float[]{
    0.5f,    0.5f,    0.0f,          1.0f, 1.0f,
    0.5f,   -0.5f,    0.0f,          1.0f, 0,
    -0.5f,  -0.5f,   0.0f,           0, 0,
    -0.5f,   0.5f,   0.0f,           0.0f, 1.0f,
};  

float *vertices1 = new float[]{
    0.5f,    0.5f,    0.0f,           
    0.5f,   -0.5f,    0.0f,           
    -0.5f,  -0.5f,   0.0f,           
    -0.5f,   0.5f,   0.0f,            
};  

float *vertices2 = new float[]{
    1.0f, 1.0f,
    1.0f, 0,
    0, 0,
    0.0f, 1.0f,
};  

uint32_t *indcies = new uint32_t[]{
    0, 1, 2,
    0, 3, 2
};

void keyListener(const keyData& sendor)
{
    LOG_INFO(sendor.id.index << ", " << sendor.utf8Buffer << ", " << keyCodeToString(sendor.value) << "(" << (int)sendor.value << ")");
}


void mouseMove(const mouseMoveData& sendor)
{
    // LOG_INFO("pointer motion (" << a->getWindowTitle(sendor.id) <<"): "<< sendor.x << ", " << sendor.y);
}

void mousePress(const mouseButtonData& sendor)
{
    LOG_INFO("pointer button press (" <<  a->getWindowTitle(sendor.id) << "): " << mouseButtonToString(sendor.value));
}

void mouseReleased(const mouseButtonData& sendor)
{
    LOG_INFO("pointer button released (" <<  a->getWindowTitle(sendor.id) <<"): " << mouseButtonToString(sendor.value));
}


void mouseScroll(const mouseScrollData& sendor)
{
    LOG_INFO("pointer axis " << mouseAxisToString(sendor.axis) << "from source " << mouseAxisSourceToString(sendor.axisSource) << " = " << sendor.discreteValue << " or " << sendor.value);
}

void windowResize(const windowResizeData& sendor)
{
    // LOG_INFO(sendor.height << ", " << sendor.width << ", " << (int)sendor.state)

}


void windowClose(surfaceId winId)
{
    LOG_INFO("close window(" << a->getWindowTitle(winId) <<")")
    run = false;
}

void focusSwap(surfaceId winId)
{
    LOG_INFO("focus swap window(" << a->getWindowTitle(winId) <<")")
}

void cpuRender(const cpuRenderData& sendor)
{
    {
        static double *offset = new double();
        static uint32_t size = 16;

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
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
}


void gpuRender(const gpuRenderData& sendor)
{
    {
        static bool temp = false;
        static bool tempFinsh = false;
        static vaoId shapeToRender;
        static vaoId shapeToRender2;
        static uniformBufferId UBO;
        static shaderId shaderToRender;
        static textureId testTexture;
        static stbi_uc* data = nullptr;
        static int width, height, channels;

        if(!temp)
        {
            temp = true;

            shapeToRender = sendor.api->allocVao(sendor.apiType);
            shapeToRender2 = sendor.api->allocVao(sendor.apiType);
            shaderToRender = sendor.api->allocShader(sendor.apiType);
            testTexture = sendor.api->allocTexture(sendor.apiType);
            UBO = sendor.api->allocUniformBuffer(sendor.apiType);
            
            sendor.api->setVao({
                shapeToRender2,
                .iboToUpdate = new IBOUpdateRequst{indcies, 6},
                .bindingSlotsToUpdate[0] = new VBOUpdateRequst{vertices3, 80, 5 * sizeof(float)},
                .attacmentsToUpdate[0] = new attacmentUpdateRequst{0, 3},
                .attacmentsToUpdate[1] = new attacmentUpdateRequst{0, 2, 3 * sizeof(float)},
                });

            sendor.api->setVao({
                shapeToRender,
                .iboToUpdate = new IBOUpdateRequst{indcies, 6},
                .bindingSlotsToUpdate[0] = new VBOUpdateRequst{vertices1, 48, 3 * sizeof(float)},
                .bindingSlotsToUpdate[1] = new VBOUpdateRequst{vertices2, 32, 2 * sizeof(float)},
                .attacmentsToUpdate[0] = new attacmentUpdateRequst{0, 3},
                .attacmentsToUpdate[1] = new attacmentUpdateRequst{1, 2},
                });
                
            sendor.api->setShader({shaderToRender, "assets/shaders/vertex/texture.vert.spv", "assets/shaders/fragment/texture.frag.spv"});
            stbi_set_flip_vertically_on_load(1);
            
            data = stbi_load("assets/textures/preview-simple-dungeon-crawler-set1.png", &width, &height, &channels, 0);
            textureInfo tempTex(testTexture, (uint32_t)width, (uint32_t)height, textureFormat::RGB8);
            tempTex.bufferToLoad = new loadTextureRequst{
                0, 0, (uint32_t)width, (uint32_t)height, textureFormat::RGB8, 0x1401, data
            };

            sendor.api->setTexture(tempTex);
            
            float *temp = (float *)malloc(16);
            temp[0] = 0.5f;
            temp[1] = 0.5f;
    
            sendor.api->setUniformBuffer(uniformBufferInfo(UBO, new uniformBufferRequst{(uint8_t *)temp, 16}));
            
            tempFinsh = true; 
        }

        while (!tempFinsh)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            
        
        sendor.api->renderRequest({
        .frameBufferId = sendor.buffer, 
        .drawCalls = {{
                .shader = shaderToRender, 
                .vertexArrayId = shapeToRender2, 
                .texturesIds = {[0] = testTexture,
                                [1 ... 31] = {16777215, 255}}, 
                .UBO = UBO,
                .mode = renderMode::triangles},
                {
                .shader = shaderToRender, 
                .vertexArrayId = shapeToRender, 
                .texturesIds = {[0] = testTexture,
                                [1 ... 31] = {16777215, 255}}, 
                .UBO = UBO,
                .mode = renderMode::triangles} }
        });
    
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
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
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    winowsIds.push_back(a->createWindow({"test 3", 64*7, 64*11, .gpuRenderFunction = gpuRender}));
    // winowsIds.push_back(a->createWindow({"test 4", 64*9, 64*11, .gpuRenderFunction = gpuRender}));
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
        a->setKeyPressEventListeners(id, keyListener);
        a->setKeyReleasedEventListeners(id, keyListener);
        a->setKeyRepeatEventListeners(id, keyListener);
        a->setMouseMovedListeners(id, mouseMove);
        a->setMouseButtonPressEventListeners(id, mousePress);
        a->setMouseButtonReleasedEventListeners(id, mouseReleased);
        a->setMouseScrollListeners(id, mouseScroll);
        a->setResizeEventeListeners(id, windowResize);
        a->setCloseEventeListeners(id, windowClose);
        a->setGainFocusEventListeners(id, focusSwap);
        a->setLostFocusEventListeners(id, focusSwap);

        
        a->setRenderEventListeners(id, gpuRender);
        a->attachSubSurfaceToWindow(id, {0, 200, 200, 100, 100, cpuRender});
        
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
