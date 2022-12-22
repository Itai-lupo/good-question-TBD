#pragma once 

#include "core.hpp"
#include "entityPool.hpp"
#include "log.hpp"

#include <array>


struct shaderInfo
{
    shaderId id;
    char *vertexPath, *fragmentPath;

    uint32_t programId = 0;
    uint32_t vertexShader, fragmentShader;

    bool needToRebuild;
};

class shadersComponents
{
    private:
        entityPool *pool;

        std::vector<shaderInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            shadersComponents *This = static_cast<shadersComponents *>(data);
            This->deleteComponent(id);
        }

    public:
        shadersComponents(entityPool *pool);
        ~shadersComponents();

        void deleteComponent(entityId id);
        shaderInfo *getComponent(entityId id);
        void setComponent(entityId id, shaderInfo buffer);
        std::vector<shaderInfo>& getData()
        {
            return data;
        }
};
