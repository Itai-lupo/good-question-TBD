#pragma once 

#include "core.hpp"
#include "entityPool.hpp"
#include "log.hpp"

#include <array>

struct VBOUpdateRequst;
struct IBOUpdateRequst;
struct attacmentUpdateRequst;

void VBOUpdateRequstDefaultDeleteCallback(VBOUpdateRequst* toDelete);
void IBOUpdateRequstDefaultDeleteCallback(IBOUpdateRequst* toDelete);
void attacmentUpdateRequstDefaultDeleteCallback(attacmentUpdateRequst* toDelete);


struct VAOInfo
{
    vaoId id;

    uint32_t VAO = 0;
    uint32_t IBO = 0;
    bool needToRebuild;
    
    IBOUpdateRequst *iboToUpdate = nullptr;
    VBOUpdateRequst *bindingSlotsToUpdate[16] = {0};
    attacmentUpdateRequst *attacmentsToUpdate[16] = {0};

    uint32_t bindingsSlots[16] = {0};
    uint32_t count;
};

class VAOsComponents
{
    private:
        entityPool *pool;

        std::vector<VAOInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            VAOsComponents *This = static_cast<VAOsComponents *>(data);
            This->deleteComponent(id);
        }

    public:
        VAOsComponents(entityPool *pool);
        ~VAOsComponents();

        void deleteComponent(entityId id);
        VAOInfo *getComponent(entityId id);
        void setComponent(entityId id, VAOInfo buffer);
        std::vector<VAOInfo>& getData()
        {
            return data;
        }
};


struct VBOUpdateRequst
{
    void *data;
    uint32_t size; 
    uint32_t stride;
    uint32_t offset;

    void(*deleteCallback)(VBOUpdateRequst*) = VBOUpdateRequstDefaultDeleteCallback;
};

struct IBOUpdateRequst
{
    uint32_t *data = nullptr;
    uint32_t count = 0;

    void(*deleteCallback)(IBOUpdateRequst*) = IBOUpdateRequstDefaultDeleteCallback;
};

struct attacmentUpdateRequst
{
    uint32_t bindingSlot;
    uint32_t size; 
    uint32_t relativeOffset;
    uint32_t dataType = 0x1406; 

    void(*deleteCallback)(attacmentUpdateRequst*) = attacmentUpdateRequstDefaultDeleteCallback;
};