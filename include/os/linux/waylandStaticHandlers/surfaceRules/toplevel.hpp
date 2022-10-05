#pragma once

#include <xdg-decoration-client-protocol.h>
#include <xdg-shell-client-protocol.h>
#include <wayland-client.h>

#include <string>
#include <vector>
#include <functional>

#include "core.hpp"
#include "surface.hpp"

class toplevel
{
    private:
		static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial);
		static constexpr struct xdg_surface_listener xdg_surface_listener = {
            .configure = xdg_surface_configure,
        };	

        static void toplevelDecorationConfigure(void *data, zxdg_toplevel_decoration_v1 *zxdg_toplevelDecoration, uint32_t mode){}
        static constexpr zxdg_toplevel_decoration_v1_listener toplevelDecorationListener = {
            .configure = toplevelDecorationConfigure
        };

        
        static void xdgTopLevelConfigure(void *data, xdg_toplevel *xdgToplevel, int32_t width, int32_t height, wl_array *states);
        static void xdgTopLevelClose(void *data, xdg_toplevel *xdgToplevel);
        static void xdgTopLevelConfigureBounds(void *data, xdg_toplevel *xdgToplevel, int32_t width, int32_t height){}
        static constexpr xdg_toplevel_listener xdgTopLevelListener = {
            .configure = xdgTopLevelConfigure,
            .close = xdgTopLevelClose,
            .configure_bounds = xdgTopLevelConfigureBounds
        };

    
    public:
        static inline xdg_wm_base *xdgWmBase;
        static inline zxdg_decoration_manager_v1 *decorationManger;

        struct idToSurfaceDataIndexes
        {
            uint8_t gen;
            uint8_t toplevelDataIndex;
            uint8_t resizeEventIndex;
            uint8_t closeEventIndex;
            idToSurfaceDataIndexes(): toplevelDataIndex(-1), resizeEventIndex(-1), closeEventIndex(-1){}
        };
        
        static inline std::array<idToSurfaceDataIndexes, 255> idToIndex;  

        struct toplevelSurfaceInfo
        {
            /* data */
            surfaceId id;
            std::string title;


            xdg_surface *xdgSurface; // to do add child windows support as well as popup support
            xdg_toplevel *xdgToplevel;
            zxdg_toplevel_decoration_v1 *topLevelDecoration;
        };

        static inline std::vector<toplevelSurfaceInfo> topLevelSurfaces;
        static inline std::vector<std::function<void()>> closeEventListeners;
        static inline std::vector<surfaceId> closeEventId;

        static inline std::vector<std::function<void(const windowResizeData&)>> resizeEventListeners;
        static inline std::vector<surfaceId> resizeEventId;

        static void setWindowTitle(surfaceId id, const std::string& title);
        static std::string getWindowTitle(surfaceId id);

        static void allocateTopLevel(surfaceId winId, wl_surface *s, const surfaceSpec& surfaceData);
        static void setCloseEventListener(surfaceId winId, std::function<void()> callback);
        static void setResizeEventListener(surfaceId winId, std::function<void(const windowResizeData&)> callback);

        static void deallocateTopLevel(surfaceId winId);        
        static void unsetCloseEventListener(surfaceId winId);
        static void unsetResizeEventListener(surfaceId winId);
        
};