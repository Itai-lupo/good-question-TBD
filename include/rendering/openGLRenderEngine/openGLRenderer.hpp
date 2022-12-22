#pragma once

#include "renderApi.hpp"
#include "gpuRenderData.hpp"

#include "openglContext.hpp"
#include "textureManger.hpp"
#include "vertexArrayManger.hpp"
#include "shaderManger.hpp"
#include "frameBuffersManger.hpp"


#include <array>
#include <vector>
#include <queue>
#include <thread>

namespace openGLRenderEngine
{
    class openGLRenderer
    {
        private:
            static inline std::queue<renderRequestInfo> requests;
            static inline std::thread *renderThread;

            static void renderHandle(openglContext *shared);
            
        public:
            static inline openglContext *context;

            static void init(openglContext *shared);
            static void close();

            static void renderRequest(const renderRequestInfo& dataToRender);
    };
}