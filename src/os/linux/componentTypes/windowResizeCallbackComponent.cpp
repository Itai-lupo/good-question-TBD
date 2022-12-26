#include "windowResizeCallbackComponent.hpp"

windowResizeCallbackComponent::windowResizeCallbackComponent(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void windowResizeCallbackComponent::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    uint32_t index = IdToIndex[id.index];
    IdToIndex[id.index] = -1;
    indexToId[index] = *indexToId.end();
    data[index] = *data.end();
    data.pop_back();

}

resizeCallback windowResizeCallbackComponent::getCallback(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return data[IdToIndex[id.index]];
}

void windowResizeCallbackComponent::setCallback(entityId id, resizeCallback callback)
{
    if(!pool->isIdValid(id)){
        LOG_ERROR("culdn't set data, id(index: " << id.index << ", gen: " << (int)id.gen << ")")
        return;
    }

    if(IdToIndex[id.index] == -1)
    {
        IdToIndex[id.index] = data.size();
        data.push_back(callback);
        indexToId.push_back(id);

    }
    else
        data[IdToIndex[id.index]] = callback;
    
}


windowResizeCallbackComponent::~windowResizeCallbackComponent()
{
    pool->unenlistType(this, IdToIndex);
}
