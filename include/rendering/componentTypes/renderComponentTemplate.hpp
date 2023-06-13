#pragma once

#include "core.hpp"
#include "entityPool.hpp"
#include "supportedRenderApis.hpp"

/**
 * @brief basic component container for a renderer system.
 *
 * @warning you should not hold onto the members pointers as they might move around while in use.
 * @warning don't add to the data while you iterate it if you didn't pre allocate it because realloc will change it
 */
class renderComponentTemplate
{
private:
    entityPool *pool;

    size_t dataSize;

    size_t dataMaxAllocated;
    size_t dataAllocated = 0;
    char *data;

    uint32_t *IdToIndex;
    entityId *indexToId;

    static void deleteCallback(void *data, entityId id)
    {
        renderComponentTemplate *This = static_cast<renderComponentTemplate *>(data);
        This->deleteComponent(id);
    }

    /**
     * @brief Get the Component raw ptr
     *
     * @param id the component id
     * @return void* the raw ptr
     */
    void *getComponentBuffer(entityId id) noexcept;

public:
    /**
     * @brief init the data strucure and list it in pool
     *
     *@param pool the id pool to use
     *@param dataSize the size of one component
     */
    renderComponentTemplate(entityPool *pool, size_t dataSize) noexcept;

    /**
     * @brief unlist the data from pool
     *
     */
    ~renderComponentTemplate() noexcept;

    /**
     * @brief delete a component
     *
     * @param id  the component to delete
     */
    void deleteComponent(entityId id) noexcept;

    /**
     * @brief Set the Component object(you might want to use getComponent to set the data)
     *
     * @param id the Component to set
     * @param buffer the data to set
     */
    void setComponent(entityId id, void *buffer) noexcept;

    /**
     * @brief Get the Component
     *
     * @tparam T the type of the Component
     * @param id the Component id
     * @return T& the Component
     */
    template <class T>
    T &getComponent(entityId id) noexcept
    {
        return *static_cast<T *>(getComponentBuffer(id));
    }

    /**
     * @brief check if id is valid to use(aka Component exists)
     *
     * @param id the id to check
     * @return if the Component exists
     */
    bool isIdValid(entityId id) noexcept;

    /**
     * @brief Get the Data start raw pointer used to itrater over all the obejects
     *
     * @return void* data
     */
    void *getData();

    /**
     * @brief Get the pointer to the end of the array
     *
     * @return void* last element pointer + dataSize
     */
    void *getLastElement();
};
