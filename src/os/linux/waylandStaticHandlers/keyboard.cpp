#ifdef __linux__
#include "keyboard.hpp"
#include "log.hpp"
#include "surface.hpp"


#include <sys/prctl.h>
#include <sys/mman.h>
#include <assert.h>
#include <Tracy.hpp>
#include <thread>


#include <fcntl.h>

void keyboard::init(entityPool *surfacesPool)
{
    keyPressEventListeners = new keyCallbackComponent(surfacesPool);
    keyReleasedEventListeners = new keyCallbackComponent(surfacesPool);
    keyRepeatEventListeners = new keyCallbackComponent(surfacesPool);
    gainFocusEventListeners = new windowSurfaceCallbackComponent(surfacesPool);
    lostFocusEventListeners = new windowSurfaceCallbackComponent(surfacesPool);
}

void keyboard::closeKeyboard()
{
    delete keyPressEventListeners;
    delete keyReleasedEventListeners;
    delete keyRepeatEventListeners;
    delete gainFocusEventListeners;
    delete lostFocusEventListeners;
}

void keyboard::wlKeymap(void *data, wl_keyboard *wl_keyboard, uint32_t format, int32_t fd, uint32_t size)
{
    ZoneScoped;

    assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

        
    char filePath[255];
    
    char *map_shm = (char *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
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
    ZoneScoped;
    for (int i = 0; i < surface::surfacesInfo->getData().size(); i++)
    {
        if(surface::surfacesInfo->getData()[i].surface == surface)
        {
            activeWindow = surface::surfacesInfo->getData()[i].id;
        }
    }

    surfaceCallback temp = gainFocusEventListeners->getCallback(activeWindow);
    if(temp)         
        std::thread(temp, activeWindow).detach();

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
        
        keyCallback temp = keyPressEventListeners->getCallback(activeWindow);
        if(temp)  
            std::thread(temp, keyData{activeWindow, buf2, keycodeFromScaneCode[key], key}).detach();
    }
}

void keyboard::wlLeave(void *data, wl_keyboard *wl_keyboard, uint32_t serial, wl_surface *surface)
{
    ZoneScoped;
    surfaceCallback temp = lostFocusEventListeners->getCallback(activeWindow);
    if(temp)             
        std::thread(temp, activeWindow).detach();

    isKeyPressed.clear();

    activeWindow.gen = -1;
    activeWindow.index = -1;
}

void keyboard::wlKey(void *data, wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    ZoneScoped;
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
    

    keyCallback temp = keyPressEventListeners->getCallback(activeWindow);
    if(temp && state == WL_KEYBOARD_KEY_STATE_PRESSED)
        std::thread(temp, keyData{activeWindow, buf2,  keycodeFromScaneCode[key], key}).detach();


    temp = keyReleasedEventListeners->getCallback(activeWindow);
    if(temp && state == WL_KEYBOARD_KEY_STATE_PRESSED)
        std::thread(temp, keyData{activeWindow, buf2,  keycodeFromScaneCode[key], key}).detach();
}

void keyboard::wlModifiers(void *data, wl_keyboard *wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
    
    ZoneScoped;
    xkb_state_update_mask(xkbState,
        mods_depressed, mods_latched, mods_locked, 0, 0, group);

}

void keyboard::wlRepeatInfo(void *data, wl_keyboard *wl_keyboard, int32_t rate, int32_t delay)
{
    ZoneScoped;
    keyRepeatRate = rate;
    keyRepeatdelay = delay;
}


void keyboard::keyListener(uint32_t key)
{
    ZoneScoped;
    keyCallback temp = keyRepeatEventListeners->getCallback(activeWindow);
    if(!temp)
        return;

    char buf1[128];
    char buf2[6];
    uint32_t keycode = key + 8;
    
    xkb_keysym_t sym = xkb_state_key_get_one_sym(xkbState, keycode);
    xkb_keysym_get_name(sym, buf1, sizeof(buf1));
    
    std::string thradNameA = "key listener " + std::string(buf1);
    prctl(PR_SET_NAME, thradNameA.c_str());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(keyRepeatdelay));
    while (isKeyPressed[keycodeFromScaneCode[key]] && temp)
    {
        xkb_state_key_get_utf8(xkbState, key + 8, buf2, sizeof(buf2));
        std::thread(temp, keyData{activeWindow, buf2, keycodeFromScaneCode[key], key}).detach();

        std::this_thread::sleep_for(std::chrono::milliseconds(keyRepeatRate));
        temp = keyRepeatEventListeners->getCallback(activeWindow);
    }    
}


void keyboard::setKeyPressEventListeners(surfaceId winId, keyCallback callback)
{   
    keyPressEventListeners->setCallback(winId, callback);
}

void keyboard::setKeyReleasedEventListeners(surfaceId winId, keyCallback callback)
{
    keyReleasedEventListeners->setCallback(winId, callback);
}

void keyboard::setKeyRepeatEventListeners(surfaceId winId, keyCallback callback)
{
    keyRepeatEventListeners->setCallback(winId, callback);
}

void keyboard::setGainFocusEventListeners(surfaceId winId, surfaceCallback callback)
{
    gainFocusEventListeners->setCallback(winId, callback);
}

void keyboard::setLostFocusEventListeners(surfaceId winId, surfaceCallback callback)
{
    lostFocusEventListeners->setCallback(winId, callback);
}

void keyboard::unsetKeyPressEventListeners(surfaceId winId)
{
    keyPressEventListeners->deleteComponent(winId);
}

void keyboard::unsetKeyReleasedEventListeners(surfaceId winId)
{
    keyReleasedEventListeners->deleteComponent(winId);
}

void keyboard::unsetKeyRepeatEventListeners(surfaceId winId)
{
    keyRepeatEventListeners->deleteComponent(winId);
}

void keyboard::unsetGainFocusEventListeners(surfaceId winId)
{
    gainFocusEventListeners->deleteComponent(winId);
}

void keyboard::unsetLostFocusEventListeners(surfaceId winId)
{
    lostFocusEventListeners->deleteComponent(winId);
}

#endif