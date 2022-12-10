#include "entityPool.hpp"

entityPool::entityPool(uint32_t maxSize): maxSize(maxSize)
{
    maxId = 10;
    gen = new uint8_t[maxId];
    memset(gen, 255, (maxId));    
}


entityId entityPool::allocEntity()
{
    entityId id;
    if(!freeSlots.empty())
    {
        id = {
            .index = freeSlots.front(),
            .gen = gen[freeSlots.front()]
        };
        freeSlots.pop_front();
    }
    else
    {
        id = {
            .index = maxAllocatedId,
            .gen = 0
        };

        if(maxAllocatedId >= maxId)
        {
            gen = (uint8_t*)realloc(gen, maxId * 2 *sizeof(uint8_t));
            memset(gen + maxId, 255, maxId);

            for (auto& type: listedTypes)
            {
                *type.IdToIndex = (uint32_t*)realloc(*type.IdToIndex, maxId * 2 * sizeof(uint32_t));
                memset(*type.IdToIndex + maxId, 255, maxId * sizeof(uint32_t));
            }

            maxId *= 2;
        }

        gen[maxAllocatedId] = 0; 
        maxAllocatedId++;
    }

    return id;
}


void entityPool::freeEntity(entityId id)
{
    for (auto& type: listedTypes)
        type.deleteFunc(type.data, id);

    gen[id.index]++;
    freeSlots.push_back(id.index);
    
}

void entityPool::enlistType(void *dataPtr, typeCallback freeCallback, uint32_t **IdToIndex)
{
    *IdToIndex =  new uint32_t[maxId];
    memset(*IdToIndex, 255, sizeof(uint32_t) * maxId);      

    listedTypes.push_back({dataPtr, freeCallback, IdToIndex});
}

void entityPool::unenlistType(void *dataPtr, uint32_t *IdToIndex)
{
    for (int i = 0; i < listedTypes.size(); i++)
    {
        if(listedTypes[i].data == dataPtr && *listedTypes[i].IdToIndex == IdToIndex){
            listedTypes[i] = *listedTypes.end();
            listedTypes.pop_back();
        }

    }
    
}




entityPool::~entityPool()
{
    delete[] gen;
}
