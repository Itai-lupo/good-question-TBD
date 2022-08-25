#ifdef __linux__

#include "pointer.hpp"
#include "log.hpp"
#include "linuxWindowAPI.hpp"


#include <linux/input.h>

void pointer::wlPointerEnter(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{    
    for (size_t i = 0; i < linuxWindowAPI::windowsInfo.size(); i++)
    {
        if(linuxWindowAPI::windowsInfo[i].surface == surface)
        {
            inputFrameData.eventTypes.pointerEnter = true;
            inputFrameData.windowIndex = i;
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
    
    if(frameData.windowIndex == -1)
        return;

    linuxWindowAPI::windowInfo& windowInfo = linuxWindowAPI::windowsInfo[frameData.windowIndex];

    if(frameData.eventTypes.pointerEnter)
    {
        
    }

    if(frameData.eventTypes.pointerMotion)
    {    
        if(windowInfo.mouseMovedListenrs)
            std::thread(
                windowInfo.mouseMovedListenrs, 
                mouseMoveData{wl_fixed_to_int(frameData.surface_x), wl_fixed_to_int(frameData.surface_y)}).detach();
    }

    if(frameData.eventTypes.pointerButton)
    {    
        if(frameData.state ? windowInfo.mouseButtonPressEventListenrs: windowInfo.mouseButtonReleasedEventListenrs)
            std::thread(
                (frameData.state ? windowInfo.mouseButtonPressEventListenrs: windowInfo.mouseButtonReleasedEventListenrs), 
                mouseButtonData{mouseButtons(frameData.button - BTN_MOUSE)}).detach();
    }

    if(frameData.eventTypes.pointerAxis && windowInfo.mouseScrollListenrs)
        for (size_t i = 0; i < 2; i++)
        {
            if(frameData.axes[i].valid)
                std::thread(
                    windowInfo.mouseScrollListenrs, 
                    mouseScrollData{(mouseAxis)i, (mouseAxisSource)frameData.axis_source, wl_fixed_to_int(frameData.axes[i].value), frameData.axes[i].discrete}).detach();
        }

    if(frameData.eventTypes.pointerLeave)
    {
        
    }
    
    frameData.eventTypesReg = 0;
}

#endif