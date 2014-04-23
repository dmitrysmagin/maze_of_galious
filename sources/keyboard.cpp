#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

#include "SDL.h"

bool IsAltPressed()
{
	SDLMod modifiers;

	SDL_PumpEvents();
	modifiers=SDL_GetModState();
   
	return ((modifiers & KMOD_ALT) != 0);
} /* IsAltPressed */ 


bool IsAltPressed2()
{
#ifdef _WIN32
    return (GetKeyState (VK_MENU) & 0x8000) ? true : false;
#else
	SDL_PumpEvents();
    return ( (SDL_GetModState() & KMOD_ALT) != 0);
#endif
}

