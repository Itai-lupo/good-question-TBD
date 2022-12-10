#pragma once
#include "entityPool.hpp"

class componentType
{
    private:
        entityPool *pool;
        uint16_t dataSize;

        uint32_t dataMaxAllocated;
        uint32_t dataAllocated = 0;
        void * data;

        uint32_t *IdToIndex;  
        entityId *indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            componentType *This = static_cast<componentType *>(data);
            This->deleteComponent(id);
        }

    public:
        componentType(entityPool *pool, uint16_t dataSize);
        ~componentType();

        void deleteComponent(entityId id);
        void *getComponent(entityId id);
        void setComponent(entityId id, void *buffer);
};

componentType::componentType(entityPool *pool, uint16_t dataSize): 
    pool(pool), 
    dataSize(dataSize), 
    dataMaxAllocated(10), 
    data( malloc(dataSize * 10)),
    indexToId(new entityId[dataMaxAllocated])
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);

}

void componentType::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    uint32_t index = IdToIndex[id.index];
    IdToIndex[id.index] = -1;
    indexToId[index] = indexToId[dataAllocated - 1];
    memcpy((char*)data + index * dataSize, (char*)data + (dataAllocated - 1) * dataSize, dataSize);
    dataAllocated--;

}

void *componentType::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return (void*)((char *)data + IdToIndex[id.index] * dataSize);
}

void componentType::setComponent(entityId id, void *buffer)
{
    if(!pool->isIdValid(id)){
        LOG_ERROR("culdn't set data, id(index: " << id.index << ", gen: " << (int)id.gen << ")")
        return;
    }

    if(IdToIndex[id.index] == -1)
    {
        IdToIndex[id.index] = dataAllocated;
        if( dataAllocated == dataMaxAllocated)
        {
            dataMaxAllocated *= 2;
            data = realloc(data, dataMaxAllocated * dataSize);
            indexToId = (entityId*)realloc(indexToId, dataMaxAllocated * sizeof(entityId));
        } 
        memcpy((char *)data + dataAllocated * dataSize, buffer, dataSize);
        indexToId[dataAllocated] = id;
        dataAllocated++;

    }
    else
        memcpy((char *)data + IdToIndex[id.index] * dataSize, buffer, dataSize);
    
}


componentType::~componentType()
{
    pool->unenlistType(this, IdToIndex);
    free(data);
    free(IdToIndex);
    free(indexToId);
}
