#ifndef _MOG_PATH_H_
#define _MOG_PATH_H_

#ifdef _WIN32
#define SLASH "\\"
#else
#define SLASH "/"
#endif

#ifdef RENDER_320x240
#define GRAPHICS "graphics_lowres"
#else
#define GRAPHICS "graphics"
#endif

#define GRAPHICS_ORIGINAL  GRAPHICS SLASH "original" SLASH
#define SOUND_ORIGINAL     "sound" SLASH "original" SLASH

#endif /* _MOG_PATH_H_ */
