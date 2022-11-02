#pragma once
#include "openglContext.hpp"
#include "core.hpp"

#include <array>
#include <list>
#include <vector>
#include <queue>

enum class textureFormat
{
    None = 0,

    // Color
    RGBA8,
    RED_INTEGER,
    RGB8,
    RG8,
    R8,

    // Depth/stencil
    DEPTH24STENCIL8,

    // Defaults
    Depth = DEPTH24STENCIL8
};

class textureManger
{
    private:
        openglContext *context;
        std::queue<uint32_t> toDelete;

        struct loadTextureRequst{
            textureId id;
            uint32_t x, y;
            uint32_t width, height;
            textureFormat format;
            uint32_t type;
            void* pixels;
        };

        std::queue<loadTextureRequst> toLoad;

        struct textureInfo
        {
            uint32_t renderId = 0;
            textureId id;

            int channels = 4;
            int samples = 1;
            uint32_t width, height;
            
            textureFormat format = textureFormat::RGBA8;
            uint32_t type;
            void* pixels = nullptr;
            loadTextureRequst temp;
            bool needToRebuild;
        };


        std::vector<textureInfo> textures;

        struct idIndexes
        {
            uint32_t gen: 8 = -1;
            uint32_t index: 24;
        };        

        std::vector<idIndexes> idToIndex;  
        std::list<uint32_t> freeSlots;
        
        void rebuild(textureInfo& fbo);
        uint32_t textureFormatToOpenGlFormat(textureFormat formatToConvert);
        uint32_t textureFormatToOpenGlDataFormat(textureFormat formatToConvert);
        uint32_t textureFormatToChannelsCount(textureFormat formatToConvert);

    public:
        textureManger(openglContext *context): context(context){}
        
        void handleRequsets();
        void rebuild(textureId idx);
        void bind(textureId id, uint32_t slot);
        uint32_t getRenderId(textureId id);
        void setRenderId(textureId id, uint32_t renderId);


        textureId createTexture(textureFormat format, uint32_t width, uint32_t height);

        void resize(textureId id, uint32_t width, uint32_t height);
        void loadBuffer(textureId id, uint32_t x, uint32_t y, uint32_t width, uint32_t height, textureFormat format, uint32_t type, void* pixels);

        void deleteTexture(textureId id);
        
        uint32_t getWidth(textureId id);
        uint32_t getheight(textureId id);

        textureFormat getTextureFormat(textureId id);
};
