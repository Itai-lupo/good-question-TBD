#include "vulkanRenderInfoComponent.hpp"
#include <Tracy.hpp>

vulkanRenderInfoComponent::vulkanRenderInfoComponent(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void vulkanRenderInfoComponent::deleteComponent(entityId id)
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

vulkanRenderInfo* vulkanRenderInfoComponent::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return &data[IdToIndex[id.index]];
}

void vulkanRenderInfoComponent::setComponent(entityId id, vulkanRenderInfo buffer)
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
        TracyAlloc(&data.back(), sizeof(vulkanRenderInfo));
    }
    else
    {
        data[IdToIndex[id.index]] = buffer;
    }
    
}


vulkanRenderInfoComponent::~vulkanRenderInfoComponent()
{
    pool->unenlistType(this, IdToIndex);
}
