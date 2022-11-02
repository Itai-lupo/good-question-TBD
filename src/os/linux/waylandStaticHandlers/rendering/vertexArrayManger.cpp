#include "vertexArrayManger.hpp"
#include <Tracy.hpp>

vaoId vertexArrayManger::createVao()
{
    vaoId id;
    if(!freeSlots.empty())
    {
        id = {
            .gen = idToIndex[freeSlots.front()].gen,
            .index = freeSlots.front()
        };
        freeSlots.pop_front();
    }
    else
    {
        id = {
            .gen = 0,
            .index = (uint32_t)idToIndex.size()
        };
        idToIndex.push_back({0, (uint32_t)buffers.size()});
    }

    idToIndex[id.index].index = buffers.size();
    
    VAOInfo info;
    info.id = id;
    buffers.push_back(info);

    VAORequstes handle;  
    handle.id = id;
    requstes.push_back(handle);
    
    
    return id;

}

void vertexArrayManger::addVertexBufferBinding(vaoId id, const VBOSpec& data)
{
    if(idToIndex[id.index].gen != id.gen)
        return;

    VAOInfo& temp = buffers[idToIndex[id.index].index];
    
    temp.bindingsSlots[data.bindingSlot].data = data.data;
    temp.bindingsSlots[data.bindingSlot].offset = data.offset;
    temp.bindingsSlots[data.bindingSlot].size = data.size;
    temp.bindingsSlots[data.bindingSlot].stride = data.stride;

    requstes[idToIndex[id.index].index].rebuildBinding.push(data.bindingSlot);
}

void vertexArrayManger::addVertexBufferAttacment(vaoId id, uint32_t slot, uint32_t binding, uint32_t size, uint32_t offset, uint32_t dataType)
{
    if(idToIndex[id.index].gen != id.gen)
        return;

    VAOInfo& temp = buffers[idToIndex[id.index].index];
    
    temp.attributes[slot].bindingSlot = binding;
    temp.attributes[slot].size = size;
    temp.attributes[slot].offset = offset;
    temp.attributes[slot].dataType = dataType;

    requstes[idToIndex[id.index].index].rebuildAttacment.push(slot);
}

void vertexArrayManger::attachIndexBuffer(vaoId id, const uint32_t *IBO, uint32_t size)
{
    if(idToIndex[id.index].gen != id.gen)
        return;

    requstes[idToIndex[id.index].index].IBOToBuild = IBO;
    requstes[idToIndex[id.index].index].IBOsize = size;

}

void vertexArrayManger::bind(vaoId id)
{
    ZoneScopedN("bind shape");

    if(idToIndex[id.index].gen != id.gen)
        return;

    VAOInfo& temp = buffers[idToIndex[id.index].index];
    VAORequstes& toHandle = requstes[idToIndex[id.index].index];

    if(temp.VAO == 0)
    {
        GL_CALL(context, CreateVertexArrays(1, &temp.VAO));
    }

    while (!toHandle.rebuildBinding.empty())
    {
        uint8_t binding = toHandle.rebuildBinding.front();

        
        GL_CALL(context, CreateBuffers(1, &temp.bindingsSlots[binding].renderId));
        GL_CALL(context, NamedBufferData(temp.bindingsSlots[binding].renderId, temp.bindingsSlots[binding].size, temp.bindingsSlots[binding].data, GL_DYNAMIC_DRAW));
        GL_CALL(context, VertexArrayVertexBuffer(temp.VAO, binding, temp.bindingsSlots[binding].renderId, temp.bindingsSlots[binding].offset, temp.bindingsSlots[binding].stride));

        toHandle.rebuildBinding.pop();
    }

    while (!toHandle.rebuildAttacment.empty())
    {
        uint8_t attrib = toHandle.rebuildAttacment.front();

        
        GL_CALL(context, EnableVertexArrayAttrib(temp.VAO, attrib));
        GL_CALL(context, VertexArrayAttribBinding(temp.VAO, attrib, temp.attributes[attrib].bindingSlot));
        GL_CALL(context, VertexArrayAttribFormat(temp.VAO, attrib, temp.attributes[attrib].size, temp.attributes[attrib].dataType, GL_FALSE, temp.attributes[attrib].offset));

        toHandle.rebuildAttacment.pop();
    }

    if(toHandle.IBOToBuild)
    {
        GL_CALL(context, CreateBuffers(1, &temp.IBO));

        GL_CALL(context, NamedBufferData(temp.IBO, toHandle.IBOsize * sizeof(unsigned int), toHandle.IBOToBuild, GL_DYNAMIC_DRAW));
        GL_CALL(context, VertexArrayElementBuffer(temp.VAO, temp.IBO));
        temp.count = toHandle.IBOsize;
        
        toHandle.IBOToBuild = nullptr;
        toHandle.IBOsize = 0;
    }

    GL_CALL(context, BindVertexArray(temp.VAO));
    
}

int vertexArrayManger::getCount(vaoId id)
{
    if(idToIndex[id.index].gen != id.gen)
        return 0;

    VAOInfo& temp = buffers[idToIndex[id.index].index];

    return temp.count;
}

