#include "apiTypeComponents.hpp"
#include "log.hpp"

apiTypeComponents::apiTypeComponents(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void apiTypeComponents::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    uint32_t index = IdToIndex[id.index];
    IdToIndex[id.index] = -1;
    indexToId[index] = *indexToId.end();
    data[index] = *data.end();
    data.pop_back();

}

supportedRenderApis apiTypeComponents::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return supportedRenderApis::apiCount;
    
    return data[IdToIndex[id.index]];
}

void apiTypeComponents::setComponent(entityId id, supportedRenderApis buffer)
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


apiTypeComponents::~apiTypeComponents()
{
    pool->unenlistType(this, IdToIndex);
    free(IdToIndex);
}
