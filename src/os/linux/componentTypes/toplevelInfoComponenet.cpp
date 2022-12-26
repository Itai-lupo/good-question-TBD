#include "toplevelInfoComponenet.hpp"

toplevelInfoComponenet::toplevelInfoComponenet(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void toplevelInfoComponenet::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    uint32_t index = IdToIndex[id.index];

    zxdg_toplevel_decoration_v1_destroy(data[index].topLevelDecoration);
    xdg_toplevel_destroy(data[index].xdgToplevel);
    xdg_surface_destroy(data[index].xdgSurface);

    IdToIndex[id.index] = -1;
    indexToId[index] = *indexToId.end();
    data[index] = *data.end();
    data.pop_back();

}

toplevelSurfaceInfo *toplevelInfoComponenet::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return &data[IdToIndex[id.index]];
}

void toplevelInfoComponenet::setComponent(entityId id, toplevelSurfaceInfo callback)
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


toplevelInfoComponenet::~toplevelInfoComponenet()
{
    pool->unenlistType(this, IdToIndex);
    
}
