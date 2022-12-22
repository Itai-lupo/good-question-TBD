#ifdef __linux__

#include "pointer.hpp"
#include "log.hpp"
#include "surface.hpp"


#include <linux/input.h>
#include <thread>
#include <Tracy.hpp>

void pointer::init(entityPool *surfacesesPool)
{
    mouseButtonPressEventListeners = new mouseButtonCallbackComponent(surfacesesPool);
    mouseButtonReleasedEventListeners = new mouseButtonCallbackComponent(surfacesesPool);
    mouseMovedEventListeners = new mouseMoveCallbackComponent(surfacesesPool);
    mouseScrollEventListeners = new mouseScrollCallbackComponent(surfacesesPool);
}

void pointer::close()
{
    delete mouseButtonPressEventListeners;
    delete mouseButtonReleasedEventListeners;
    delete mouseMovedEventListeners;
    delete mouseScrollEventListeners;

}

void pointer::wlPointerEnter(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{    
    ZoneScoped;
    for (auto& surf: surface::surfacesInfo->getData())
    {
        if(surf.surface == surface)
        {
            inputFrameData.eventTypes.pointerEnter = true;
            inputFrameData.id = surf.id;
            inputFrameData.surface_x = surface_x;
            inputFrameData.surface_y = surface_y;
            inputFrameData.serial = serial;
            return;
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
    

    if(frameData.eventTypes.pointerEnter)
    {
        
    }

    mouseMoveCallback temp = mouseMovedEventListeners->getCallback(frameData.id);
    if(frameData.eventTypes.pointerMotion && temp)
            std::thread(temp, 
                mouseMoveData{frameData.id, wl_fixed_to_int(frameData.surface_x), wl_fixed_to_int(frameData.surface_y)}).detach();


    if(frameData.eventTypes.pointerButton)
    {
        mouseCallback tempMouse = mouseButtonPressEventListeners->getCallback(frameData.id);
        if(frameData.state == 1 && tempMouse)
            std::thread(
                mouseButtonPressEventListeners->getCallback(frameData.id), 
                mouseButtonData{frameData.id, mouseButtons(frameData.button - BTN_MOUSE)}).detach();


        tempMouse = mouseButtonPressEventListeners->getCallback(frameData.id);
        if(frameData.state == 0 && tempMouse)
            std::thread(
                mouseButtonReleasedEventListeners->getCallback(frameData.id), 
                mouseButtonData{frameData.id, mouseButtons(frameData.button - BTN_MOUSE)}).detach();
    }


    scrollCallback tempScroll = mouseScrollEventListeners->getCallback(frameData.id);
    if(frameData.eventTypes.pointerAxis && tempScroll)
        for (size_t i = 0; i < 2; i++)
        {
            if(frameData.axes[i].valid)
                std::thread(
                    tempScroll, 
                    mouseScrollData{frameData.id, (mouseAxis)i, (mouseAxisSource)frameData.axis_source, wl_fixed_to_int(frameData.axes[i].value), frameData.axes[i].discrete}).detach();
        }

    if(frameData.eventTypes.pointerLeave)
    {
        
    }
    
    frameData.eventTypesReg = 0;
}


void pointer::setMouseButtonPressEventListeners(surfaceId id, void(*callback)(const mouseButtonData&))
{
    mouseButtonPressEventListeners->setCallback(id, callback);
}

void pointer::setMouseButtonReleasedEventListeners(surfaceId id, void(*callback)(const mouseButtonData&))
{
    mouseButtonReleasedEventListeners->setCallback(id, callback);
}

void pointer::setMouseMovedListeners(surfaceId id, void(*callback)(const mouseMoveData&))
{
    mouseMovedEventListeners->setCallback(id, callback);
}

void pointer::setMouseScrollListeners(surfaceId id, void(*callback)(const mouseScrollData&))
{
    mouseScrollEventListeners->setCallback(id, callback);
}


void pointer::unsetMouseButtonPressEventListeners(surfaceId id)
{
    mouseButtonPressEventListeners->deleteComponent(id);
}

void pointer::unsetMouseButtonReleasedEventListeners(surfaceId id)
{
    mouseButtonReleasedEventListeners->deleteComponent(id);
}

void pointer::unsetMouseMovedListeners(surfaceId id)
{
    mouseMovedEventListeners->deleteComponent(id);
}

void pointer::unsetMouseScrollListeners(surfaceId id)
{
    mouseScrollEventListeners->deleteComponent(id);
}



#endif