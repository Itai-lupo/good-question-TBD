#include "vkSurfaceComponents.hpp"
#include "log.hpp"

vkSurfaceComponents::vkSurfaceComponents(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void vkSurfaceComponents::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    uint32_t index = IdToIndex[id.index];
    IdToIndex[id.index] = -1;
    indexToId[index] = *indexToId.end();
    data[index] = *data.end();
    data.pop_back();

}

vk::SurfaceKHR vkSurfaceComponents::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        LOG_FATAL("no vk surface with that id " << id.index << ", " << id.gen);
    
    return data[IdToIndex[id.index]];
}

void vkSurfaceComponents::setComponent(entityId id, vk::SurfaceKHR buffer)
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


vkSurfaceComponents::~vkSurfaceComponents()
{
    pool->unenlistType(this, IdToIndex);
    
}
