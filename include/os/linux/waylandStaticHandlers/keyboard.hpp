#pragma once
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>
#include <linux/input.h>

#include <map>
#include <thread>
#include <functional>

#include "keycodes.hpp"
#include "core.hpp"
#include "keyData.hpp"
#include "surface.hpp"

class keyboard
{
    public:
        static inline wl_keyboard *keyboardHandle = NULL;
        static inline xkb_context *xkbContext;
        static inline xkb_state *xkbState = NULL;
        static inline xkb_keymap *xkbKeymap = NULL;
        
        static inline int32_t keyRepeatRate; 
        static inline int32_t keyRepeatdelay;
        
        static inline surfaceId activeWindow{(uint16_t)-1, (uint16_t)-1}; 
        static inline std::map<keycodes, bool> isKeyPressed;
        
        static void wlKeymap(void *data, wl_keyboard *wl_keyboard, uint32_t format, int32_t fd, uint32_t size);
        static void wlEnter(void *data, wl_keyboard *wl_keyboard, uint32_t serial, wl_surface *surface, wl_array *keys);
        static void wlLeave(void *data, wl_keyboard *wl_keyboard, uint32_t serial, wl_surface *surface);
        static void wlKey(void *data, wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
        static void wlModifiers(void *data, wl_keyboard *wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
        static void wlRepeatInfo(void *data, wl_keyboard *wl_keyboard, int32_t rate, int32_t delay);
        static void keyListener(uint32_t key);


        static inline std::vector<std::function<void(const keyData&)>> keyPressEventListeners;
        static inline std::vector<surfaceId> keyPressEventId;

        static inline std::vector<std::function<void(const keyData&)>> keyReleasedEventListeners;
        static inline std::vector<surfaceId> keyReleasedEventId;
        
        static inline std::vector<std::function<void(const keyData&)>> keyRepeatEventListeners;
        static inline std::vector<surfaceId> keyRepeatEventId;

        static inline std::vector<std::function<void()>> gainFocusEventListeners;
        static inline std::vector<surfaceId> gainFocusEventId;

        static inline std::vector<std::function<void()>> lostFocusEventListeners;
        static inline std::vector<surfaceId> lostFocusEventId;

        struct idTpKeyEventIndexes
        {
            uint8_t gen = -1;
            uint8_t pressEventIndex = -1;
            uint8_t releaseEventIndex = -1;
            uint8_t repeatEventIndex = -1;
            uint8_t gainFocusEventIndex = -1;
            uint8_t lostFocusEventIndex = -1;
        };
        
        static inline std::vector<idTpKeyEventIndexes> idToIndex;  
        
        static constexpr wl_keyboard_listener wlKeyboardListener = {
            .keymap = wlKeymap,
            .enter = wlEnter,
            .leave = wlLeave,
            .key = wlKey,
            .modifiers = wlModifiers,
            .repeat_info = wlRepeatInfo,
        };

        static inline constexpr keycodes keycodeFromScaneCode[] = {
                [KEY_GRAVE]  =   keycodes::keyGraveAccent,
                [KEY_1]  =   keycodes::key1,
                [KEY_2]  =   keycodes::key2,
                [KEY_3]  =   keycodes::key3,
                [KEY_4]  =   keycodes::key4,
                [KEY_5]  =   keycodes::key5,
                [KEY_6]  =   keycodes::key6,
                [KEY_7]  =   keycodes::key7,
                [KEY_8]  =   keycodes::key8,
                [KEY_9]  =   keycodes::key9,
                [KEY_0]  =   keycodes::key0,

                [KEY_SPACE]  =   keycodes::keySpace,
                [KEY_MINUS]  =   keycodes::keyMinus,
                [KEY_EQUAL]  =   keycodes::keyEqual,
                
                [KEY_Q]  =   keycodes::keyQ,
                [KEY_W]  =   keycodes::keyW,
                [KEY_E]  =   keycodes::keyE,
                [KEY_R]  =   keycodes::keyR,
                [KEY_T]  =   keycodes::keyT,
                [KEY_Y]  =   keycodes::keyY,
                [KEY_U]  =   keycodes::keyU,
                [KEY_I]  =   keycodes::keyI,
                [KEY_O]  =   keycodes::keyO,
                [KEY_P]  =   keycodes::keyP,

                [KEY_LEFTBRACE]  =   keycodes::keyLeftBracket,
                [KEY_RIGHTBRACE] =   keycodes::keyRightBracket,
                
                [KEY_A]  =   keycodes::keyA,
                [KEY_S]  =   keycodes::keyS,
                [KEY_D]  =   keycodes::keyD,
                [KEY_F]  =   keycodes::keyF,
                [KEY_G]  =   keycodes::keyG,
                [KEY_H]  =   keycodes::keyH,
                [KEY_J]  =   keycodes::keyJ,
                [KEY_K]  =   keycodes::keyK,
                [KEY_L]  =   keycodes::keyL,
                
                [KEY_SEMICOLON]  =   keycodes::keySemicolon,
                [KEY_APOSTROPHE] =   keycodes::keyApostrophe,
                
                [KEY_Z]  =   keycodes::keyZ,
                [KEY_X]  =   keycodes::keyX,
                [KEY_C]  =   keycodes::keyC,
                [KEY_V]  =   keycodes::keyV,
                [KEY_B]  =   keycodes::keyB,
                [KEY_N]  =   keycodes::keyN,
                [KEY_M]  =   keycodes::keyM,
                [KEY_COMMA]  =   keycodes::keyComma,
                [KEY_DOT]    =   keycodes::keyPeriod,
                [KEY_SLASH]  =   keycodes::keySlash,
                [KEY_BACKSLASH]  =   keycodes::keyBackslash,
                [KEY_ESC]    =   keycodes::keyEscape,
                [KEY_TAB]    =   keycodes::keyTab,
                [KEY_LEFTSHIFT]  =   keycodes::keyLeftShift,
                [KEY_RIGHTSHIFT] =   keycodes::keyRightShift,
                [KEY_LEFTCTRL]   =   keycodes::keyLeftControl,
                [KEY_RIGHTCTRL]  =   keycodes::keyRightControl,
                [KEY_LEFTALT]    =   keycodes::keyLeftAlt,
                [KEY_RIGHTALT]   =   keycodes::keyRightAlt,
                [KEY_LEFTMETA]   =   keycodes::keyLeftSuper,
                [KEY_RIGHTMETA]  =   keycodes::keyRightShift,
                [KEY_COMPOSE]    =   keycodes::keyMenu,
                [KEY_NUMLOCK]    =   keycodes::keyNumLock,
                [KEY_CAPSLOCK]   =   keycodes::keyCapsLock,
                [KEY_PRINT]  =   keycodes::keyPrintScreen,
                [KEY_SCROLLLOCK] =   keycodes::keyScrollLock,
                [KEY_PAUSE]  =   keycodes::keyPause,
                [KEY_DELETE] =   keycodes::keyDelete,
                [KEY_BACKSPACE]  =   keycodes::keyBackspace,
                [KEY_ENTER]  =   keycodes::keyEnter,
                [KEY_HOME]   =   keycodes::keyHome,
                [KEY_END]    =   keycodes::keyEnd,
                [KEY_PAGEUP] =   keycodes::keyPageUp,
                [KEY_PAGEDOWN]   =   keycodes::keyPageDown,
                [KEY_INSERT] =   keycodes::keyInsert,
                [KEY_LEFT]   =   keycodes::keyLeft,
                [KEY_RIGHT]  =   keycodes::keyRight,
                [KEY_DOWN]   =   keycodes::keyDown,
                [KEY_UP] =   keycodes::keyUp,
                [KEY_F1] =   keycodes::keyF1,
                [KEY_F2] =   keycodes::keyF2,
                [KEY_F3] =   keycodes::keyF3,
                [KEY_F4] =   keycodes::keyF4,
                [KEY_F5] =   keycodes::keyF5,
                [KEY_F6] =   keycodes::keyF6,
                [KEY_F7] =   keycodes::keyF7,
                [KEY_F8] =   keycodes::keyF8,
                [KEY_F9] =   keycodes::keyF9,
                [KEY_F10]    =   keycodes::keyF10,
                [KEY_F11]    =   keycodes::keyF11,
                [KEY_F12]    =   keycodes::keyF12,
                [KEY_F13]    =   keycodes::keyF13,
                [KEY_F14]    =   keycodes::keyF14,
                [KEY_F15]    =   keycodes::keyF15,
                [KEY_F16]    =   keycodes::keyF16,
                [KEY_F17]    =   keycodes::keyF17,
                [KEY_F18]    =   keycodes::keyF18,
                [KEY_F19]    =   keycodes::keyF19,
                [KEY_F20]    =   keycodes::keyF20,
                [KEY_F21]    =   keycodes::keyF21,
                [KEY_F22]    =   keycodes::keyF22,
                [KEY_F23]    =   keycodes::keyF23,
                [KEY_F24]    =   keycodes::keyF24,
                [KEY_KPSLASH]    =   keycodes::keyKpDivide,
                [KEY_KPASTERISK] =   keycodes::keyKpMultiply,
                [KEY_KPMINUS]    =   keycodes::keyKpSubtract,
                [KEY_KPPLUS] =   keycodes::keyKpAdd,
                [KEY_KP0]    =   keycodes::keyKp0,
                [KEY_KP1]    =   keycodes::keyKp1,
                [KEY_KP2]    =   keycodes::keyKp2,
                [KEY_KP3]    =   keycodes::keyKp3,
                [KEY_KP4]    =   keycodes::keyKp4,
                [KEY_KP5]    =   keycodes::keyKp5,
                [KEY_KP6]    =   keycodes::keyKp6,
                [KEY_KP7]    =   keycodes::keyKp7,
                [KEY_KP8]    =   keycodes::keyKp8,
                [KEY_KP9]    =   keycodes::keyKp9,
                [KEY_KPDOT]  =   keycodes::keyKpDecimal,
                [KEY_KPEQUAL]    =   keycodes::keyKpEqual,
                [KEY_KPENTER]    =   keycodes::keyKpEnter,
                [KEY_102ND]  =   keycodes::keyWorld2,
        };


        static void allocateWindowEvents(surfaceId surfaceId);
        static void setKeyPressEventListeners(surfaceId surfaceId, std::function<void(const keyData&)> callback);
        static void setKeyReleasedEventListeners(surfaceId surfaceId, std::function<void(const keyData&)> callback);
        static void setKeyRepeatEventListeners(surfaceId surfaceId, std::function<void(const keyData&)> callback);
        static void setGainFocusEventListeners(surfaceId surfaceId, std::function<void()> callback);
        static void setLostFocusEventListeners(surfaceId surfaceId, std::function<void()> callback);
        

        static void deallocateWindowEvents(surfaceId surfaceId);
        static void unsetKeyPressEventListeners(surfaceId surfaceId);
        static void unsetKeyReleasedEventListeners(surfaceId surfaceId);
        static void unsetKeyRepeatEventListeners(surfaceId surfaceId);
        static void unsetGainFocusEventListeners(surfaceId surfaceId);
        static void unsetLostFocusEventListeners(surfaceId surfaceId);
        
};