/*
  XTulator: A portable, open-source 80186 PC emulator.
  Copyright (C)2020 Mike Chambers

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "../../config.h"

#include <SDL.h>
#ifdef _WIN32
#include <SDL_syswm.h>
#include <Windows.h>
#endif
#include <stdio.h>
#include <stdint.h>
#include "sdlconsole.h"
#include "../input/sdlkeys.h"
#include "../input/mouse.h"
#include "../../timing.h"
#include "../../menus.h"
#include "../../debuglog.h"
#if defined(__APPLE__) && !defined(ROCKBOX)
#include <dispatch/dispatch.h>
#include <pthread.h>
#endif

SDL_Surface *sdlconsole_screen = NULL;

uint64_t sdlconsole_frameTime[30];
uint32_t sdlconsole_keyTimer;
uint8_t sdlconsole_curkey, sdlconsole_lastKey, sdlconsole_frameIdx = 0, sdlconsole_grabbed = 0, sdlconsole_ctrl = 0, sdlconsole_alt = 0, sdlconsole_doRepeat = 0;
int sdlconsole_curw, sdlconsole_curh;

char* sdlconsole_title;

void sdlconsole_keyRepeat(void* dummy) {
	sdlconsole_doRepeat = 1;
	timing_updateIntervalFreq(sdlconsole_keyTimer, 15);
}

int sdlconsole_init(char *title) {
#ifdef _WIN32
	HWND hwnd;
	SDL_SysWMinfo wmInfo;
#endif

	if (SDL_Init(SDL_INIT_VIDEO)) return -1;

	sdlconsole_title = title;
	sdlconsole_curw = 640;
	sdlconsole_curh = 400;

	sdlconsole_screen = SDL_SetVideoMode(sdlconsole_curw, sdlconsole_curh, 32, SDL_SWSURFACE);
	if (!sdlconsole_screen) return -1;

	SDL_WM_SetCaption(sdlconsole_title, NULL);

	sdlconsole_keyTimer = timing_addTimer(sdlconsole_keyRepeat, NULL, 2, TIMING_DISABLED);

#ifdef _WIN32
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(sdlconsole_window, &wmInfo);
	hwnd = wmInfo.info.win.window;
	menus_init(hwnd);
#endif

	return 0;
}

int sdlconsole_setWindow(int w, int h) {
	if (sdlconsole_screen) SDL_FreeSurface(sdlconsole_screen);
	sdlconsole_screen = NULL;

#if defined(__APPLE__) && !defined(ROCKBOX)
	if (pthread_main_np()) {
		sdlconsole_screen = SDL_SetVideoMode(w, h, 32, SDL_SWSURFACE);
	}
	else {
		__block SDL_Surface *result = NULL;
		dispatch_sync(dispatch_get_main_queue(), ^{
			result = SDL_SetVideoMode(w, h, 32, SDL_SWSURFACE);
		});
		sdlconsole_screen = result;
	}
#else
	sdlconsole_screen = SDL_SetVideoMode(w, h, 32, SDL_SWSURFACE);
#endif

	if (!sdlconsole_screen) return -1;

	sdlconsole_curw = w;
	sdlconsole_curh = h;

	return 0;
}

void sdlconsole_setTitle(char* title) { //appends something to the main title, doesn't replace it all
	char tmp[1024];
	sprintf(tmp, "%s - %s", sdlconsole_title, title);

#if defined(__APPLE__) && !defined(ROCKBOX)
	char *tmp_copy = strdup(tmp);
	dispatch_async(dispatch_get_main_queue(), ^{
		SDL_WM_SetCaption(tmp_copy, NULL);
		free(tmp_copy);
	});
#else
	SDL_WM_SetCaption(tmp, NULL);
#endif
}

void sdlconsole_blit(uint32_t *pixels, int w, int h, int stride) {
	static uint64_t lasttime = 0;
	uint64_t curtime;
	curtime = timing_getCur();

	if ((w != sdlconsole_curw) || (h != sdlconsole_curh)) {
		if (sdlconsole_setWindow(w, h)) {
			debug_log(DEBUG_ERROR, "failed setWindow %ux%u\r\n", w, h);
		}
	}

	if (SDL_MUSTLOCK(sdlconsole_screen)) SDL_LockSurface(sdlconsole_screen);

	uint8_t *dst = (uint8_t *)sdlconsole_screen->pixels;
	uint8_t *src = (uint8_t *)pixels;

	if (sdlconsole_screen->pitch == stride) {
		memcpy(dst, src, sdlconsole_screen->pitch * sdlconsole_screen->h);
	}
	else {
		for (int y = 0; y < sdlconsole_screen->h; y++) {
			memcpy(dst + sdlconsole_screen->pitch * y, src + stride * y, sdlconsole_screen->pitch);
		}
	}

	if (SDL_MUSTLOCK(sdlconsole_screen)) SDL_UnlockSurface(sdlconsole_screen);

	SDL_Flip(sdlconsole_screen);

	if (lasttime != 0) {
		int i, avgcount;
		uint64_t curavg;
		char tmp[64];
		sdlconsole_frameTime[sdlconsole_frameIdx++] = curtime - lasttime;
		if (sdlconsole_frameIdx == 30) {
			sdlconsole_frameIdx = 0;
			avgcount = 0;
			curavg = 0;
			for (i = 0; i < 30; i++) {
				if (sdlconsole_frameTime[i] != 0) {
					curavg += sdlconsole_frameTime[i];
					avgcount++;
				}
			}
			curavg /= avgcount;
			sprintf(tmp, "%.2f FPS", (double)((timing_getFreq() * 10) / curavg) / 10);
			sdlconsole_setTitle(tmp);
		}
	}
	lasttime = curtime;
}

void sdlconsole_mousegrab() {
	sdlconsole_ctrl = sdlconsole_alt = 0;
	if (sdlconsole_grabbed) {
		SDL_WM_GrabInput(SDL_GRAB_OFF);
		SDL_ShowCursor(1);
		sdlconsole_grabbed = 0;
	} else {
		SDL_WM_GrabInput(SDL_GRAB_ON);
		SDL_ShowCursor(0);
		sdlconsole_grabbed = 1;
	}
}

int sdlconsole_loop() {
	SDL_Event event;
	int8_t xrel, yrel;
	uint8_t action = 0;

	if (sdlconsole_doRepeat) {
		sdlconsole_doRepeat = 0;
		sdlconsole_curkey = sdlconsole_lastKey;
		return SDLCONSOLE_EVENT_KEY;
	}

	if (!SDL_PollEvent(&event)) return SDLCONSOLE_EVENT_NONE;
	switch (event.type) {
		case SDL_KEYDOWN:
#ifdef DEBUG_VGA
			if (event.key.keysym.sym == SDLK_F12) {
				vga_dumpregs();
			}
#endif
			switch (event.key.keysym.sym) {
			case SDLK_F11:
				return SDLCONSOLE_EVENT_DEBUG_1;
			case SDLK_F12:
				return SDLCONSOLE_EVENT_DEBUG_2;
			default:
				if (event.key.keysym.sym == SDLK_LCTRL) sdlconsole_ctrl = 1;
				if (event.key.keysym.sym == SDLK_LALT) sdlconsole_alt = 1;
				if (sdlconsole_ctrl & sdlconsole_alt) {
					sdlconsole_mousegrab();
				}
				sdlconsole_curkey = sdlconsole_translateScancode(event.key.keysym.sym);
				if (sdlconsole_curkey == 0x00) {
					return SDLCONSOLE_EVENT_NONE;
				} else {
					sdlconsole_lastKey = sdlconsole_curkey;
					timing_updateIntervalFreq(sdlconsole_keyTimer, 2);
					timing_timerEnable(sdlconsole_keyTimer);
					return SDLCONSOLE_EVENT_KEY;
				}
			}
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LCTRL) sdlconsole_ctrl = 0;
			if (event.key.keysym.sym == SDLK_LALT) sdlconsole_alt = 0;
			sdlconsole_curkey = sdlconsole_translateScancode(event.key.keysym.sym) | 0x80;
			if ((sdlconsole_curkey & 0x7F) == sdlconsole_lastKey) {
				timing_timerDisable(sdlconsole_keyTimer);
			}
			return (sdlconsole_curkey == 0x80) ? SDLCONSOLE_EVENT_NONE : SDLCONSOLE_EVENT_KEY;
		case SDL_MOUSEMOTION:
			xrel = (event.motion.xrel < -128) ? -128 : (int8_t)event.motion.xrel;
			xrel = (event.motion.xrel > 127) ? 127 : (int8_t)event.motion.xrel;
			yrel = (event.motion.yrel < -128) ? -128 : (int8_t)event.motion.yrel;
			yrel = (event.motion.yrel > 127) ? 127 : (int8_t)event.motion.yrel;
			if (sdlconsole_grabbed) {
				mouse_action(MOUSE_ACTION_MOVE, MOUSE_NEITHER, xrel, yrel);
			}
			return SDLCONSOLE_EVENT_NONE;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT) {
				action = MOUSE_ACTION_LEFT;
				if (!sdlconsole_grabbed) {
					sdlconsole_mousegrab();
					break;
				}
			}
			else if (event.button.button == SDL_BUTTON_RIGHT) {
				action = MOUSE_ACTION_RIGHT;
			}
			if (sdlconsole_grabbed) {
				mouse_action(action, (event.button.state == SDL_PRESSED) ? MOUSE_PRESSED : MOUSE_UNPRESSED, 0, 0);
			}
			return SDLCONSOLE_EVENT_NONE;
		case SDL_QUIT:
			return SDLCONSOLE_EVENT_QUIT;
	}
	return SDLCONSOLE_EVENT_NONE;
}

uint8_t sdlconsole_getScancode() {
	//debug_log(DEBUG_DETAIL, "curkey: %02X\r\n", sdlconsole_curkey);
	return sdlconsole_curkey;
}

uint8_t sdlconsole_translateScancode(SDLKey keyval) {
	uint8_t i;
	for (i = 0; i < 95; i++) {
		if (keyval == (SDLKey)sdlconsole_translateMatrix[i][0]) {
			return (uint8_t)sdlconsole_translateMatrix[i][1];
		}
	}
	return 0x00;
}
