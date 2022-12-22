#pragma once

#include "core.hpp"
#include "openglContext.hpp"
#include "entityPool.hpp"
#include "shadersComponents.hpp"

#include <array>
#include <list>
#include <vector>
#include <queue>
#include <string>


namespace openGLRenderEngine
{
    class shaders
    {
        private:
            static inline openglContext *context;
            static inline shadersComponents *shadersData;


            static void compileVertexShader(shaderInfo *shader);
            static void compileFrgmentShader(shaderInfo *shader);
            static void linkProgram(shaderInfo *shader);

        public:
            static inline std::queue<uint32_t> toDelete;

            static void init(entityPool *shadersPool);
            static void close();
            
            static void setContext(openglContext *context);
            static void handleRequsets();

            static void setShadersData(shaderInfo info);
            static shaderInfo *getShaders(shaderId id);
            
            static void bind(shaderId id);
    };
}