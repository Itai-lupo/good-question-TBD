#include "VAOsComponents.hpp"
#include "vertexArrayManger.hpp"
#include "log.hpp"

#include <stdlib.h>

VAOsComponents::VAOsComponents(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

VAOsComponents::~VAOsComponents()
{
    pool->unenlistType(this, IdToIndex);
    
} 


void VAOsComponents::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;
    
    VAOInfo& temp = data[IdToIndex[id.index]];

    openGLRenderEngine::vaos::toDelete.push(temp.VAO);
    openGLRenderEngine::vaos::toDelete.push(temp.IBO);
    temp.iboToUpdate->deleteCallback(temp.iboToUpdate);

    for (size_t i = 0; i < 16; i++)
    {
        if(temp.bindingsSlots[i])
            openGLRenderEngine::vaos::toDelete.push(temp.bindingsSlots[i]);
        temp.bindingSlotsToUpdate[i]->deleteCallback(temp.bindingSlotsToUpdate[i]);
        temp.bindingSlotsToUpdate[i]->deleteCallback(temp.bindingSlotsToUpdate[i]);
    }
    
    
    uint32_t index = IdToIndex[id.index];
    IdToIndex[id.index] = -1;
    indexToId[index] = *indexToId.end();
    data[index] = *data.end();
    data.pop_back();

}

VAOInfo* VAOsComponents::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return &data[IdToIndex[id.index]];
}

void VAOsComponents::setComponent(entityId id, VAOInfo buffer)
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

void VBOUpdateRequstDefaultDeleteCallback(VBOUpdateRequst* toDelete)
{
    free(toDelete->data);
    delete toDelete;
}

void IBOUpdateRequstDefaultDeleteCallback(IBOUpdateRequst* toDelete)
{
    free(toDelete->data);
    delete toDelete;
}

void attacmentUpdateRequstDefaultDeleteCallback(attacmentUpdateRequst* toDelete)
{
    delete toDelete;
}