#pragma once
#include <wayland-client.h>
#include <vector>
#include <functional>

#include "core.hpp"
#include "surface.hpp"
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

            surfaceId id;
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
            uint8_t gen = -1;
            uint8_t mouseButtonPressEventIndex = -1;
            uint8_t mouseButtonReleasedEventIndex = -1;
            uint8_t mouseMovedEventIndex = -1;
            uint8_t mouseScrollEventIndex = -1;
        };
        
        static inline std::vector<idToPointerEventIndexes> idToIndex;  

        static inline  inputBuffer inputFrameData;
        
        static inline std::vector<std::function<void(const mouseButtonData&)>> mouseButtonPressEventListeners;
        static inline std::vector<surfaceId> mouseButtonPressEventId;

        static inline std::vector<std::function<void(const mouseButtonData&)>> mouseButtonReleasedEventListeners;
        static inline std::vector<surfaceId> mouseButtonReleasedEventId;

        static inline std::vector<std::function<void(const mouseMoveData&)>> mouseMovedEventListeners;
        static inline std::vector<surfaceId> mouseMovedEventId;

        static inline std::vector<std::function<void(const mouseScrollData&)>> mouseScrollEventListeners;
        static inline std::vector<surfaceId> mouseScrollEventId;
        
        static void allocateWindowEvents(surfaceId id);
        static void setMouseButtonPressEventListeners(surfaceId id, std::function<void(const mouseButtonData&)> callback);
        static void setMouseButtonReleasedEventListeners(surfaceId id, std::function<void(const mouseButtonData&)> callback);
        static void setMouseMovedListeners(surfaceId id, std::function<void(const mouseMoveData&)> callback);
        static void setMouseScrollListeners(surfaceId id, std::function<void(const mouseScrollData&)> callback);

        static void deallocateWindowEvents(surfaceId id);
        static void unsetMouseButtonPressEventListeners(surfaceId id);
        static void unsetMouseButtonReleasedEventListeners(surfaceId id);        
        static void unsetMouseMovedListeners(surfaceId id);
        static void unsetMouseScrollListeners(surfaceId id);
};
