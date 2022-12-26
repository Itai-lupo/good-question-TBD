#include "uniformBufferManger.hpp"
#include <Tracy.hpp>





namespace openGLRenderEngine
{
    void uniformBuffers::init(entityPool *uniformBuffersPool)
    {
        uniformBuffersData = new uniformBufferComponents(uniformBuffersPool);
    }

    void uniformBuffers::close()
    {
        delete uniformBuffersData;
    }
    
    void uniformBuffers::setContext(openglContext *context)
    {
        uniformBuffers::context = context;
    }

    void uniformBuffers::handleRequsets()
    {
        if(toDelete.empty())
            return;

        uint32_t *uniformBuffersToDelete = new uint32_t[toDelete.size()];
        uint32_t temp = toDelete.size();

        for (size_t i = 0; i < temp; i++)
        {
            uniformBuffersToDelete[i] = toDelete.front();
            toDelete.pop();
        }
        
        GL_CALL(context, DeleteBuffers(temp, uniformBuffersToDelete)); 
    }


    void uniformBuffers::setUniformBufferData(uniformBufferInfo info)
    {
        uniformBuffersData->setComponent(info.id, info);
    }

    uniformBufferInfo *uniformBuffers::getUniformBuffer(uniformBufferId id)
    {
        return uniformBuffersData->getComponent(id);
    }

    
    void uniformBuffers::bind(uniformBufferId id, int bindingIndex)
    {
        uniformBufferInfo *UBO = uniformBuffersData->getComponent(id);
        if(!UBO)
            return;

        int size = 16;
        if(UBO->renderId == 0)
        {
            GL_CALL(context, CreateBuffers(1, &UBO->renderId));

    
            GL_CALL(context, NamedBufferStorage(UBO->renderId, UBO->bufferToLoad->size, UBO->bufferToLoad->data, GL_DYNAMIC_STORAGE_BIT));
        }

        GL_CALL(context, BindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, UBO->renderId, 0, UBO->bufferToLoad->size));

    }
}
