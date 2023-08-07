#include "scancode_keycode_map_x11.h"
#include <linux/input-event-codes.h>

#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>

struct scancode_keycode_map_entry{
    int scancode;
    int keycode;
};


static struct scancode_keycode_map_entry scancode_keycode_map[]={
    {KEY_ESC,XK_Escape},
    {KEY_1,XK_1},
    {KEY_2,XK_2},
    {KEY_3,XK_3},
    {KEY_4,XK_4},
    {KEY_5,XK_5},
    {KEY_6,XK_6},
    {KEY_7,XK_7},
    {KEY_8,XK_8},
    {KEY_9,XK_9},
    {KEY_0,XK_0},
    {KEY_MINUS,XK_minus},
    {KEY_EQUAL,XK_equal},
    {KEY_BACKSPACE,XK_BackSpace},
    {KEY_TAB,XK_Tab},
    {KEY_Q,XK_Q},
    {KEY_W,XK_W},
    {KEY_E,XK_E},
    {KEY_R,XK_R},
    {KEY_T,XK_T},
    {KEY_Y,XK_Y},
    {KEY_U,XK_U},
    {KEY_I,XK_I},
    {KEY_O,XK_O},
    {KEY_P,XK_P},
    {KEY_LEFTBRACE,XK_bracketleft},
    {KEY_RIGHTBRACE,XK_bracketright},
    {KEY_ENTER,XK_Return},
    {KEY_LEFTCTRL,XK_Control_L},
    {KEY_A,XK_A},
    {KEY_S,XK_S},
    {KEY_D,XK_D},
    {KEY_F,XK_F},
    {KEY_G,XK_G},
    {KEY_H,XK_H},
    {KEY_J,XK_J},
    {KEY_K,XK_K},
    {KEY_L,XK_L},
    {KEY_SEMICOLON,XK_semicolon},
    {KEY_APOSTROPHE,XK_apostrophe},
    {KEY_GRAVE,XK_grave},
    {KEY_LEFTSHIFT,XK_Shift_L},
    {KEY_BACKSLASH,XK_backslash},
    {KEY_Z,XK_Z},
    {KEY_X,XK_X},
    {KEY_C,XK_C},
    {KEY_V,XK_V},
    {KEY_B,XK_B},
    {KEY_N,XK_N},
    {KEY_M,XK_M},
    {KEY_COMMA,XK_comma},
    {KEY_DOT,XK_greater},
    {KEY_SLASH,XK_slash},
    {KEY_RIGHTSHIFT,XK_Shift_R},
    {KEY_KPASTERISK,XK_KP_Multiply},
    {KEY_LEFTALT,XK_Alt_L},
    {KEY_SPACE,XK_space},
    {KEY_CAPSLOCK,XK_Caps_Lock},
    {KEY_F1,XK_F1},
    {KEY_F2,XK_F2},
    {KEY_F3,XK_F3},
    {KEY_F4,XK_F4},
    {KEY_F5,XK_F5},
    {KEY_F6,XK_F6},
    {KEY_F7,XK_F7},
    {KEY_F8,XK_F8},
    {KEY_F9,XK_F9},
    {KEY_F10,XK_F10},
    {KEY_NUMLOCK,XK_Num_Lock},
    {KEY_SCROLLLOCK,XK_Scroll_Lock},
    {KEY_KP7,XK_Home},
    {KEY_KP8,XK_Up},
    {KEY_KP9,XK_Page_Up},
    {KEY_KPMINUS,XK_KP_Subtract},
    {KEY_KP4,XK_Left},
    {KEY_KP5,XK_KP_5},
    {KEY_KP6,XK_Right},
    {KEY_KPPLUS,XK_KP_Add},
    {KEY_KP1,XK_End},
    {KEY_KP2,XK_Down},
    {KEY_KP3,XK_Page_Down},
    {KEY_KP0,XK_Insert},
    {KEY_KPDOT,XK_Delete},
    {KEY_F11,XK_F11},
    {KEY_F12,XK_F12},
    {KEY_KPENTER,XK_KP_Enter},
    {KEY_RIGHTCTRL,XK_Control_R},
    {KEY_KPSLASH,XK_KP_Divide},
    {KEY_RIGHTALT,XK_Alt_R},
    {KEY_HOME,XK_Home},
    {KEY_UP,XK_Up},
    {KEY_PAGEUP,XK_Page_Up},
    {KEY_LEFT,XK_Left},
    {KEY_RIGHT,XK_Right},
    {KEY_END,XK_End},
    {KEY_DOWN,XK_Down},
    {KEY_PAGEDOWN,XK_Page_Down},
    {KEY_INSERT,XK_Insert},
    {KEY_DELETE,XK_Delete},
    {KEY_PAUSE,XK_Pause},
    {KEY_F13,XK_F13},
    {KEY_F14,XK_F14},
    {KEY_F15,XK_F15},
    {KEY_F16,XK_F16},
    {KEY_F17,XK_F17},
    {KEY_F18,XK_F18},
    {KEY_F19,XK_F19},
    {KEY_F20,XK_F20},
    {KEY_F21,XK_F21},
    {KEY_F22,XK_F22},
    {KEY_F23,XK_F23},
    {KEY_F24,XK_F24},
};

int scancode_to_keycode(int scancode)
{
    int size = sizeof(scancode_keycode_map)/sizeof(scancode_keycode_map[0]);
    for(int i = 0; i < size; i++)
    {
        if(scancode_keycode_map[i].scancode == scancode)
            return scancode_keycode_map[i].keycode;
    }
    return XK_Escape;
}

