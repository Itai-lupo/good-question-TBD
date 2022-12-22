#pragma once
#include "core.hpp"
#include "openglContext.hpp"
#include "gpuRenderData.hpp"
#include "entityPool.hpp"
#include "textureComponents.hpp"

#include <array>
#include <list>
#include <vector>
#include <queue>

namespace openGLRenderEngine
{
    class textures
    {
        private:
            static inline openglContext *context;
            static inline textureComponents *texturesData;

            static void rebuild(textureInfo *info);
            static uint32_t textureFormatToOpenGlFormat(textureFormat formatToConvert);
            static uint32_t textureFormatToOpenGlDataFormat(textureFormat formatToConvert);
            static uint32_t textureFormatToChannelsCount(textureFormat formatToConvert);
        public:
            static inline std::queue<uint32_t> toDelete;
            
            static void init(entityPool *texturesPool);
            static void close();
            
            static void setContext(openglContext *context);
            static void handleRequsets();

            static void setTextureData(textureInfo info);
            static textureInfo *getTexture(textureId id);
            
            static void bind(textureId id, uint32_t slot);
            static void rebuild(textureId textureId);
    };
}