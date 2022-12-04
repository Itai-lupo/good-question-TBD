#pragma once
#include "core.hpp"

#include <list>
#include <vector>
#include <stdlib.h>


using typeCallback = void (*)(void *, entityId);

class entityPool
{
    private:
        uint32_t maxSize;
        uint32_t maxId;
        uint32_t maxAllocatedId = 0;

        struct listedType
        {
            void *data;
            typeCallback deleteFunc; 
            uint32_t **IdToIndex;
        };

        std::vector<listedType> listedTypes;

        static inline std::list<uint32_t> freeSlots;
        uint8_t *gen;

        
        


    public:
        entityPool(uint32_t maxSize);
        ~entityPool();

        entityId allocEntity();
        void freeEntity(entityId id);
        void enlistType(void *dataPtr, typeCallback freeCallback, uint32_t **IdToIndex);
        bool isIdValid(entityId id)
        {
            return !(id.index > maxAllocatedId || gen[id.index] != id.gen);
        }

        uint32_t getIdBufferMaxCount()
        {
            return maxId;
        }
        
};

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
            .gen = gen[freeSlots.front()],
            .index = freeSlots.front()
        };
        freeSlots.pop_front();
    }
    else
    {
        id = {
            .gen = 0,
            .index = maxAllocatedId
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





entityPool::~entityPool()
{
}
