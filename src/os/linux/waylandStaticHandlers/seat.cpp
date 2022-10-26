#ifdef __linux__

#include "seat.hpp"
#include "keyboard.hpp"
#include "pointer.hpp"
#include "log.hpp"

#include <Tracy.hpp>


void seat::wl_seat_capabilities (void *data, wl_seat *seat, uint32_t capabilities)
{
    ZoneScoped;
    if(capabilities & wl_seat_capability::WL_SEAT_CAPABILITY_KEYBOARD &&  keyboard::keyboardHandle == NULL)
    {
        keyboard::keyboardHandle = wl_seat_get_keyboard(seatHandle);
        wl_keyboard_add_listener(keyboard::keyboardHandle, &keyboard::wlKeyboardListener, data);
    }
    else if (!(capabilities & wl_seat_capability::WL_SEAT_CAPABILITY_KEYBOARD) && keyboard::keyboardHandle != NULL)
    {
        wl_keyboard_release(keyboard::keyboardHandle);
        keyboard::keyboardHandle = NULL;
    }
    

    if(capabilities & wl_seat_capability::WL_SEAT_CAPABILITY_POINTER && pointer::pointerHandler == NULL)
    {
        pointer::pointerHandler = wl_seat_get_pointer(seatHandle);
        wl_pointer_add_listener(pointer::pointerHandler, &pointer::wlPointerListener, data);
    }
    else if(!(capabilities & wl_seat_capability::WL_SEAT_CAPABILITY_POINTER) && pointer::pointerHandler != NULL)
    {
        wl_pointer_release(pointer::pointerHandler);
        pointer::pointerHandler = NULL;
    }

    if(capabilities & wl_seat_capability::WL_SEAT_CAPABILITY_TOUCH)
    {

    }
}

void seat::wl_seat_name(void *data, struct wl_seat *seat, const char *name)
{
    ZoneScoped;
}

#endif