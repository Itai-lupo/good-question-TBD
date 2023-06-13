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

    uint32_t index = IdToIndex[id.index];

    
    data[index] = data.back();
    IdToIndex[data.back().id.index] = index;

    IdToIndex[id.index] = -1;

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

    }
    else
        data[IdToIndex[id.index]] = buffer;
    
}


frameBuffersComponents::~frameBuffersComponents()
{
    pool->unenlistType(this, IdToIndex);
    
}
