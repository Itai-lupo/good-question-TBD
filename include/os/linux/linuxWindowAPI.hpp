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

#include <Tracy.hpp>

#include "osEventsData.hpp"

class linuxWindowAPI
{
	// todo: move to a cpu side rendering moudle
	private:
        static inline wl_shm *shm;
		static void randname(char *buf)
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            long r = ts.tv_nsec;
            for (int i = 0; i < 6; ++i) {
                buf[i] = 'A'+(r&15)+(r&16)*2;
                r >>= 5;
            }
        }

        static int create_shm_file(void)
        {
            int retries = 100;
            do {
                char name[] = "/wl_shm-XXXXXX";
                randname(name + sizeof(name) - 7);
                --retries;
                int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
                if (fd >= 0) {
                    shm_unlink(name);
                    return fd;
                }
            } while (retries > 0 && errno == EEXIST);
            return -1;
        }

        static int allocate_shm_file(size_t size)
        {
            int fd = create_shm_file();
            if (fd < 0)
                return -1;
            int ret;
            do {
                ret = ftruncate(fd, size);
            } while (ret < 0 && errno == EINTR);
            if (ret < 0) {
                close(fd);
                return -1;
            }
            return fd;
        }

        static struct wl_buffer *draw_frame(int width, int height, uint32_t offset = 0)
        {
            
            ZoneScoped;

            int stride = width * 4;
            int size = stride * height;
            offset %= 64;

            int fd = allocate_shm_file(size);
            if (fd == -1) {
                return NULL;
            }

            uint32_t *data = (uint32_t *)mmap(NULL, size,
                    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (data == MAP_FAILED) {
                close(fd);
                return NULL;
            }

            wl_shm_pool *pool = wl_shm_create_pool(shm, fd, size);
            wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0,
                    width, height, stride, WL_SHM_FORMAT_ARGB8888);
            wl_shm_pool_destroy(pool);
            close(fd);
            uint32_t * ptr = data;
            
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    if ((x + offset + y / 32 * 32) % 64 < 32)
                        data[y * width + x] = 0xffEEEEEE;
                    else
                        data[y * width + x] = 0xFF111111;
                }
            }


            munmap(data, size);
            wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
            return buffer;
        }        
        
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
            LOG_INFO(width << ", " << height)
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
        

        struct windowInfo
        {
            wl_surface *surface;
            xdg_surface *xdgSurface;
            xdg_toplevel *xdgToplevel;
            zxdg_toplevel_decoration_v1 *topLevelDecoration;
            
            std::string title;
            int width, height;
            uint32_t lastFrame = 0;
            float offset = 8;

            std::function<void(const keyData&)> keyPressEventListenrs;
            std::function<void(const keyData&)> keyReleasedEventListenrs;
            std::function<void(const keyData&)> keyRepeatEventListenrs;
            std::function<void(const KeyTypedData&)> keyTypedEventListenrs;

            std::function<void(const mouseButtonData&)> mouseButtonPressEventListenrs;
            std::function<void(const mouseButtonData&)> mouseButtonReleasedEventListenrs;

            std::function<void(const mouseMoveData&)> mouseMovedListenrs;
            std::function<void(const mouseScrollData&)> mouseScrollListenrs;
        
            windowId id;
        };


        static inline std::vector<windowInfo> windowsInfo;

        


        static inline std::map<uint32_t, std::pair<uint64_t, uint32_t>> idToIndex; 

        static int64_t getIndexFromId(windowId id)
        {
            if(idToIndex.find(id.index) != idToIndex.end() && id.gen == idToIndex[id.index].second)
                return id.index;
            LOG_ERROR("no matching window id with index: " << id.index << " and gen: " << id.gen)
            return -1;
        }

        static void windowEventListener();
        static inline std::thread *eventListenr;

    public:
        static void init();
        static void closeApi();

        static windowId createWindow(const windowSpec& windowToCreate);
        static void closeWindow(windowId winId);
        static bool isWindowOpen(windowId winId);

        static wl_display *getDisplay(){ return display; }


        // ################ set event listener ################################################################
        static void setKeyPressEventListenrs(windowId winId, std::function<void(const keyData&)> callback);
        static void setKeyReleasedEventListenrs(windowId winId, std::function<void(const keyData&)> callback);
        static void setKeyRepeatEventListenrs(windowId winId, std::function<void(const keyData&)> callback);
        static void setKeyTypedEventListenrs(windowId winId, std::function<void(const KeyTypedData&)> callback);

        static void setMouseButtonPressEventListenrs(windowId winId, std::function<void(const mouseButtonData&)> callback);
        static void setMouseButtonReleasedEventListenrs(windowId winId, std::function<void(const mouseButtonData&)> callback);
        
        static void setMouseMovedListenrs(windowId winId, std::function<void(const mouseMoveData&)> callback);
        static void setMouseScrollListenrs(windowId winId, std::function<void(const mouseScrollData&)> callback);


        // ################ unset event listener ################################################################
        static void unsetKeyPressEventListenrs(windowId winId);
        static void unsetKeyReleasedEventListenrs(windowId winId);
        static void unsetKeyRepeatEventListenrs(windowId winId);
        static void unsetKeyTypedEventListenrs(windowId winId);

        static void unsetMouseButtonPressEventListenrs(windowId winId);
        static void unsetMouseButtonReleasedEventListenrs(windowId winId);
        
        static void unsetMouseMovedListenrs(windowId winId);
        static void unsetMouseScrollListenrs(windowId winId);




};