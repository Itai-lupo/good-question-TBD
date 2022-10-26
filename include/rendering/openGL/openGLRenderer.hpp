#pragma once

#include "openglContext.hpp"

#include "textureManger.hpp"
#include "vertexArrayManger.hpp"
#include "shaderManger.hpp"
#include "frameBuffersManger.hpp"

#include <array>
#include <vector>
#include <queue>
#include <thread>

enum class renderMode
{
    triangles,
    dots,
    lines
};

struct renderRequestInfo
{
    framebufferId frameBufferId;
    struct drawCallData
    {
        shaderId shader;
        vaoId vertexArrayId;
        textureId texturesIds[32] ;
        renderMode mode;
    };

    std::vector<drawCallData> drawCalls;    
};


class openGLRenderer
{
    private:
        std::queue<renderRequestInfo> requests;
        std::thread *renderThread;

        void renderHandle(openglContext *shared);
        
    public:        
        vertexArrayManger *shapes;
        shaderManger *shaders;
        frameBuffersManger *frameBuffers;
        textureManger *textures;

        static inline openglContext *context;
        openGLRenderer(openglContext *shared);
        ~openGLRenderer();
    
        void renderRequest(const renderRequestInfo& dataToRender);
};