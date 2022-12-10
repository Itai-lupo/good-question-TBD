#pragma once 
#include "windowCloseCallbackComponent.hpp"

windowCloseCallbackComponent::windowCloseCallbackComponent(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void windowCloseCallbackComponent::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    uint32_t index = IdToIndex[id.index];
    IdToIndex[id.index] = -1;
    indexToId[index] = *indexToId.end();
    data[index] = *data.end();
    data.pop_back();

}

closeCallback windowCloseCallbackComponent::getCallback(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return data[IdToIndex[id.index]];
}

void windowCloseCallbackComponent::setCallback(entityId id, closeCallback callback)
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


windowCloseCallbackComponent::~windowCloseCallbackComponent()
{
    pool->unenlistType(this, IdToIndex);
    free(IdToIndex);
}
