#pragma once
#include <wayland-client.h>

class seat
{
    public:
        static inline wl_seat *seatHandle;
        static void wl_seat_capabilities (void *data, wl_seat *seat, uint32_t capabilities);
        static void wl_seat_name(void *data, struct wl_seat *seat, const char *name);
        static constexpr struct wl_seat_listener wl_seat_listener = {
            .capabilities = wl_seat_capabilities,
            .name = wl_seat_name,
        };

};