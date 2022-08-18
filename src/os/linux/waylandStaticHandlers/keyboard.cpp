#ifdef __linux__
#include "keyboard.hpp"
#include "log.hpp"
#include "linuxWindowAPI.hpp"

#include <sys/prctl.h>
#include <sys/mman.h>
#include <assert.h>

void keyboard::wlKeymap(void *data, wl_keyboard *wl_keyboard, uint32_t format, int32_t fd, uint32_t size)
{
    assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

        
    char *map_shm = (char *)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    assert(map_shm != MAP_FAILED);
    
    xkb_state *xkbStateTemp = NULL;
    xkb_keymap *xkbKeymapTemp = NULL;
        
    xkbKeymap = xkb_keymap_new_from_string(xkbContext, map_shm, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
    munmap(map_shm, size);
    close(fd);

    xkbStateTemp = xkb_state_new(xkbKeymap);
    xkb_keymap_unref(xkbKeymap);
    xkb_state_unref(xkbState);
    xkbKeymap = xkbKeymapTemp;
    xkbState = xkbStateTemp;
}

void keyboard::wlEnter(void *data, wl_keyboard *wl_keyboard, uint32_t serial, wl_surface *surface, wl_array *keys)
{
    for (size_t i = 0; i < linuxWindowAPI::windowsInfo.size(); i++)
    {
        if(linuxWindowAPI::windowsInfo[i].surface == surface)
        {
            LOG_INFO("key enter on '" << linuxWindowAPI::windowsInfo[i].title << "' keys pressed are:")
        }
    }
    

    char buf1[128];  
    char buf2[128];  
    uint32_t key;
    for (uint32_t i = 0; i < keys->size/sizeof(uint32_t); i++) 
    {
        key = ((uint32_t*)keys->data)[i];
        xkb_keysym_t sym = xkb_state_key_get_one_sym(xkbState, key + 8);
        xkb_keysym_get_name(sym, buf1, sizeof(buf1));
        xkb_state_key_get_utf8(xkbState, key + 8, buf2, sizeof(buf2));

        LOG_INFO(i << ": sym = " << buf1 << " (" <<  sym << "), key = " << key << ", utf8 = '" << buf2 << "'");
        
        keyRepertListenersShouldRun[key] = true;
        keyRepertListeners[key] = new std::thread(keyboard::keyListener, key);
    }

}

void keyboard::wlLeave(void *data, wl_keyboard *wl_keyboard, uint32_t serial, wl_surface *surface)
{
    for(auto& [key, t]: keyRepertListeners)
    {
        if(!keyRepertListenersShouldRun[key])
            continue;
        keyRepertListenersShouldRun[key] = false;
        t->detach();
        delete t;

        char buf[128];
        uint32_t keycode = key + 8;
        xkb_state_key_get_utf8(xkbState, keycode, buf, sizeof(buf));
        LOG_INFO(buf << ", " << 0)

    }
}

void keyboard::wlKey(void *data, wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    char buf1[128];  
    char buf2[128];

    xkb_keysym_t sym = xkb_state_key_get_one_sym(xkbState, key + 8);
    xkb_keysym_get_name(sym, buf1, sizeof(buf1));
    xkb_state_key_get_utf8(xkbState, key + 8, buf2, sizeof(buf2));

    LOG_INFO("key event: sym = " << buf1 << " (" <<  sym << "), key = " << key << ", utf8 = '" << buf2 << "', state = " << state);
    
    if(state == keyRepertListenersShouldRun[key])
        return;
    keyRepertListenersShouldRun[key] = state;
    if(state)
        keyRepertListeners[key] = new std::thread(keyboard::keyListener, key);
    else
    {
        keyRepertListeners[key]->detach();
        delete keyRepertListeners[key];
    }


}

void keyboard::wlModifiers(void *data, wl_keyboard *wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
    
    xkb_state_update_mask(xkbState,
        mods_depressed, mods_latched, mods_locked, 0, 0, group);

}

void keyboard::wlRepeatInfo(void *data, wl_keyboard *wl_keyboard, int32_t rate, int32_t delay)
{
    LOG_INFO("wlRepeatInfo: " << rate << ", " << delay)
    keyRepeatRate = rate;
    keyRepeatdelay = delay;
}


void keyboard::keyListener(uint32_t key)
{

    char buf1[128];
    char buf2[128];
    uint32_t keycode = key + 8;
    xkb_keysym_t sym = xkb_state_key_get_one_sym(xkbState, keycode);
    xkb_keysym_get_name(sym, buf1, sizeof(buf1));
    std::string thradNameA = "key listener " + std::string(buf1);
    prctl(PR_SET_NAME, thradNameA.c_str());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(keyRepeatdelay));
    while (keyRepertListenersShouldRun[key])
    {
        xkb_keysym_t sym = xkb_state_key_get_one_sym(xkbState, key + 8);
        xkb_keysym_get_name(sym, buf1, sizeof(buf1));
        xkb_state_key_get_utf8(xkbState, key + 8, buf2, sizeof(buf2));

        LOG_INFO("key repeat event: sym = " << buf1 << " (" <<  sym << "), key = " << key << ", utf8 = '" << buf2 << "', state = " << 1);

        if(keyRepertListenersShouldRun[key])
            std::this_thread::sleep_for(std::chrono::milliseconds(keyRepeatRate));
    }
    
}


#endif