#include "cpuRenderInfoComponent.hpp"
#include <Tracy.hpp>

cpuRenderInfoComponent::cpuRenderInfoComponent(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void cpuRenderInfoComponent::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    uint32_t index = IdToIndex[id.index];
    IdToIndex[id.index] = -1;
    indexToId[index] = *indexToId.end();
    data[index] = *data.end();

    TracyFree(&data.back());
    data.pop_back();

}

cpuRenderInfo* cpuRenderInfoComponent::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return &data[IdToIndex[id.index]];
}

void cpuRenderInfoComponent::setComponent(entityId id, cpuRenderInfo& buffer)
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
        TracyAlloc(&data.back(), sizeof(cpuRenderInfo));
    }
    else
    {
        data[IdToIndex[id.index]] = buffer;
    }
    
}


cpuRenderInfoComponent::~cpuRenderInfoComponent()
{
    for(int i = 0; i < pool->getIdBufferMaxCount(); i++)
        IdToIndex[i] = -1;
    for(auto& cpuRender: data)
        if(cpuRender.renderThread->joinable())
            cpuRender.renderThread->join();
    pool->unenlistType(this, IdToIndex);
}
