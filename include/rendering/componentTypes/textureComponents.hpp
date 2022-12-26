#pragma once 

#include "core.hpp"
#include "entityPool.hpp"
#include "log.hpp"

#include <array>

struct loadTextureRequst;
struct textureInfo;

void loadTextureRequstDefaultDeleteCallback(loadTextureRequst* toDelete);

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




class textureComponents
{
    private:
        entityPool *pool;

        std::vector<textureInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            textureComponents *This = static_cast<textureComponents *>(data);
            This->deleteComponent(id);
        }

    public:
        textureComponents(entityPool *pool);
        ~textureComponents();

        void deleteComponent(entityId id);
        textureInfo *getComponent(entityId id);
        void setComponent(entityId id, textureInfo buffer);
        std::vector<textureInfo>& getData()
        {
            return data;
        }
};

struct loadTextureRequst{
    // textureId id;
    uint32_t x, y;
    uint32_t width, height;
    textureFormat format;
    uint32_t type;
    void* pixels;
    void(*deleteCallback)(loadTextureRequst*) = loadTextureRequstDefaultDeleteCallback;

};

struct textureInfo
{
    textureInfo(){}
    textureInfo(textureId id, uint32_t width, uint32_t height, textureFormat format):
        id(id), width(width), height(height), format(format){}

    uint32_t renderId = 0;
    textureId id;

    uint32_t width, height;
    textureFormat format = textureFormat::RGBA8;
    int channels = 4;
    int samples = 1;
    
    
    loadTextureRequst *bufferToLoad = nullptr;
    bool needToRebuild;
};