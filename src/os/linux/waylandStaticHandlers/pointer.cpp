#ifdef __linux__

#include "pointer.hpp"
#include "log.hpp"
#include "linuxWindowAPI.hpp"

void pointer::wlPointerEnter(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    LOG_INFO( surface << ", " << surface_x << ", " << surface_y);
    
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
        LOG_INFO(windowInfo.title);

    }


    if(frameData.eventTypes.pointerMotion)
    {    
        // LOG_INFO("pointer motion (" << windowInfo.title <<"): "<< wl_fixed_to_int(frameData.surface_x) << ", " << wl_fixed_to_int(frameData.surface_y));
    }

    if(frameData.eventTypes.pointerButton)
    {    
        LOG_INFO("pointer button (" << windowInfo.title <<"): " << frameData.button << ", " << frameData.state);
    }

    static constexpr char *axis_name[2] = {
        [WL_POINTER_AXIS_VERTICAL_SCROLL] = "vertical",
        [WL_POINTER_AXIS_HORIZONTAL_SCROLL] = "horizontal"
    };
    static constexpr char *axis_source[4] = {
        [WL_POINTER_AXIS_SOURCE_WHEEL] = "wheel",
        [WL_POINTER_AXIS_SOURCE_FINGER] = "finger",
        [WL_POINTER_AXIS_SOURCE_CONTINUOUS] = "continuous",
        [WL_POINTER_AXIS_SOURCE_WHEEL_TILT] = "wheel tilt"
    };

    if(frameData.eventTypes.pointerAxis)
    {
        for (size_t i = 0; i < 2; i++)
        {
            CONDTION_LOG_INFO(
                "pointer axis " << i << " (" << windowInfo.title <<"): " << 
                axis_name[i] << ", " << wl_fixed_to_int(frameData.axes[i].value) << ", " << frameData.axes[i].discrete << ", " <<  axis_source[frameData.axis_source], 
                frameData.axes[i].valid);
                frameData.axes[i].valid = false;

        }
        

        if(frameData.eventTypes.pointerLeave)
        {
            LOG_INFO(windowInfo.title);
            frameData.windowIndex = -1;
        }
    }
    frameData.eventTypesReg = 0;

}

#endif