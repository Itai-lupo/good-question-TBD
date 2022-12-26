#include "textureComponents.hpp"
#include "textureManger.hpp"
#include "log.hpp"

textureComponents::textureComponents(entityPool *pool): 
    pool(pool)
{
    pool->enlistType((void*)this, deleteCallback, &IdToIndex);
}

void textureComponents::deleteComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return;

    openGLRenderEngine::textures::toDelete.push(data[IdToIndex[id.index]].renderId);
    
    if(data[IdToIndex[id.index]].bufferToLoad)
        data[IdToIndex[id.index]].bufferToLoad->deleteCallback(data[IdToIndex[id.index]].bufferToLoad);

    uint32_t index = IdToIndex[id.index];
    IdToIndex[id.index] = -1;
    indexToId[index] = *indexToId.end();
    data[index] = *data.end();
    data.pop_back();

}

textureInfo* textureComponents::getComponent(entityId id)
{
    if(!pool->isIdValid(id) || IdToIndex[id.index] == -1)
        return nullptr;
    
    return &data[IdToIndex[id.index]];
}

void textureComponents::setComponent(entityId id, textureInfo buffer)
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


textureComponents::~textureComponents()
{
    pool->unenlistType(this, IdToIndex);
    
}


void loadTextureRequstDefaultDeleteCallback(loadTextureRequst* toDelete)
{
    free(toDelete->pixels);
    delete toDelete;
}
