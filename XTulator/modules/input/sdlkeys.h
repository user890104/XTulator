#ifndef _SDLKEYS_H_
#define _SDLKEYS_H_

#include <stdint.h>
#ifdef _WIN32
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

const uint32_t sdlconsole_translateMatrix[95][2] = {
	{ SDLK_ESCAPE, 0x01 },
	{ SDLK_1, 0x02 },
	{ SDLK_2, 0x03 },
	{ SDLK_3, 0x04 },
	{ SDLK_4, 0x05 },
	{ SDLK_5, 0x06 },
	{ SDLK_6, 0x07 },
	{ SDLK_7, 0x08 },
	{ SDLK_8, 0x09 },
	{ SDLK_9, 0x0A },
	{ SDLK_0, 0x0B },
	{ SDLK_MINUS, 0x0C },
	{ SDLK_EQUALS, 0x0D },
	{ SDLK_BACKSPACE, 0x0E },
	{ SDLK_TAB, 0x0F },
	{ SDLK_q, 0x10 },
	{ SDLK_w, 0x11 },
	{ SDLK_e, 0x12 },
	{ SDLK_r, 0x13 },
	{ SDLK_t, 0x14 },
	{ SDLK_y, 0x15 },
	{ SDLK_u, 0x16 },
	{ SDLK_i, 0x17 },
	{ SDLK_o, 0x18 },
	{ SDLK_p, 0x19 },
	{ SDLK_LEFTBRACKET, 0x1A },
	{ SDLK_RIGHTBRACKET, 0x1B },
	{ SDLK_RETURN, 0x1C },
	{ SDLK_RETURN2, 0x1C },
	{ SDLK_KP_ENTER, 0x1C },
	{ SDLK_LCTRL, 0x1D },
	{ SDLK_a, 0x1E },
	{ SDLK_s, 0x1F },
	{ SDLK_d, 0x20 },
	{ SDLK_f, 0x21 },
	{ SDLK_g, 0x22 },
	{ SDLK_h, 0x23 },
	{ SDLK_j, 0x24 },
	{ SDLK_k, 0x25 },
	{ SDLK_l, 0x26 },
	{ SDLK_SEMICOLON, 0x27 },
	{ SDLK_QUOTE, 0x28 },
	{ SDLK_BACKQUOTE, 0x29 },
	{ SDLK_LSHIFT, 0x2A },
	{ SDLK_BACKSLASH, 0x2B },
	{ SDLK_z, 0x2C },
	{ SDLK_x, 0x2D },
	{ SDLK_c, 0x2E },
	{ SDLK_v, 0x2F },
	{ SDLK_b, 0x30 },
	{ SDLK_n, 0x31 },
	{ SDLK_m, 0x32 },
	{ SDLK_COMMA, 0x33 },
	{ SDLK_PERIOD, 0x34 },
	{ SDLK_SLASH, 0x35 },
	{ SDLK_RSHIFT, 0x36 },
	{ SDLK_KP_MULTIPLY, 0x37 },
	{ SDLK_LALT, 0x38 },
	{ SDLK_SPACE, 0x39 },
	{ SDLK_CAPSLOCK, 0x3A },
	{ SDLK_F1, 0x3B },
	{ SDLK_F2, 0x3C },
	{ SDLK_F3, 0x3D },
	{ SDLK_F4, 0x3E },
	{ SDLK_F5, 0x3F },
	{ SDLK_F6, 0x40 },
	{ SDLK_F7, 0x41 },
	{ SDLK_F8, 0x42 },
	{ SDLK_F9, 0x43 },
	{ SDLK_F10, 0x44 },
	{ SDLK_NUMLOCKCLEAR, 0x45 },
	{ SDLK_SCROLLLOCK, 0x46 },
	{ SDLK_KP_7, 0x47 },
	{ SDLK_HOME, 0x47 },
	{ SDLK_KP_8, 0x48 },
	{ SDLK_UP, 0x48 },
	{ SDLK_KP_9, 0x49 },
	{ SDLK_PAGEUP, 0x49 },
	{ SDLK_KP_MINUS, 0x4A },
	{ SDLK_KP_4, 0x4B },
	{ SDLK_LEFT, 0x4B },
	{ SDLK_KP_5, 0x4C },
	{ SDLK_KP_6, 0x4D },
	{ SDLK_RIGHT, 0x4D },
	{ SDLK_KP_PLUS, 0x4E },
	{ SDLK_KP_1, 0x4F },
	{ SDLK_END, 0x4F },
	{ SDLK_KP_2, 0x50 },
	{ SDLK_DOWN, 0x50 },
	{ SDLK_KP_3, 0x51 },
	{ SDLK_PAGEDOWN, 0x51 },
	{ SDLK_KP_0, 0x52 },
	{ SDLK_INSERT, 0x52 },
	{ SDLK_KP_PERIOD, 0x53 },
	{ SDLK_DELETE, 0x53 }
};

#endif
