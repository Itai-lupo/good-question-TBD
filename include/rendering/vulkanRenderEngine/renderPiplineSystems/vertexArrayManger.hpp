#pragma once
#include "core.hpp"
#include "log.hpp"

#include "entityPool.hpp"
#include "VAOsComponents.hpp"

#include <list>
#include <vector>
#include <queue>



namespace vulkanRenderEngine
{

    class vaos
    {
        private:
            static inline VAOsComponents *vaosData;


        public:
            static inline std::queue<uint32_t> toDelete;
            
            static void init(entityPool *vaosPool);
            static void close();
            
            static void setContext();
            static void handleRequsets();

            static void setVAOData(VAOInfo info);
            static VAOInfo *getVAO(vaoId id);
            
            static void bind(vaoId id);
    };

}
