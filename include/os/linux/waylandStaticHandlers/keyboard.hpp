#pragma once
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>
#include <map>
#include <thread>

class keyboard
{
    public:
        static inline wl_keyboard *keyboardHandle = NULL;
        static inline xkb_context *xkbContext;
        static inline xkb_state *xkbState = NULL;
        static inline xkb_keymap *xkbKeymap = NULL;
        
        static inline int32_t keyRepeatRate; 
        static inline int32_t keyRepeatdelay;
        
        static inline std::map<uint32_t, std::thread*> keyRepertListeners;
        static inline std::map<uint32_t, bool> keyRepertListenersShouldRun;

        
        static void wlKeymap(void *data, wl_keyboard *wl_keyboard, uint32_t format, int32_t fd, uint32_t size);
        static void wlEnter(void *data, wl_keyboard *wl_keyboard, uint32_t serial, wl_surface *surface, wl_array *keys);
        static void wlLeave(void *data, wl_keyboard *wl_keyboard, uint32_t serial, wl_surface *surface);
        static void wlKey(void *data, wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
        static void wlModifiers(void *data, wl_keyboard *wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
        static void wlRepeatInfo(void *data, wl_keyboard *wl_keyboard, int32_t rate, int32_t delay);
        static void keyListener(uint32_t key);

        static constexpr wl_keyboard_listener wlKeyboardListener = {
            .keymap = wlKeymap,
            .enter = wlEnter,
            .leave = wlLeave,
            .key = wlKey,
            .modifiers = wlModifiers,
            .repeat_info = wlRepeatInfo,
        };



};