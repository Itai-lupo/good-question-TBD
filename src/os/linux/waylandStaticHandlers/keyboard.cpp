#ifdef __linux__
#include "keyboard.hpp"
#include "log.hpp"
#include "window.hpp"


#include <sys/prctl.h>
#include <sys/mman.h>
#include <assert.h>
#include <Tracy.hpp>
#include <thread>

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
    for (int i = 0; i < window::windows.size(); i++)
    {
        if(window::windows[i].surface == surface)
        {
            activeWindow = window::windows[i].id;
        }
    }

    uint32_t index = idToIndex[activeWindow.index].gainFocusEventIndex;
    if( index != 255 && idToIndex[activeWindow.index].gen == activeWindow.gen)         
            std::thread(gainFocusEventListeners[index]).detach();

    char buf1[128];  
    char buf2[128];  
    uint32_t key;
    for (uint32_t i = 0; i < keys->size/sizeof(uint32_t); i++) 
    {
        key = ((uint32_t*)keys->data)[i];
        xkb_keysym_t sym = xkb_state_key_get_one_sym(xkbState, key + 8);
        xkb_keysym_get_name(sym, buf1, sizeof(buf1));
        xkb_state_key_get_utf8(xkbState, key + 8, buf2, sizeof(buf2));

        
        isKeyPressed[keycodeFromScaneCode[key]] = true;
        std::thread(keyboard::keyListener, key).detach();
        
        uint32_t index = idToIndex[activeWindow.index].pressEventIndex;
        if( index != -1 && idToIndex[activeWindow.index].gen == activeWindow.gen)  
            std::thread(keyPressEventListeners[index], keyData{buf2, keycodeFromScaneCode[key], key}).detach();
    }
}

void keyboard::wlLeave(void *data, wl_keyboard *wl_keyboard, uint32_t serial, wl_surface *surface)
{
    uint32_t index = idToIndex[activeWindow.index].lostFocusEventIndex;
    if( index != 255 && idToIndex[activeWindow.index].gen == activeWindow.gen)             
            std::thread(lostFocusEventListeners[index]).detach();

    isKeyPressed.clear();

    activeWindow.gen = -1;
    activeWindow.index = -1;
}

void keyboard::wlKey(void *data, wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    char buf1[128];  
    char buf2[128];

    xkb_keysym_t sym = xkb_state_key_get_one_sym(xkbState, key + 8);
    xkb_keysym_get_name(sym, buf1, sizeof(buf1));
    xkb_state_key_get_utf8(xkbState, key + 8, buf2, sizeof(buf2));

    if(state == isKeyPressed[keycodeFromScaneCode[key]])
        return;

    isKeyPressed[keycodeFromScaneCode[key]] = state;
    
    if(state)
        std::thread(keyboard::keyListener, key).detach();
    
    if(idToIndex[activeWindow.index].gen != activeWindow.gen)
        return;

    uint32_t index = idToIndex[activeWindow.index].pressEventIndex;
    if(index != (uint32_t)-1 && state == WL_KEYBOARD_KEY_STATE_PRESSED)
        std::thread(keyPressEventListeners[index], keyData{buf2,  keycodeFromScaneCode[key], key}).detach();


    index = idToIndex[activeWindow.index].releaseEventIndex;
    if(index != (uint32_t)-1 && state == WL_KEYBOARD_KEY_STATE_PRESSED)
        std::thread(keyReleasedEventListeners[index], keyData{buf2,  keycodeFromScaneCode[key], key}).detach();
}

void keyboard::wlModifiers(void *data, wl_keyboard *wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
    
    xkb_state_update_mask(xkbState,
        mods_depressed, mods_latched, mods_locked, 0, 0, group);

}

void keyboard::wlRepeatInfo(void *data, wl_keyboard *wl_keyboard, int32_t rate, int32_t delay)
{
    keyRepeatRate = rate;
    keyRepeatdelay = delay;
}


void keyboard::keyListener(uint32_t key)
{
    ZoneScoped;
    uint32_t index = idToIndex[activeWindow.index].repeatEventIndex;
    if(index == (uint32_t)-1 || idToIndex[activeWindow.index].gen != activeWindow.gen)
        return;

    char buf1[128];
    char buf2[6];
    uint32_t keycode = key + 8;
    
    xkb_keysym_t sym = xkb_state_key_get_one_sym(xkbState, keycode);
    xkb_keysym_get_name(sym, buf1, sizeof(buf1));
    
    std::string thradNameA = "key listener " + std::string(buf1);
    prctl(PR_SET_NAME, thradNameA.c_str());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(keyRepeatdelay));
    while (index != -1 && idToIndex[activeWindow.index].gen == activeWindow.gen && isKeyPressed[keycodeFromScaneCode[key]])
    {
        xkb_state_key_get_utf8(xkbState, key + 8, buf2, sizeof(buf2));
        std::thread(keyRepeatEventListeners[index], keyData{buf2, keycodeFromScaneCode[key], key}).detach();

        std::this_thread::sleep_for(std::chrono::milliseconds(keyRepeatRate));
        
        uint32_t index = idToIndex[activeWindow.index].repeatEventIndex;
    }    
}

void keyboard::allocateWindowEvents(windowId winId)
{
    if(winId.index >= idToIndex.size())
        idToIndex.resize(winId.index + 1);

    idToIndex[winId.index].gen = winId.gen;
}

void keyboard::setKeyPressEventListeners(windowId winId, const std::function<void(const keyData&)> callback)
{    
    uint32_t index = idToIndex[winId.index].pressEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;
    

    if(index != (uint8_t)-1)
    {    
        keyPressEventListeners[index] = callback;
        keyPressEventId[index] = winId;

        return;
    }
    
    idToIndex[winId.index].pressEventIndex = keyPressEventListeners.size();
    keyPressEventListeners.push_back(callback);
    keyPressEventId.push_back(winId);
}

void keyboard::setKeyReleasedEventListeners(windowId winId, const std::function<void(const keyData&)> callback)
{
    uint32_t index = idToIndex[winId.index].releaseEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        keyReleasedEventListeners[index] = callback;
        keyReleasedEventId[index] = winId;

        return;
    }
    
    idToIndex[winId.index].releaseEventIndex = keyReleasedEventListeners.size();
    keyReleasedEventListeners.push_back(callback);
    keyReleasedEventId.push_back(winId);

}

void keyboard::setKeyRepeatEventListeners(windowId winId, const std::function<void(const keyData&)> callback)
{
    uint32_t index = idToIndex[winId.index].repeatEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        keyRepeatEventListeners[index] = callback;
        keyRepeatEventId[index] = winId;
        return;
    }
    
    idToIndex[winId.index].repeatEventIndex = keyRepeatEventListeners.size();
    keyRepeatEventListeners.push_back(callback);
    keyRepeatEventId.push_back(winId);
}

void keyboard::setGainFocusEventListeners(windowId winId, std::function<void()> callback)
{
    uint32_t index = idToIndex[winId.index].gainFocusEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        gainFocusEventListeners[index] = callback;
        gainFocusEventId[index] = winId;
        return;
    }

    idToIndex[winId.index].gainFocusEventIndex = gainFocusEventListeners.size();
    gainFocusEventListeners.push_back(callback);
    gainFocusEventId.push_back(winId);
}

void keyboard::setLostFocusEventListeners(windowId winId, std::function<void()> callback)
{
    uint32_t index = idToIndex[winId.index].lostFocusEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        lostFocusEventListeners[index] = callback;
        lostFocusEventId[index] = winId;
        return;
    }

    idToIndex[winId.index].lostFocusEventIndex = lostFocusEventListeners.size();
    lostFocusEventListeners.push_back(callback);
    lostFocusEventId.push_back(winId);
}

void keyboard::deallocateWindowEvents(windowId winId)
{
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    unsetKeyPressEventListeners(winId);
    unsetKeyReleasedEventListeners(winId);
    unsetKeyRepeatEventListeners(winId);
    unsetGainFocusEventListeners(winId);
    unsetLostFocusEventListeners(winId);

    idToIndex[winId.index].gen = -1;

}

void keyboard::unsetKeyPressEventListeners(windowId winId)
{
    uint32_t index = idToIndex[winId.index].pressEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = keyPressEventListeners.size() - 1;
    idToIndex[keyPressEventId[lastIndex].index].pressEventIndex = index;
    keyPressEventListeners[index] = keyPressEventListeners[lastIndex];
    keyPressEventId[index] = keyPressEventId[lastIndex];

    keyPressEventListeners.pop_back();
    keyPressEventId.pop_back();

    idToIndex[winId.index].pressEventIndex = -1;
}

void keyboard::unsetKeyReleasedEventListeners(windowId winId)
{
    
    uint32_t index = idToIndex[winId.index].releaseEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = keyReleasedEventListeners.size() - 1;
    idToIndex[keyReleasedEventId[lastIndex].index].releaseEventIndex = index;
    keyReleasedEventListeners[index] = keyReleasedEventListeners[lastIndex];
    keyReleasedEventId[index] = keyReleasedEventId[lastIndex];

    keyReleasedEventListeners.pop_back();
    keyReleasedEventId.pop_back();

    idToIndex[winId.index].releaseEventIndex = -1;

}

void keyboard::unsetKeyRepeatEventListeners(windowId winId)
{
    uint32_t index = idToIndex[winId.index].repeatEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = keyRepeatEventListeners.size() - 1;
    idToIndex[keyRepeatEventId[lastIndex].index].repeatEventIndex = index;
    keyRepeatEventListeners[index] = keyRepeatEventListeners[lastIndex];
    keyRepeatEventId[index] = keyRepeatEventId[lastIndex];

    keyRepeatEventListeners.pop_back();
    keyRepeatEventId.pop_back();

    idToIndex[winId.index].repeatEventIndex = -1;
}

void keyboard::unsetGainFocusEventListeners(windowId winId)
{
    uint32_t index = idToIndex[winId.index].gainFocusEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = gainFocusEventListeners.size() - 1;
    idToIndex[gainFocusEventId[lastIndex].index].repeatEventIndex = index;
    gainFocusEventListeners[index] = gainFocusEventListeners[lastIndex];
    gainFocusEventId[index] = gainFocusEventId[lastIndex];

    gainFocusEventListeners.pop_back();
    gainFocusEventId.pop_back();

    idToIndex[winId.index].gainFocusEventIndex = -1;
}

void keyboard::unsetLostFocusEventListeners(windowId winId)
{
    uint32_t index = idToIndex[winId.index].lostFocusEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = lostFocusEventListeners.size() - 1;
    idToIndex[lostFocusEventId[lastIndex].index].repeatEventIndex = index;
    lostFocusEventListeners[index] = lostFocusEventListeners[lastIndex];
    lostFocusEventId[index] = lostFocusEventId[lastIndex];

    lostFocusEventListeners.pop_back();
    lostFocusEventId.pop_back();

    idToIndex[winId.index].lostFocusEventIndex = -1;
}

#endif