#pragma once

#include <xdg-decoration-client-protocol.h>
#include <xdg-shell-client-protocol.h>
#include <wayland-client.h>

#include <string>
#include <vector>
#include <functional>

#include "core.hpp"
#include "surface.hpp"

#include "windowCloseCallbackComponent.hpp"
#include "windowResizeCallbackComponent.hpp"
#include "toplevelInfoComponenet.hpp"

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

    
        static inline windowCloseCallbackComponent *closeCallbacks;
        static inline windowResizeCallbackComponent *resizeCallbacks;
        static inline toplevelInfoComponenet *topLevelSurfaces;

    public:
        static inline xdg_wm_base *xdgWmBase;
        static inline zxdg_decoration_manager_v1 *decorationManger;

        
        static inline void init(entityPool *surfacePool)
        {
            closeCallbacks = new windowCloseCallbackComponent(surfacePool);
            resizeCallbacks = new windowResizeCallbackComponent(surfacePool);
            topLevelSurfaces = new toplevelInfoComponenet(surfacePool);
        }

        static inline void close()
        {
            delete closeCallbacks;
            delete resizeCallbacks;
            delete topLevelSurfaces;
        }

        static void setWindowTitle(surfaceId id, const std::string& title);
        static std::string getWindowTitle(surfaceId id);

        static void allocateTopLevel(surfaceId winId, wl_surface *s, const surfaceSpec& surfaceData);
        static void setCloseEventListener(surfaceId winId, void(*callback)(surfaceId));
        static void setResizeEventListener(surfaceId winId, void(*callback)(const windowResizeData&));

        static void deallocateTopLevel(surfaceId winId);        
        static void unsetCloseEventListener(surfaceId winId);
        static void unsetResizeEventListener(surfaceId winId);
        
};