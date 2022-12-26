#pragma once 

#include "core.hpp"
#include "entityPool.hpp"
#include "log.hpp"

#include <array>

struct uniformBufferRequst;
struct uniformBufferInfo;

void uniformBufferRequstDefaultDeleteCallback(uniformBufferRequst* toDelete);

class uniformBufferComponents
{
    private:
        entityPool *pool;

        std::vector<uniformBufferInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            uniformBufferComponents *This = static_cast<uniformBufferComponents *>(data);
            This->deleteComponent(id);
        }

    public:
        uniformBufferComponents(entityPool *pool);
        ~uniformBufferComponents();

        void deleteComponent(entityId id);
        uniformBufferInfo *getComponent(entityId id);
        void setComponent(entityId id, uniformBufferInfo buffer);
        std::vector<uniformBufferInfo>& getData()
        {
            return data;
        }
};

struct uniformBufferRequst{
    uint8_t* data;
    int size;
    void(*deleteCallback)(uniformBufferRequst*) = uniformBufferRequstDefaultDeleteCallback;
};

struct uniformBufferInfo
{
    uniformBufferInfo(){}
    uniformBufferInfo(uniformBufferId id, uniformBufferRequst *bufferToLoad):
        id(id), bufferToLoad(bufferToLoad){}

    uint32_t renderId = 0;
    uniformBufferId id;

    uniformBufferRequst *bufferToLoad = nullptr;
    bool needToRebuild;
};