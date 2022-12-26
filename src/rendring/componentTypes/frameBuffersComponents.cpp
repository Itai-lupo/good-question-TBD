#include "frameBuffersComponents.hpp"
#include "frameBuffersManger.hpp"
#include "log.hpp"

frameBuffersComponents::frameBuffersComponents(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void frameBuffersComponents::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    openGLRenderEngine::framebuffers::toDelete.push(data[IdToIndex[id.index]].renderId);

    uint32_t index = IdToIndex[id.index];
    IdToIndex[id.index] = -1;
    indexToId[index] = *indexToId.end();
    data[index] = *data.end();
    data.pop_back();

}

frameBufferInfo* frameBuffersComponents::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return &data[IdToIndex[id.index]];
}

void frameBuffersComponents::setComponent(entityId id, frameBufferInfo buffer)
{
    if(!pool->isIdValid(id)){
        LOG_ERROR("culdn't set data, id(index: " << id.index << ", gen: " << (int)id.gen << ")")
        return;
    }

    if(IdToIndex[id.index] == -1)
    {
        IdToIndex[id.index] = data.size();
        data.push_back(buffer);
        indexToId.push_back(id);

    }
    else
        data[IdToIndex[id.index]] = buffer;
    
}


frameBuffersComponents::~frameBuffersComponents()
{
    pool->unenlistType(this, IdToIndex);
    
}
