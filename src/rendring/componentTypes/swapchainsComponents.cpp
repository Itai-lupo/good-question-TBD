#include "swapchainsComponents.hpp"
#include "log.hpp"

swapchainsComponents::swapchainsComponents(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void swapchainsComponents::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    uint32_t index = IdToIndex[id.index];
    IdToIndex[id.index] = -1;
    indexToId[index] = indexToId.back();
    data[index] = data.back();

    data.pop_back();

}

swapChainInfo *swapchainsComponents::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return &data[IdToIndex[id.index]];
}

void swapchainsComponents::setComponent(entityId id, swapChainInfo& buffer)
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


swapchainsComponents::~swapchainsComponents()
{
    pool->unenlistType(this, IdToIndex);
    
}
