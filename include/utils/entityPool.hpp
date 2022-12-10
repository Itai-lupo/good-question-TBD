#pragma once
#include "core.hpp"

#include <list>
#include <vector>
#include <stdlib.h>
#include <string.h>

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
        void unenlistType(void *dataPtr, uint32_t *IdToIndex);
        bool isIdValid(entityId id)
        {
            return !(id.index > maxAllocatedId || gen[id.index] != id.gen);
        }

        uint32_t getIdBufferMaxCount()
        {
            return maxId;
        }
        
};
