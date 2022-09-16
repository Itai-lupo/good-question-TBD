#ifdef __linux__

#include "pointer.hpp"
#include "log.hpp"
#include "window.hpp"


#include <linux/input.h>
#include <thread>


void pointer::wlPointerEnter(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{    
    for (size_t i = 0; i < window::windows.size(); i++)
    {
        if(window::windows[i].surface == surface)
        {
            inputFrameData.eventTypes.pointerEnter = true;
            inputFrameData.winId = window::windows[i].id;
            inputFrameData.surface_x = surface_x;
            inputFrameData.surface_y = surface_y;
            inputFrameData.serial = serial;
        }
    }    
}

void pointer::wlPointerLeave(void *data, wl_pointer *wlPointer, uint32_t serial, struct wl_surface *surface)
{
    inputFrameData.eventTypes.pointerLeave = true;
    inputFrameData.serial = serial;
}

void pointer::wlPointerMotion(void *data, wl_pointer *wlPointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    inputFrameData.eventTypes.pointerMotion = true;
    inputFrameData.time = time;
    inputFrameData.surface_x = surface_x;
    inputFrameData.surface_y = surface_y;
}

void pointer::wlPointerButton(void *data, wl_pointer *wlPointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    inputFrameData.eventTypes.pointerButton = true;
    inputFrameData.time = time;
    inputFrameData.button = button;
    inputFrameData.state = state;
    inputFrameData.serial = serial;
}

void pointer::wlPointerAxis(void *data, wl_pointer *wlPointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
    inputFrameData.eventTypes.pointerAxis = true;
    inputFrameData.time = time;
    inputFrameData.axes[axis].valid = true;
    inputFrameData.axes[axis].value = value;
}

void pointer::wlPointerAxisSource(void *data, wl_pointer *wlPointer, uint32_t axis_source)
{   
    inputFrameData.eventTypes.pointerAxisSource = true;
    inputFrameData.axis_source = axis_source;
}

void pointer::wlPointerAxisStop(void *data, wl_pointer *wlPointer, uint32_t time, uint32_t axis)
{
    inputFrameData.eventTypes.pointerAxisStop = true;
    inputFrameData.time = time;
    inputFrameData.axes[axis].valid = true;
}

void pointer::wlPointerAxisDiscrete(void *data, wl_pointer *wlPointer, uint32_t axis, int32_t discrete)
{
    inputFrameData.eventTypes.pointerAxisDiscrete = true;
    inputFrameData.axes[axis].discrete = discrete;
}

void pointer::wlPointerFrame(void *data, wl_pointer *wlPointer)
{
    inputBuffer& frameData = inputFrameData;
    
    if(frameData.winId.index == -1 || frameData.winId.gen != idToIndex[frameData.winId.index].gen)
        return;

    if(frameData.eventTypes.pointerEnter)
    {
        
    }

    int index = idToIndex[frameData.winId.index].mouseMovedEventIndex;
    if(frameData.eventTypes.pointerMotion)
    {    
        if(index != -1)
            std::thread(
                mouseMovedEventListeners[index], 
                mouseMoveData{wl_fixed_to_int(frameData.surface_x), wl_fixed_to_int(frameData.surface_y)}).detach();
    }

    if(frameData.eventTypes.pointerButton)
    {
        index = idToIndex[frameData.winId.index].mouseButtonPressEventIndex;
        if(frameData.state == 1 && index != -1)
            std::thread(
                mouseButtonPressEventListeners[index], 
                mouseButtonData{mouseButtons(frameData.button - BTN_MOUSE)}).detach();


        index = idToIndex[frameData.winId.index].mouseButtonReleasedEventIndex;
        if(frameData.state == 0 && index != -1)
            std::thread(
                mouseButtonReleasedEventListeners[index], 
                mouseButtonData{mouseButtons(frameData.button - BTN_MOUSE)}).detach();
    }


    index = idToIndex[frameData.winId.index].mouseScrollEventIndex;
    if(frameData.eventTypes.pointerAxis && index != -1)
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


void pointer::allocateWindowEvents(windowId winId)
{
    if(winId.index >= idToIndex.size())
        idToIndex.resize(winId.index + 1);

    idToIndex[winId.index].gen = winId.gen;
}

void pointer::setMouseButtonPressEventListeners(windowId winId, std::function<void(const mouseButtonData&)> callback)
{
    uint32_t index = idToIndex[winId.index].mouseButtonPressEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        mouseButtonPressEventListeners[index] = callback;
        mouseButtonPressEventId[index] = winId;
        return;
    }

    idToIndex[winId.index].mouseButtonPressEventIndex = mouseButtonPressEventListeners.size();
    mouseButtonPressEventListeners.push_back(callback);
    mouseButtonPressEventId.push_back(winId);
}

void pointer::setMouseButtonReleasedEventListeners(windowId winId, std::function<void(const mouseButtonData&)> callback)
{
    uint32_t index = idToIndex[winId.index].mouseButtonReleasedEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        mouseButtonReleasedEventListeners[index] = callback;
        mouseButtonReleasedEventId[index] = winId;
        return;
    }

    idToIndex[winId.index].mouseButtonReleasedEventIndex = mouseButtonReleasedEventListeners.size();
    mouseButtonReleasedEventListeners.push_back(callback);
    mouseButtonReleasedEventId.push_back(winId);
}

void pointer::setMouseMovedListeners(windowId winId, std::function<void(const mouseMoveData&)> callback)
{
    uint32_t index = idToIndex[winId.index].mouseMovedEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        mouseMovedEventListeners[index] = callback;
        mouseMovedEventId[index] = winId;
        return;
    }

    idToIndex[winId.index].mouseMovedEventIndex = mouseMovedEventListeners.size();
    mouseMovedEventListeners.push_back(callback);
    mouseMovedEventId.push_back(winId);
}

void pointer::setMouseScrollListeners(windowId winId, std::function<void(const mouseScrollData&)> callback)
{
    uint32_t index = idToIndex[winId.index].mouseScrollEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        mouseScrollEventListeners[index] = callback;
        mouseScrollEventId[index] = winId;
        return;
    }

    idToIndex[winId.index].mouseScrollEventIndex = mouseScrollEventListeners.size();
    mouseScrollEventListeners.push_back(callback);
    mouseScrollEventId.push_back(winId);
}



void pointer::deallocateWindowEvents(windowId winId)
{
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    unsetMouseButtonPressEventListeners(winId);
    unsetMouseButtonReleasedEventListeners(winId);
    unsetMouseMovedListeners(winId);
    unsetMouseScrollListeners(winId);

    idToIndex[winId.index].gen = -1;
}

void pointer::unsetMouseButtonPressEventListeners(windowId winId)
{
    uint32_t index = idToIndex[winId.index].mouseButtonPressEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = mouseButtonPressEventListeners.size() - 1;
    idToIndex[mouseButtonPressEventId[lastIndex].index].mouseButtonPressEventIndex = index;
    mouseButtonPressEventListeners[index] = mouseButtonPressEventListeners[lastIndex];
    mouseButtonPressEventId[index] = mouseButtonPressEventId[lastIndex];

    mouseButtonPressEventListeners.pop_back();
    mouseButtonPressEventId.pop_back();

    idToIndex[winId.index].mouseButtonPressEventIndex = -1;
}

void pointer::unsetMouseButtonReleasedEventListeners(windowId winId)
{
    uint32_t index = idToIndex[winId.index].mouseButtonReleasedEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = mouseButtonReleasedEventListeners.size() - 1;
    idToIndex[mouseButtonReleasedEventId[lastIndex].index].mouseButtonReleasedEventIndex = index;
    mouseButtonReleasedEventListeners[index] = mouseButtonReleasedEventListeners[lastIndex];
    mouseButtonReleasedEventId[index] = mouseButtonReleasedEventId[lastIndex];

    mouseButtonReleasedEventListeners.pop_back();
    mouseButtonReleasedEventId.pop_back();

    idToIndex[winId.index].mouseButtonReleasedEventIndex = -1;
}  

void pointer::unsetMouseMovedListeners(windowId winId)
{
    uint32_t index = idToIndex[winId.index].mouseMovedEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = mouseMovedEventListeners.size() - 1;
    idToIndex[mouseMovedEventId[lastIndex].index].mouseMovedEventIndex = index;
    mouseMovedEventListeners[index] = mouseMovedEventListeners[lastIndex];
    mouseMovedEventId[index] = mouseMovedEventId[lastIndex];

    mouseMovedEventListeners.pop_back();
    mouseMovedEventId.pop_back();

    idToIndex[winId.index].mouseMovedEventIndex = -1;
}

void pointer::unsetMouseScrollListeners(windowId winId)
{
    uint32_t index = idToIndex[winId.index].mouseScrollEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = mouseScrollEventListeners.size() - 1;
    idToIndex[mouseScrollEventId[lastIndex].index].mouseScrollEventIndex = index;
    mouseScrollEventListeners[index] = mouseScrollEventListeners[lastIndex];
    mouseScrollEventId[index] = mouseScrollEventId[lastIndex];

    mouseScrollEventListeners.pop_back();
    mouseScrollEventId.pop_back();

    idToIndex[winId.index].mouseScrollEventIndex = -1;
}



#endif