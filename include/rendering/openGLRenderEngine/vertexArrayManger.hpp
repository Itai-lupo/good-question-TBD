#pragma once
#include "openglContext.hpp"
#include "core.hpp"
#include "log.hpp"

#include "entityPool.hpp"
#include "VAOsComponents.hpp"

#include <list>
#include <vector>
#include <queue>


// struct VBOSpec
// {
//     uint32_t bindingSlot;
//     const void *data;
//     uint32_t size; 
//     uint32_t stride;
//     uint32_t offset = 0;
//     uint32_t dataType = GL_FLOAT; 
// };


namespace openGLRenderEngine
{

    class vaos
    {
        private:
            static inline openglContext *context;
            static inline VAOsComponents *vaosData;


        public:
            static inline std::queue<uint32_t> toDelete;
            
            static void init(entityPool *vaosPool);
            static void close();
            
            static void setContext(openglContext *context);
            static void handleRequsets();

            static void setVAOData(VAOInfo info);
            static VAOInfo *getVAO(vaoId id);
            
            static void bind(vaoId id);
    };

}
