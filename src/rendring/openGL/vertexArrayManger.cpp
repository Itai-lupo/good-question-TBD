#include "vertexArrayManger.hpp"
#include <Tracy.hpp>





namespace openGLRenderEngine
{
    void vaos::init(entityPool *vaosPool)
    {
        vaosData = new VAOsComponents(vaosPool);
    }

    void vaos::close()
    {
        delete vaosData;
    }
    
    void vaos::setContext(openglContext *context)
    {
        vaos::context = context;
    }

    void vaos::handleRequsets()
    {
        if(toDelete.empty())
            return;

        uint32_t *vaosToDelete = new uint32_t[toDelete.size()];
        uint32_t temp = toDelete.size();

        for (size_t i = 0; i < temp; i++)
        {
            vaosToDelete[i] = toDelete.front();
            toDelete.pop();
        }
        
        GL_CALL(context, DeleteBuffers(temp, vaosToDelete)); 
    }


    void vaos::setVAOData(VAOInfo info)
    {
        vaosData->setComponent(info.id, info);
    }

    VAOInfo *vaos::getVAO(vaoId id)
    {
        return vaosData->getComponent(id);
    }

    
    void vaos::bind(vaoId id)
    {
        ZoneScopedN("bind shape");
        VAOInfo *vao = vaosData->getComponent(id);
        if(!vao)
            return;

        if(vao->VAO == 0)
            GL_CALL(context, CreateVertexArrays(1, &vao->VAO));


        for (size_t i = 0; i < 16; i++)
        {
            if(vao->bindingSlotsToUpdate[i])
            {
                if(vao->bindingsSlots[i])
                    GL_CALL(context, DeleteBuffers(1, &vao->bindingsSlots[i])); 
                

                GL_CALL(context, CreateBuffers(1, &vao->bindingsSlots[i]));
                GL_CALL(context, NamedBufferData(vao->bindingsSlots[i], vao->bindingSlotsToUpdate[i]->size, vao->bindingSlotsToUpdate[i]->data, GL_DYNAMIC_DRAW));
                GL_CALL(context, VertexArrayVertexBuffer(vao->VAO, i, vao->bindingsSlots[i], vao->bindingSlotsToUpdate[i]->offset, vao->bindingSlotsToUpdate[i]->stride));

                vao->bindingSlotsToUpdate[i]->deleteCallback(vao->bindingSlotsToUpdate[i]);
                vao->bindingSlotsToUpdate[i] = nullptr;
            }
        }
        
        for (size_t i = 0; i < 16; i++)
        {

            if(vao->attacmentsToUpdate[i])
            {
                GL_CALL(context, EnableVertexArrayAttrib(vao->VAO, i));
                GL_CALL(context, VertexArrayAttribBinding(vao->VAO, i, vao->attacmentsToUpdate[i]->bindingSlot));
                GL_CALL(context, VertexArrayAttribFormat(vao->VAO, i, vao->attacmentsToUpdate[i]->size, vao->attacmentsToUpdate[i]->dataType, GL_FALSE, vao->attacmentsToUpdate[i]->relativeOffset));

                vao->attacmentsToUpdate[i]->deleteCallback(vao->attacmentsToUpdate[i]);
                vao->attacmentsToUpdate[i] = nullptr;
            }
        }
        
        if(vao->iboToUpdate)
        {
            if(vao->IBO)
                GL_CALL(context, DeleteBuffers(1, &vao->IBO)); 
            
            GL_CALL(context, CreateBuffers(1, &vao->IBO));

            GL_CALL(context, NamedBufferData(vao->IBO, vao->iboToUpdate->count * sizeof(unsigned int), vao->iboToUpdate->data, GL_DYNAMIC_DRAW));
            GL_CALL(context, VertexArrayElementBuffer(vao->VAO, vao->IBO));
            vao->count = vao->iboToUpdate->count;
            

            vao->iboToUpdate->deleteCallback(vao->iboToUpdate);
            vao->iboToUpdate = nullptr;
        }
        
        GL_CALL(context, BindVertexArray(vao->VAO));
        GL_CALL(context, BindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->IBO));


    }

}
