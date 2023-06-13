#include "vulkanRenderEngine/renderPiplineSystems/vertexArrayManger.hpp"

namespace vulkanRenderEngine
{

    void vaos::init(entityPool *pool) noexcept
    {
        vaos::vaosPool = pool;
        vaosData = new renderComponentTemplate(pool, sizeof(vao));
    }

    void vaos::close() noexcept
    {
        delete vaosData;
    }

    vaoId vaos::createVAO(vao info) noexcept
    {
        vaoId id = vaosPool->allocEntity();
        info.id = id;
        setVAOData(info);
        return id;
    }

    void vaos::setVAOData(vao info) noexcept
    {
        vaosData->setComponent(info.id, (void *)&info);
    }

    vao &vaos::getVAO(vaoId id) noexcept
    {
        return vaosData->getComponent<vao>(id);
    }

    vk::VertexInputBindingDescription *vaos::getBindingDescriptions(vaoId id) noexcept
    {
        if (!vaosData->isIdValid(id))
        {
            return nullptr;
        }
        const vao &temp = getVAO(id);
        vk::VertexInputBindingDescription *bindingDescription = new vk::VertexInputBindingDescription[temp.bindings->size()];

        for (int i = 0; i < temp.bindings->size(); i++)
        {
            bindingDescription[i].binding = i;
            bindingDescription[i].stride = (*temp.bindings)[i].stride;

            bindingDescription[i].inputRate = ((*temp.bindings)[i].isInstancedRendering ? vk::VertexInputRate::eInstance : vk::VertexInputRate::eVertex);
        }
        return bindingDescription;
    }

    vk::VertexInputAttributeDescription *vaos::getAttributeDescriptions(vaoId id) noexcept
    {

        if (!vaosData->isIdValid(id))
        {
            return nullptr;
        }
        const vao &temp = getVAO(id);

        vk::VertexInputAttributeDescription *attributeDescriptions = new vk::VertexInputAttributeDescription[temp.attributes->size()];

        for (int i = 0; i < temp.attributes->size(); i++)
        {
            attributeDescriptions[i].binding = (*temp.attributes)[i].binding;
            attributeDescriptions[i].location = (*temp.attributes)[i].location;
            attributeDescriptions[i].offset = (*temp.attributes)[i].offset;
            attributeDescriptions[i].format = (*temp.attributes)[i].format;
        }

        return attributeDescriptions;
    }

    size_t vaos::getBindingDescriptionsSize(vaoId id) noexcept
    {
        if (vaosData->isIdValid(id))
        {
            return getVAO(id).bindings->size();
        }
        else
        {
            return 0;
        }
    }

    size_t vaos::getAttributeDescriptionsSize(vaoId id) noexcept
    {
        if (vaosData->isIdValid(id))
        {
            return getVAO(id).attributes->size();
        }
        else
        {
            return 0;
        }
    }

}
