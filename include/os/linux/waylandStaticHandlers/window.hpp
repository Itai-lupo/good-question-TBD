#pragma once

#include "core.hpp"
#include "windowResizeData.hpp"
#include "osAPI.hpp"


#include <vector>
#include <string>
#include <functional>
#include <wayland-client.h>
#include <xdg-decoration-client-protocol.h>
#include <xdg-shell-client-protocol.h>

class window
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
        static inline wl_compositor *compositor = NULL;
        static inline wl_registry *registry;
        static inline xdg_wm_base *xdgWmBase;
        static inline zxdg_decoration_manager_v1 *decorationManger;

        struct idToWindowDataIndexes
        {
            uint8_t gen;
            uint8_t windowDataIndex = -1;
            uint8_t resizeEventIndex = -1;
            uint8_t closeEventIndex = -1;
        };
        
        static inline std::vector<idToWindowDataIndexes> idToIndex;  

        struct windowData
        {
            std::string title;
            wl_surface *surface;
            int width, height;
            windowId id;


            xdg_surface *xdgSurface; // to do add child windows support as well as popup support
            xdg_toplevel *xdgToplevel;
            zxdg_toplevel_decoration_v1 *topLevelDecoration;
        };

        static inline std::vector<windowData> windows;

        static inline std::vector<std::function<void()>> closeEventListeners;
        static inline std::vector<windowId> closeEventId;

        static inline std::vector<std::function<void(const windowResizeData&)>> resizeEventListeners;
        static inline std::vector<windowId> resizeEventId;

        
        static void allocateWindow(windowId winId, windowSpec winData);
        static void setCloseEventListener(windowId winId, std::function<void()> callback);
        static void setResizeEventListener(windowId winId, std::function<void(const windowResizeData&)> callback);

        static void deallocateWindow(windowId winId);        
        static void unsetCloseEventListener(windowId winId);
        static void unsetResizeEventListener(windowId winId);

        static void setWindowTitle(windowId id, const std::string& title);
        static std::string getWindowTitle(windowId id);

        static void setWindowHeight(windowId id, int height);
        static int getWindowHeight(windowId id);

        static void setWindowTitle(windowId id, int width);
        static int getWindowWidth(windowId id);

        static wl_surface *getSurface(windowId id)
        {
            if(idToIndex[id.index].windowDataIndex != -1 && id.gen == idToIndex[id.index].gen)
                return windows[idToIndex[id.index].windowDataIndex].surface;
    
            return nullptr;
        }
};
