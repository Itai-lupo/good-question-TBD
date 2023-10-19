#include "vulkanRenderEngine/renderPiplineSystems/uniformBufferManger.hpp"
#include "device.hpp"

namespace vulkanRenderEngine
{
    void UBOs::init(entityPool *pool) noexcept
    {
        UBOsPool = pool;
        UBOsData = new renderComponentTemplate(pool, sizeof(ubo));
    }

    void UBOs::close() noexcept
    {
        delete UBOsData;
    }

    vaoId UBOs::createUniformBufferData(ubo info)
    {
        uniformBufferId id = UBOsPool->allocEntity();
        info.id = id;
        setUniformBufferData(info);
        return id;
    }

    void UBOs::setUniformBufferData(ubo info) noexcept
    {
        UBOsData->setComponent(info.id, (void *)&info);
    }

    ubo &UBOs::getUniformBuffer(uniformBufferId id) noexcept
    {
        return UBOsData->getComponent<ubo>(id);
    }

}