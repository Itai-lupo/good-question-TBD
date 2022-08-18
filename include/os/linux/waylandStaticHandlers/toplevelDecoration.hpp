#pragma once
#include <wayland-client.h>
#include <xdg-decoration-client-protocol.h>

class toplevelDecoration
{
    public:
        static inline zxdg_decoration_manager_v1 *decorationManger;
        
        static void toplevelDecorationConfigure(void *data, zxdg_toplevel_decoration_v1 *zxdg_toplevelDecoration, uint32_t mode){}
        static constexpr zxdg_toplevel_decoration_v1_listener toplevelDecorationListener = {
            .configure = toplevelDecorationConfigure
        };

};
