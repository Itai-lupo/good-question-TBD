#pragma once
#include "osAPI.hpp"
#include "log.hpp"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <time.h>


#include <wayland-client.h>
#include <xdg-decoration-client-protocol.h>
#include <xdg-shell-client-protocol.h>
#include <xkbcommon/xkbcommon.h>

#include <unistd.h>
#include <string.h>
#include <map>
#include <thread>
#include <functional>
#include <list>
#include <shared_mutex>
#include <condition_variable>

#include <Tracy.hpp>

#include "osEventsData.hpp"

class linuxWindowAPI
{
	private:
        

        static void wlSurfaceFrameDone(void *data, wl_callback *cb, uint32_t time);
        static constexpr wl_callback_listener wlSurfaceFrameListener = 
        {
            .done = wlSurfaceFrameDone
        };
        
        static void xdgTopLevelConfigure(void *data, xdg_toplevel *xdgToplevel, int32_t width, int32_t height, wl_array *states);
        static void xdgTopLevelClose(void *data, xdg_toplevel *xdgToplevel);
        static void xdgTopLevelConfigureBounds(void *data, xdg_toplevel *xdgToplevel, int32_t width, int32_t height)
        {

        }

        static constexpr xdg_toplevel_listener xdgTopLevelListener = {
            .configure = xdgTopLevelConfigure,
            .close = xdgTopLevelClose,
            .configure_bounds = xdgTopLevelConfigureBounds
        };

        static void xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial);
        static constexpr struct xdg_wm_base_listener xdg_wm_base_listener = {
            .ping = xdg_wm_base_ping,
        };

		static void global_registry_handler(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
		static void global_registry_remover(void *data, struct wl_registry *registry, uint32_t id);
		static constexpr wl_registry_listener registry_listener{
            global_registry_handler,
            global_registry_remover
        };

        
		static void wl_buffer_release(void *data, struct wl_buffer *wl_buffer);
        static constexpr struct wl_buffer_listener wl_buffer_listener = {
            .release = wl_buffer_release,
        };

		static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial);
		static constexpr struct xdg_surface_listener xdg_surface_listener = {
            .configure = xdg_surface_configure,
        };	

    public:
        static inline wl_display *display = NULL;
        static inline wl_compositor *compositor = NULL;
        static inline wl_registry *registry;
        static inline xdg_wm_base *xdgWmBase;
        static inline wl_shm *shm;
        
        static inline uint32_t smallestWindowId = 0;

        struct windowInfo
        {
            xdg_toplevel *xdgToplevel;
            zxdg_toplevel_decoration_v1 *topLevelDecoration;
            std::string title;
            xdg_surface *xdgSurface; // to do add child windows support as well as popup support
            wl_surface *surface;
            int width, height;
            
            //to do add sub-surface system
            


            //to do move into a mouse system

            //to do move into a window events system
            std::function<void()> closeListeners;
            std::function<void(const windowResizeData&)> resizeListeners;
            std::function<void(const windowRenderData&)> renderListeners;
        
            windowId id;

            //to do move into a cpu rendering system
            int fd;
            wl_shm_pool *pool;
            uint32_t memoryPoolSize;
            uint8_t bufferInRender;
            uint8_t bufferToRender;
            uint8_t freeBuffer;
            uint32_t *buffer;
            int bufferSize;

            std::thread *renderThread;

            std::shared_ptr<std::shared_mutex> renderMutex{};
            std::shared_ptr<std::condition_variable_any> renderFinshed{};
            bool renderFinshedBool;


            windowInfo(): bufferInRender(0), bufferToRender(1), freeBuffer(2), 
                renderMutex(std::make_shared<std::shared_mutex>()), 
                renderFinshed(std::make_shared<std::condition_variable_any>())
            {}
        };


        static inline std::array<windowInfo, 254> windowsInfo;
        static inline int windowsInfoSize = 0;


        struct idIndexes
        {
            uint8_t gen;
            uint8_t index;
            uint8_t renderIndex;
        };
        

        static inline std::array<idIndexes, 31> idToIndex;  
        static inline std::list<uint32_t> freeSlots;
        static inline int hightestId = 0;

        static int64_t getIndexFromId(windowId id)
        {
            if(id.gen == idToIndex[id.index].gen)
                return idToIndex[id.index].index;
            LOG_ERROR("no matching window id with index: " << id.index << " and gen: " << id.gen)
            return -1;
        }


        static int64_t getRenderIndexFromId(windowId id)
        {
            if(id.gen == idToIndex[id.index].gen)
                return idToIndex[id.index].renderIndex;
            LOG_ERROR("no matching window id with index: " << id.index << " and gen: " << id.gen)
            return -1;
        }

        static void windowEventListener();
        static inline std::thread *eventListenr;
        
        static void renderWindow(windowId win); 
        static void swapWindowBuffers(windowId win); 

        static void allocateWindowCpuPool(windowInfo& info);
        static void reallocateWindowCpuPool(windowInfo& info);
        static wl_buffer *allocateWindowBuffer(const windowInfo& info, uint32_t offset);
        static uint32_t *mapWindowCpuBuffer(windowInfo& info, uint32_t offset);

		static void randname(char *buf);
        static int create_shm_file(void);
        static int allocate_shm_file(size_t size);


    public:
        static void init();
        static void closeApi();

        static windowId createWindow(const windowSpec& windowToCreate);
        static void closeWindow(windowId winId);
        static bool isWindowOpen(windowId winId);

        static wl_display *getDisplay(){ return display; }


        static std::string getWindowTitle(windowId winId);
        static std::pair<uint32_t, uint32_t> getWindowSize(windowId winId);


        // ################ set event listener ################################################################
        static void setCloseEventeListeners(windowId winId, std::function<void()> callback);
        static void setResizeEventeListeners(windowId winId, std::function<void(const windowResizeData&)> callback);
        static void setRenderEventListeners(windowId winId, std::function<void(const windowRenderData&)> callback);
        
        // ################ unset event listener ################################################################
        static void unsetCloseEventeListeners(windowId winId);
        static void unsetResizeEventeListeners(windowId winId);
        static void unsetRenderEventListeners(windowId winId);
};