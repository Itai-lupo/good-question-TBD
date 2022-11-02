#pragma once
#include "openglContext.hpp"
#include "core.hpp"
#include "log.hpp"

#include <list>
#include <vector>
#include <queue>


struct VBOSpec
{
    uint32_t bindingSlot;
    const void *data;
    uint32_t size; 
    uint32_t stride;
    uint32_t offset = 0;
    uint32_t dataType = GL_FLOAT; 
};



class vertexArrayManger
{
    private:
        openglContext *context;
        struct VAOInfo
        {
            uint32_t VAO = 0;
            uint32_t IBO = 0;

            struct VBO
            {
                const void *data;
                uint32_t size; 
                uint32_t stride;
                uint32_t offset;
                uint32_t renderId;
            } bindingsSlots[16];

            struct VBOAttacment
            {
                uint32_t bindingSlot;
                uint32_t size; 
                uint32_t offset;
                uint32_t dataType; 

            } attributes[16];

            uint32_t count;

            vaoId id;
        };

        struct VAORequstes
        {
            std::queue<uint8_t> rebuildBinding;
            std::queue<uint8_t> rebuildAttacment;
            
            const uint32_t *IBOToBuild = nullptr;
            uint32_t IBOsize = 0;
            vaoId id;

        };
        

        std::vector<VAOInfo> buffers;
        std::vector<VAORequstes> requstes;

        struct idIndexes
        {
            uint32_t gen: 8 = -1;
            uint32_t index: 24;
        };        

        std::vector<idIndexes> idToIndex;  
        std::list<uint32_t> freeSlots;

        std::vector<vaoId> needToRebuild;
    public:
        vertexArrayManger(openglContext *context):context(context)
        {
            int max;
            GL_CALL(context, GetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max));
            GL_CALL(context, GetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &max));
        }
        
        vaoId createVao();
        void addVertexBufferBinding(vaoId id, const VBOSpec& data);
        void addVertexBufferAttacment(vaoId id, uint32_t slot, uint32_t binding, uint32_t size, uint32_t offset = 0, uint32_t dataType = GL_FLOAT);

        void attachIndexBuffer(vaoId id, const uint32_t *IBO, uint32_t size);
        
        void bind(vaoId b);
        
        int getCount(vaoId b);
};


