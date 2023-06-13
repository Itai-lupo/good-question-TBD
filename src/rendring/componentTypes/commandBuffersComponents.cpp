#include "commandBuffersComponents.hpp"
#include "log.hpp"

commandBuffersComponents::commandBuffersComponents(entityPool *buffersPool, entityPool *poolsPool): 
    buffersPool(buffersPool), poolsPool(poolsPool)
{
    poolsPool->enlistType((void*)this, poolDeleteCallback, &poolIdToIndex);
    buffersPool->enlistType((void*)this, bufferDeleteCallback, &bufferIdToBuffer.idPoolBuffer);
}


void commandBuffersComponents::deletePool(entityId id)
{
    if(!poolsPool->isIdValid(id) || poolIdToIndex[id.index] == -1)
        return;

    
    uint32_t index = poolIdToIndex[id.index];

    delete data[index].buffers;
    poolIdToIndex[id.index] = -1;
    poolIdToIndex[data.end()->id.index] = index;
    
    data[index] = *data.end();
    data.pop_back();
}

commandPoolInfo *commandBuffersComponents::getPool(entityId id)
{
    if(!poolsPool->isIdValid(id) || poolIdToIndex[id.index] == -1)
        return nullptr;
    
    return &data[poolIdToIndex[id.index]];
}

void commandBuffersComponents::setPool(entityId id, commandPoolInfo& buffer)
{
    if(!poolsPool->isIdValid(id)){
        LOG_ERROR("culdn't set data, id(index: " << id.index << ", gen: " << (int)id.gen << ")")
        return;
    }

    if(poolIdToIndex[id.index] == -1)
    {
        poolIdToIndex[id.index] = data.size();
        buffer.id = id;
        buffer.buffers = new std::vector<commandBufferInfo>();
        data.push_back(buffer);
    }
    else
        data[poolIdToIndex[id.index]] = buffer;
}


void commandBuffersComponents::deleteBuffer(entityId id)
{
    if( !buffersPool->isIdValid(id) || bufferIdToBuffer.idPoolBuffer[id.index] == -1)
        return;

    uint16_t poolId = bufferIdToBuffer.poolAndIndex[id.index].poolId;
    if(poolId >= poolsPool->getIdBufferMaxCount() || poolIdToIndex[poolId] == -1)
        return;

    std::vector<commandBufferInfo> *buffers = data[poolIdToIndex[poolId]].buffers;
    uint16_t bufferIndex = bufferIdToBuffer.poolAndIndex[id.index].bufferIndex;

    bufferIdToBuffer.idPoolBuffer[id.index] = -1;
    bufferIdToBuffer.poolAndIndex[buffers->end()->id.index].bufferIndex = bufferIndex;
    
    buffers->at(bufferIndex) = *buffers->end();
    buffers->pop_back();
}

commandBufferInfo *commandBuffersComponents::getBuffer(entityId id)
{
    if( !buffersPool->isIdValid(id) || bufferIdToBuffer.idPoolBuffer[id.index] == -1)
        return nullptr;

    uint16_t poolId = bufferIdToBuffer.poolAndIndex[id.index].poolId;
    if(poolId >= poolsPool->getIdBufferMaxCount() || poolIdToIndex[poolId] == -1)
        return nullptr;

    std::vector<commandBufferInfo> *buffers = data[poolIdToIndex[poolId]].buffers;
    uint16_t index = bufferIdToBuffer.poolAndIndex[id.index].bufferIndex;

    return &(*buffers)[index];
}

void commandBuffersComponents::setBuffer(entityId id, commandBufferInfo& buffer)
{
    if( !buffersPool->isIdValid(id))
        return ;

    uint16_t poolId = buffer.poolId.index;
    if(poolId >= poolsPool->getIdBufferMaxCount() || poolIdToIndex[poolId] == -1)
        return ;

        

    if(poolId != bufferIdToBuffer.poolAndIndex[id.index].poolId)
        deleteBuffer(id);

    std::vector<commandBufferInfo> *buffers = data[poolIdToIndex[poolId]].buffers;
    if(bufferIdToBuffer.poolAndIndex[id.index].bufferIndex == (uint16_t)-1)
    {
        bufferIdToBuffer.poolAndIndex[id.index].bufferIndex = buffers->size();
        bufferIdToBuffer.poolAndIndex[id.index].poolId = poolId;
        buffer.id = id;
        buffers->push_back(buffer);
    }
    else
    {
        buffers->at(bufferIdToBuffer.poolAndIndex[id.index].bufferIndex) = buffer;
    }
}


commandBuffersComponents::~commandBuffersComponents()
{
    poolsPool->unenlistType(this, poolIdToIndex);
    buffersPool->unenlistType(this, bufferIdToBuffer.idPoolBuffer);    
}
