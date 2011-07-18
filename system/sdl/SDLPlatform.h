#pragma once

#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef __WIN32__
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#include <SDL/SDL.h>
#ifndef __WIN32__
#include <SDL/SDL_opengl.h>
#endif
#include <SDL/SDL_net.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include "SDLInputConfig.h"

#ifdef __WIN32__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#endif

class				PlatformHelpers
{
	public:
		static uint32_t					GetTicks					()
		{
			return SDL_GetTicks();
		}

		static void						Sleep						(uint32_t aMilliseconds)
		{
			SDL_Delay(aMilliseconds);
		}
};


