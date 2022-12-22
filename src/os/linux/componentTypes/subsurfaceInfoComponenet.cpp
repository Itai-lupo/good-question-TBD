#include "subsurfaceInfoComponenet.hpp"

subsurfaceInfoComponenet::subsurfaceInfoComponenet(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void subsurfaceInfoComponenet::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    uint32_t index = IdToIndex[id.index];

    wl_subsurface_destroy(data[index].subsurface);

    IdToIndex[id.index] = -1;
    indexToId[index] = *indexToId.end();
    data[index] = *data.end();
    data.pop_back();

}

subsurfaceInfo *subsurfaceInfoComponenet::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return &data[IdToIndex[id.index]];
}

void subsurfaceInfoComponenet::setComponent(entityId id, subsurfaceInfo callback)
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


subsurfaceInfoComponenet::~subsurfaceInfoComponenet()
{
    pool->unenlistType(this, IdToIndex);
    free(IdToIndex);
}
