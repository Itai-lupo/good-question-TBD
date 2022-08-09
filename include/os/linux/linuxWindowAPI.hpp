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
#include <xdg-shell-client-protocol.h>
#include <xdg-decoration-client-protocol.h>

#include <unistd.h>
#include <string.h>
#include <map>

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

        static struct wl_buffer *draw_frame(int width, int height)
        {
            int stride = width * 4;
            int size = stride * height;

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

            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    if ((x + y / 32 * 32) % 64 < 32)
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
        static void toplevelDecorationConfigure(void *data, zxdg_toplevel_decoration_v1 *zxdg_toplevelDecoration, uint32_t mode);

        static constexpr zxdg_toplevel_decoration_v1_listener toplevelDecorationListener = {
            .configure = toplevelDecorationConfigure
        };

        static void wl_seat_capabilities (void *data, wl_seat *seat, uint32_t serial);
        static void wl_seat_name(void *data, struct wl_seat *seat, const char *name);
        static constexpr struct wl_seat_listener wl_seat_listener = {
            .capabilities = wl_seat_capabilities,
            .name = wl_seat_name,
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
	
    private:
        wl_compositor *compositor = NULL;
        wl_registry *registry;
        wl_seat *seat;
        xdg_wm_base *xdgWmBase;
        zxdg_decoration_manager_v1 * decorationManger;
        wl_display *display = NULL;
        
        struct windowInfo
        {
            wl_surface *surface;
            xdg_surface *xdgSurface;
            xdg_toplevel *xdgToplevel;
            zxdg_toplevel_decoration_v1 *topLevelDecoration;
            
            std::string title;
            int width, height;
            windowId id;
        };

        std::vector<windowInfo> windowsInfo;
        std::map<uint32_t, std::pair<uint64_t, uint32_t>> idToIndex; 

        int64_t getIndexFromId(windowId id)
        {
            if(idToIndex.find(id.index) != idToIndex.end() && id.gen == idToIndex[id.index].second)
                return id.index;
            LOG_ERROR("no matching window id with index: " << id.index << " and gen: " << id.gen)
            return -1;
        }

    public:
        linuxWindowAPI();
        ~linuxWindowAPI();
        windowId createWindow(const windowSpec& windowToCreate);
        bool isWindowOpen(windowId winId);
        void closeWindow(windowId winId);

        wl_display *getDisplay(){ return display; }

};