#ifdef __linux__

#include "pointer.hpp"
#include "log.hpp"
#include "surface.hpp"


#include <linux/input.h>
#include <thread>
#include <Tracy.hpp>


void pointer::wlPointerEnter(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{    
    ZoneScoped;
    for (size_t i = 0; i < surface::surfaces.size(); i++)
    {
        if(surface::surfaces[i].surface == surface)
        {
            inputFrameData.eventTypes.pointerEnter = true;
            inputFrameData.id = surface::surfaces[i].id;
            inputFrameData.surface_x = surface_x;
            inputFrameData.surface_y = surface_y;
            inputFrameData.serial = serial;
        }
    }    
}

void pointer::wlPointerLeave(void *data, wl_pointer *wlPointer, uint32_t serial, struct wl_surface *surface)
{
    ZoneScoped;
    inputFrameData.eventTypes.pointerLeave = true;
    inputFrameData.serial = serial;
}

void pointer::wlPointerMotion(void *data, wl_pointer *wlPointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    ZoneScoped;
    inputFrameData.eventTypes.pointerMotion = true;
    inputFrameData.time = time;
    inputFrameData.surface_x = surface_x;
    inputFrameData.surface_y = surface_y;
}

void pointer::wlPointerButton(void *data, wl_pointer *wlPointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    ZoneScoped;
    inputFrameData.eventTypes.pointerButton = true;
    inputFrameData.time = time;
    inputFrameData.button = button;
    inputFrameData.state = state;
    inputFrameData.serial = serial;
}

void pointer::wlPointerAxis(void *data, wl_pointer *wlPointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
    ZoneScoped;
    inputFrameData.eventTypes.pointerAxis = true;
    inputFrameData.time = time;
    inputFrameData.axes[axis].valid = true;
    inputFrameData.axes[axis].value = value;
}

void pointer::wlPointerAxisSource(void *data, wl_pointer *wlPointer, uint32_t axis_source)
{   
    ZoneScoped;
    inputFrameData.eventTypes.pointerAxisSource = true;
    inputFrameData.axis_source = axis_source;
}

void pointer::wlPointerAxisStop(void *data, wl_pointer *wlPointer, uint32_t time, uint32_t axis)
{
    ZoneScoped;
    inputFrameData.eventTypes.pointerAxisStop = true;
    inputFrameData.time = time;
    inputFrameData.axes[axis].valid = true;
}

void pointer::wlPointerAxisDiscrete(void *data, wl_pointer *wlPointer, uint32_t axis, int32_t discrete)
{
    ZoneScoped;
    inputFrameData.eventTypes.pointerAxisDiscrete = true;
    inputFrameData.axes[axis].discrete = discrete;
}

void pointer::wlPointerFrame(void *data, wl_pointer *wlPointer)
{
    ZoneScoped;
    inputBuffer& frameData = inputFrameData;
    
    if(frameData.id.index == (uint8_t)-1 || frameData.id.gen != idToIndex[frameData.id.index].gen)
        return;

    if(frameData.eventTypes.pointerEnter)
    {
        
    }

    int index = idToIndex[frameData.id.index].mouseMovedEventIndex;
    if(frameData.eventTypes.pointerMotion)
    {    
        if(index != (uint8_t)-1)
            std::thread(
                mouseMovedEventListeners[index], 
                mouseMoveData{wl_fixed_to_int(frameData.surface_x), wl_fixed_to_int(frameData.surface_y)}).detach();
    }

    if(frameData.eventTypes.pointerButton)
    {
        index = idToIndex[frameData.id.index].mouseButtonPressEventIndex;
        if(frameData.state == 1 && index != (uint8_t)-1)
            std::thread(
                mouseButtonPressEventListeners[index], 
                mouseButtonData{mouseButtons(frameData.button - BTN_MOUSE)}).detach();


        index = idToIndex[frameData.id.index].mouseButtonReleasedEventIndex;
        if(frameData.state == 0 && index != (uint8_t)-1)
            std::thread(
                mouseButtonReleasedEventListeners[index], 
                mouseButtonData{mouseButtons(frameData.button - BTN_MOUSE)}).detach();
    }


    index = idToIndex[frameData.id.index].mouseScrollEventIndex;
    if(frameData.eventTypes.pointerAxis && index != (uint8_t)-1)
        for (size_t i = 0; i < 2; i++)
        {
            if(frameData.axes[i].valid)
                std::thread(
                    mouseScrollEventListeners[index], 
                    mouseScrollData{(mouseAxis)i, (mouseAxisSource)frameData.axis_source, wl_fixed_to_int(frameData.axes[i].value), frameData.axes[i].discrete}).detach();
        }

    if(frameData.eventTypes.pointerLeave)
    {
        
    }
    
    frameData.eventTypesReg = 0;
}


void pointer::allocateWindowEvents(surfaceId id)
{
    if(id.index >= idToIndex.size())
        idToIndex.resize(id.index + 1);

    idToIndex[id.index].gen = id.gen;
}

void pointer::setMouseButtonPressEventListeners(surfaceId id, std::function<void(const mouseButtonData&)> callback)
{
    uint32_t index = idToIndex[id.index].mouseButtonPressEventIndex;
    if(idToIndex[id.index].gen != id.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        mouseButtonPressEventListeners[index] = callback;
        mouseButtonPressEventId[index] = id;
        return;
    }

    idToIndex[id.index].mouseButtonPressEventIndex = mouseButtonPressEventListeners.size();
    mouseButtonPressEventListeners.push_back(callback);
    mouseButtonPressEventId.push_back(id);
}

void pointer::setMouseButtonReleasedEventListeners(surfaceId id, std::function<void(const mouseButtonData&)> callback)
{
    uint32_t index = idToIndex[id.index].mouseButtonReleasedEventIndex;
    if(idToIndex[id.index].gen != id.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        mouseButtonReleasedEventListeners[index] = callback;
        mouseButtonReleasedEventId[index] = id;
        return;
    }

    idToIndex[id.index].mouseButtonReleasedEventIndex = mouseButtonReleasedEventListeners.size();
    mouseButtonReleasedEventListeners.push_back(callback);
    mouseButtonReleasedEventId.push_back(id);
}

void pointer::setMouseMovedListeners(surfaceId id, std::function<void(const mouseMoveData&)> callback)
{
    uint32_t index = idToIndex[id.index].mouseMovedEventIndex;
    if(idToIndex[id.index].gen != id.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        mouseMovedEventListeners[index] = callback;
        mouseMovedEventId[index] = id;
        return;
    }

    idToIndex[id.index].mouseMovedEventIndex = mouseMovedEventListeners.size();
    mouseMovedEventListeners.push_back(callback);
    mouseMovedEventId.push_back(id);
}

void pointer::setMouseScrollListeners(surfaceId id, std::function<void(const mouseScrollData&)> callback)
{
    uint32_t index = idToIndex[id.index].mouseScrollEventIndex;
    if(idToIndex[id.index].gen != id.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        mouseScrollEventListeners[index] = callback;
        mouseScrollEventId[index] = id;
        return;
    }

    idToIndex[id.index].mouseScrollEventIndex = mouseScrollEventListeners.size();
    mouseScrollEventListeners.push_back(callback);
    mouseScrollEventId.push_back(id);
}



void pointer::deallocateWindowEvents(surfaceId id)
{
    if(idToIndex[id.index].gen != id.gen)
        return;

    unsetMouseButtonPressEventListeners(id);
    unsetMouseButtonReleasedEventListeners(id);
    unsetMouseMovedListeners(id);
    unsetMouseScrollListeners(id);

    idToIndex[id.index].gen = -1;
}

void pointer::unsetMouseButtonPressEventListeners(surfaceId id)
{
    uint32_t index = idToIndex[id.index].mouseButtonPressEventIndex;
    if(idToIndex[id.index].gen != id.gen || index == -1)
        return;

    uint32_t lastIndex = mouseButtonPressEventListeners.size() - 1;
    idToIndex[mouseButtonPressEventId[lastIndex].index].mouseButtonPressEventIndex = index;
    mouseButtonPressEventListeners[index] = mouseButtonPressEventListeners[lastIndex];
    mouseButtonPressEventId[index] = mouseButtonPressEventId[lastIndex];

    mouseButtonPressEventListeners.pop_back();
    mouseButtonPressEventId.pop_back();

    idToIndex[id.index].mouseButtonPressEventIndex = -1;
}

void pointer::unsetMouseButtonReleasedEventListeners(surfaceId id)
{
    uint32_t index = idToIndex[id.index].mouseButtonReleasedEventIndex;
    if(idToIndex[id.index].gen != id.gen || index == -1)
        return;

    uint32_t lastIndex = mouseButtonReleasedEventListeners.size() - 1;
    idToIndex[mouseButtonReleasedEventId[lastIndex].index].mouseButtonReleasedEventIndex = index;
    mouseButtonReleasedEventListeners[index] = mouseButtonReleasedEventListeners[lastIndex];
    mouseButtonReleasedEventId[index] = mouseButtonReleasedEventId[lastIndex];

    mouseButtonReleasedEventListeners.pop_back();
    mouseButtonReleasedEventId.pop_back();

    idToIndex[id.index].mouseButtonReleasedEventIndex = -1;
}  

void pointer::unsetMouseMovedListeners(surfaceId id)
{
    uint32_t index = idToIndex[id.index].mouseMovedEventIndex;
    if(idToIndex[id.index].gen != id.gen || index == -1)
        return;

    uint32_t lastIndex = mouseMovedEventListeners.size() - 1;
    idToIndex[mouseMovedEventId[lastIndex].index].mouseMovedEventIndex = index;
    mouseMovedEventListeners[index] = mouseMovedEventListeners[lastIndex];
    mouseMovedEventId[index] = mouseMovedEventId[lastIndex];

    mouseMovedEventListeners.pop_back();
    mouseMovedEventId.pop_back();

    idToIndex[id.index].mouseMovedEventIndex = -1;
}

void pointer::unsetMouseScrollListeners(surfaceId id)
{
    uint32_t index = idToIndex[id.index].mouseScrollEventIndex;
    if(idToIndex[id.index].gen != id.gen || index == -1)
        return;

    uint32_t lastIndex = mouseScrollEventListeners.size() - 1;
    idToIndex[mouseScrollEventId[lastIndex].index].mouseScrollEventIndex = index;
    mouseScrollEventListeners[index] = mouseScrollEventListeners[lastIndex];
    mouseScrollEventId[index] = mouseScrollEventId[lastIndex];

    mouseScrollEventListeners.pop_back();
    mouseScrollEventId.pop_back();

    idToIndex[id.index].mouseScrollEventIndex = -1;
}



#endif