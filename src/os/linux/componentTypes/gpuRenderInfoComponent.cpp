#include "openGLRenderInfoComponent.hpp"
#include <Tracy.hpp>

openGLRenderInfoComponent::openGLRenderInfoComponent(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void openGLRenderInfoComponent::deleteComponent(entityId id)
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

openGlrenderInfo* openGLRenderInfoComponent::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return &data[IdToIndex[id.index]];
}

void openGLRenderInfoComponent::setComponent(entityId id,openGlrenderInfo buffer)
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
        TracyAlloc(&data.back(), sizeof(openGlrenderInfo));
    }
    else
    {
        data[IdToIndex[id.index]] = buffer;
    }
    
}


openGLRenderInfoComponent::~openGLRenderInfoComponent()
{
    pool->unenlistType(this, IdToIndex);
}
