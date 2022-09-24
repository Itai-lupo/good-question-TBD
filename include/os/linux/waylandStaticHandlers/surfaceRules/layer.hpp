#pragma once

#include <wlr-layer-shell-client-protocol.h>
#include <wayland-client.h>

#include <string>
#include <vector>
#include <functional>

#include "core.hpp"
#include "surface.hpp"

class layer
{
    private:
		static void layerConfigure(void *data, zwlr_layer_surface_v1 *wlrLayer, uint32_t serial, uint32_t width, uint32_t height);
        static void layerClose(void *data, zwlr_layer_surface_v1 *wlrLayer);
		static constexpr struct zwlr_layer_surface_v1_listener layerSurfaceListener = {
            .configure = layerConfigure,
            .closed = layerClose
        };	

        
    
    public:
        static inline zwlr_layer_shell_v1 *wlrLayerShell;

        struct idToSurfaceDataIndexes
        {
            uint8_t gen;
            uint8_t layerDataIndex = -1;
            uint8_t resizeEventIndex = -1;
            uint8_t closeEventIndex = -1;
        };
        
        static inline std::vector<idToSurfaceDataIndexes> idToIndex;  

        struct layerSurfaceInfo
        {
            /* data */
            surfaceId id;
            std::string title;
            bool canRender;

            zwlr_layer_surface_v1 *layerSurface;
        };

        static inline std::vector<layerSurfaceInfo> layerSurfaces;
        static inline std::vector<std::function<void()>> closeEventListeners;
        static inline std::vector<surfaceId> closeEventId;

        static inline std::vector<std::function<void(const windowResizeData&)>> resizeEventListeners;
        static inline std::vector<surfaceId> resizeEventId;

        static void setWindowTitle(surfaceId id, const std::string& title);
        static std::string getWindowTitle(surfaceId id);

        static void allocateLayer(surfaceId winId, wl_surface *s, const surfaceSpec& surfaceData);
        static void setCloseEventListener(surfaceId winId, std::function<void()> callback);
        static void setResizeEventListener(surfaceId winId, std::function<void(const windowResizeData&)> callback);

        static void deallocateLayer(surfaceId winId);        
        static void unsetCloseEventListener(surfaceId winId);
        static void unsetResizeEventListener(surfaceId winId);
        
};