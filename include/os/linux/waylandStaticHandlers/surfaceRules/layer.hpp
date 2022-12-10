#pragma once

#include <wlr-layer-shell-client-protocol.h>
#include <wayland-client.h>

#include <string>
#include <vector>
#include <functional>

#include "core.hpp"
#include "surface.hpp"

#include "windowCloseCallbackComponent.hpp"
#include "windowResizeCallbackComponent.hpp"
#include "layerInfoComponenet.hpp"


class layer
{
    private:
		static void layerConfigure(void *data, zwlr_layer_surface_v1 *wlrLayer, uint32_t serial, uint32_t width, uint32_t height);
        static void layerClose(void *data, zwlr_layer_surface_v1 *wlrLayer);
		static constexpr struct zwlr_layer_surface_v1_listener layerSurfaceListener = {
            .configure = layerConfigure,
            .closed = layerClose
        };	

        static inline windowCloseCallbackComponent *closeCallbacks;
        static inline windowResizeCallbackComponent *resizeCallbacks;
        static inline layerInfoComponenet *layers;

    public:
        static inline zwlr_layer_shell_v1 *wlrLayerShell;


        static inline void init(entityPool *surfacePool)
        {
            closeCallbacks = new windowCloseCallbackComponent(surfacePool);
            resizeCallbacks = new windowResizeCallbackComponent(surfacePool);
            layers = new layerInfoComponenet(surfacePool);
        }

        static inline void close()
        {
            delete closeCallbacks;
            delete resizeCallbacks;
            delete layers;
        }
        static void setWindowTitle(surfaceId id, const std::string& title);
        static std::string getWindowTitle(surfaceId id);

        static void allocateLayer(surfaceId winId, wl_surface *s, const surfaceSpec& surfaceData);
        static void setCloseEventListener(surfaceId winId, void(*callback)(surfaceId));
        static void setResizeEventListener(surfaceId winId, void(*callback)(const windowResizeData&) );

        static void deallocateLayer(surfaceId winId);        
        static void unsetCloseEventListener(surfaceId winId);
        static void unsetResizeEventListener(surfaceId winId);
        
};