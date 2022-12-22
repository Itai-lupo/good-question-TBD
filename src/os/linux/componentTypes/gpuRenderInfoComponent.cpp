#include "gpuRenderInfoComponent.hpp"
#include <Tracy.hpp>

gpuRenderInfoComponent::gpuRenderInfoComponent(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void gpuRenderInfoComponent::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    uint32_t index = IdToIndex[id.index];
    IdToIndex[id.index] = -1;
    indexToId[index] = *indexToId.end();
    data[index] = *data.end();

    TracyFree(&data.back());
    data.pop_back();

}

renderInfo* gpuRenderInfoComponent::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return &data[IdToIndex[id.index]];
}

void gpuRenderInfoComponent::setComponent(entityId id, renderInfo buffer)
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
        TracyAlloc(&data.back(), sizeof(renderInfo));
    }
    else
    {
        data[IdToIndex[id.index]] = buffer;
    }
    
}


gpuRenderInfoComponent::~gpuRenderInfoComponent()
{
    pool->unenlistType(this, IdToIndex);
}
