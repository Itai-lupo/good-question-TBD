#pragma once
#include <wayland-client.h>
#include <vector>
#include <functional>

#include "core.hpp"
#include "mouseButtonData.hpp"
#include "mouseMoveData.hpp"
#include "mouseScrollData.hpp"

class pointer
{
    public:
        static inline wl_pointer *pointerHandler = NULL;

        static void wlPointerEnter(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
        static void wlPointerLeave(void *data, wl_pointer *wlPointer, uint32_t serial, struct wl_surface *surface);
        static void wlPointerMotion(void *data, wl_pointer *wlPointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y);
        static void wlPointerButton(void *data, wl_pointer *wlPointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
        static void wlPointerAxis(void *data, wl_pointer *wlPointer, uint32_t time, uint32_t axis, wl_fixed_t value);
        static void wlPointerFrame(void *data, wl_pointer *wlPointer);
        static void wlPointerAxisSource(void *data, wl_pointer *wlPointer, uint32_t axis_source);
        static void wlPointerAxisStop(void *data, wl_pointer *wlPointer, uint32_t time, uint32_t axis);
        static void wlPointerAxisDiscrete(void *data, wl_pointer *wlPointer, uint32_t axis, int32_t discrete);
        static constexpr wl_pointer_listener wlPointerListener = {
            .enter = wlPointerEnter,
            .leave = wlPointerLeave,
            .motion = wlPointerMotion,
            .button = wlPointerButton,
            .axis = wlPointerAxis,
            .frame = wlPointerFrame,
            .axis_source = wlPointerAxisSource,
            .axis_stop = wlPointerAxisStop,
            .axis_discrete = wlPointerAxisDiscrete
        };
        
        struct inputBuffer
        {
            union
            {
                struct 
                {
                    uint8_t pointerEnter: 1;
                    uint8_t pointerLeave: 1;
                    uint8_t pointerMotion: 1;
                    uint8_t pointerButton: 1;
                    uint8_t pointerAxis: 1;
                    uint8_t pointerAxisSource: 1;
                    uint8_t pointerAxisStop: 1;
                    uint8_t pointerAxisDiscrete: 1;
                } eventTypes;
                uint8_t eventTypesReg;
            };

            windowId winId;
            wl_fixed_t surface_x, surface_y;
            uint32_t button, state;
            uint32_t time;
            uint32_t serial;
            struct {
                    bool valid;
                    wl_fixed_t value;
                    int32_t discrete;
            } axes[2];
            uint32_t axis_source;
            
        };

        struct idToPointerEventIndexes
        {
            uint32_t gen: 8;
            uint32_t mouseButtonPressEventIndex: 8 = -1;
            uint32_t mouseButtonReleasedEventIndex: 8 = -1;
            uint32_t mouseMovedEventIndex: 8 = -1;
            uint32_t mouseScrollEventIndex: 8 = -1;
        };
        
        static inline std::vector<idToPointerEventIndexes> idToIndex;  

        static inline  inputBuffer inputFrameData;
        
        static inline std::vector<std::function<void(const mouseButtonData&)>> mouseButtonPressEventListeners;
        static inline std::vector<windowId> mouseButtonPressEventId;

        static inline std::vector<std::function<void(const mouseButtonData&)>> mouseButtonReleasedEventListeners;
        static inline std::vector<windowId> mouseButtonReleasedEventId;

        static inline std::vector<std::function<void(const mouseMoveData&)>> mouseMovedEventListeners;
        static inline std::vector<windowId> mouseMovedEventId;

        static inline std::vector<std::function<void(const mouseScrollData&)>> mouseScrollEventListeners;
        static inline std::vector<windowId> mouseScrollEventId;
        
        static void allocateWindowEvents(windowId winId);
        static void setMouseButtonPressEventListeners(windowId winId, std::function<void(const mouseButtonData&)> callback);
        static void setMouseButtonReleasedEventListeners(windowId winId, std::function<void(const mouseButtonData&)> callback);
        static void setMouseMovedListeners(windowId winId, std::function<void(const mouseMoveData&)> callback);
        static void setMouseScrollListeners(windowId winId, std::function<void(const mouseScrollData&)> callback);

        static void deallocateWindowEvents(windowId winId);
        static void unsetMouseButtonPressEventListeners(windowId winId);
        static void unsetMouseButtonReleasedEventListeners(windowId winId);        
        static void unsetMouseMovedListeners(windowId winId);
        static void unsetMouseScrollListeners(windowId winId);
};
