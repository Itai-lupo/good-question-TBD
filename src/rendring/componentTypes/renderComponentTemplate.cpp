#include "renderComponentTemplate.hpp"
#include "log.hpp"

#include <Tracy.hpp>

renderComponentTemplate::renderComponentTemplate(entityPool *pool, size_t dataSize) noexcept : pool(pool), dataSize(dataSize),
                                                                                               dataMaxAllocated(10),
                                                                                               data((char *)malloc(dataSize * dataMaxAllocated)),
                                                                                               indexToId(new entityId[dataMaxAllocated])
{
    TracyAlloc(data, dataMaxAllocated * dataSize);
    TracyAlloc(indexToId, sizeof(entityId) * dataMaxAllocated);
    pool->enlistType((void *)this, deleteCallback, &IdToIndex);
}

renderComponentTemplate::~renderComponentTemplate() noexcept
{
    pool->unenlistType(this, IdToIndex);

    TracyFree(data);
    free(data);
    TracyFree(indexToId);
    free(indexToId);
}

void renderComponentTemplate::deleteComponent(entityId id) noexcept
{
    if (!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    uint32_t index = IdToIndex[id.index];
    IdToIndex[id.index] = (uint32_t)-1;
    if (index == dataAllocated - 1)
    {
        dataAllocated--;
        return;
    }

    indexToId[index] = indexToId[dataAllocated - 1];
    IdToIndex[indexToId[dataAllocated - 1].index] = index;

    memcpy(data + index * dataSize, data + (dataAllocated - 1) * dataSize, dataSize);
    dataAllocated--;
}

void *renderComponentTemplate::getComponentBuffer(entityId id) noexcept
{
    if (!isIdValid(id))
        return nullptr;

    return (void *)(data + IdToIndex[id.index] * dataSize);
}

void renderComponentTemplate::setComponent(entityId id, void *buffer) noexcept
{
    if (!pool->isIdValid(id))
    {
        LOG_ERROR("culdn't set data, id(index: " << id.index << ", gen: " << (int)id.gen << ")")
        return;
    }

    if (IdToIndex[id.index] == -1)
    {
        IdToIndex[id.index] = dataAllocated;
        if (dataAllocated == dataMaxAllocated)
        {
            TracyFree(data);
            TracyFree(indexToId);

            dataMaxAllocated *= 2;
            data = (char *)realloc(data, dataMaxAllocated * dataSize);
            indexToId = (entityId *)realloc(indexToId, dataMaxAllocated * sizeof(entityId));

            TracyAlloc(data, dataMaxAllocated * dataSize);
            TracyAlloc(indexToId, sizeof(entityId) * dataMaxAllocated);
        }
        memcpy((char *)data + dataAllocated * dataSize, buffer, dataSize);
        indexToId[dataAllocated] = id;
        dataAllocated++;
    }
    else
        memcpy((char *)data + IdToIndex[id.index] * dataSize, buffer, dataSize);
}

bool renderComponentTemplate::isIdValid(entityId id) noexcept
{
    return (pool->isIdValid(id) && (int32_t)IdToIndex[id.index] != -1);
}

void *renderComponentTemplate::getData()
{
    return data;
}

void *renderComponentTemplate::getLastElement()
{
    return data + (dataAllocated * dataSize);
}
