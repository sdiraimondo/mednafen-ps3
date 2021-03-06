/*  Copyright 2005-2006 Theo Berkau

    This file is part of Yabause.

    Yabause is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Yabause is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Yabause; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifdef MDFNPS3 //ROBO: Only for medanfen

#include <mednafen/mednafen.h>
#define MODULENAMESPACE Yabause
#define SECONDARYINCLUDE
#include <module_helper.h>
using namespace Yabause;

extern "C"
{

#include "error.h"
#include "scsp.h"
#include "sndsdl.h"
#include "debug.h"

#include "sndmdfn.h"

#include <stdlib.h>

static int SNDMDFNInit();
static void SNDMDFNDeInit();
static int SNDMDFNReset();
static int SNDMDFNChangeVideoFormat(int vertfreq);
static void SNDMDFNUpdateAudio(u32 *leftchanbuffer, u32 *rightchanbuffer, u32 num_samples);
static u32 SNDMDFNGetAudioSpace();
static void SNDMDFNMuteAudio();
static void SNDMDFNUnMuteAudio();
static void SNDMDFNSetVolume(int volume);

SoundInterface_struct SNDMDFN = {
SNDCORE_MDFN,
"Mednafen Sound Interface",
SNDMDFNInit,
SNDMDFNDeInit,
SNDMDFNReset,
SNDMDFNChangeVideoFormat,
SNDMDFNUpdateAudio,
SNDMDFNGetAudioSpace,
SNDMDFNMuteAudio,
SNDMDFNUnMuteAudio,
SNDMDFNSetVolume
};

#define NUMSOUNDBLOCKS  4

static uint32_t video_freq;
static uint32_t audio_size;

//////////////////////////////////////////////////////////////////////////////

static int SNDMDFNInit()
{
   SNDMDFNChangeVideoFormat(60);
   return 0;
}

//////////////////////////////////////////////////////////////////////////////

static void SNDMDFNDeInit()
{
}

//////////////////////////////////////////////////////////////////////////////

static int SNDMDFNReset()
{
   return 0;
}

//////////////////////////////////////////////////////////////////////////////

static int SNDMDFNChangeVideoFormat(int vertfreq)
{
   video_freq = vertfreq;
   audio_size = 44100 / vertfreq;
   return 0;
}

//////////////////////////////////////////////////////////////////////////////
static void sdlConvert32uto16s(s32 *srcL, s32 *srcR, s16 *dst, u32 len) {
   u32 i;

   for (i = 0; i < len; i++)
   {
      // Left Channel
      if (*srcL > 0x7FFF) *dst = 0x7FFF;
      else if (*srcL < -0x8000) *dst = -0x8000;
      else *dst = *srcL;
      srcL++;
      dst++;
      // Right Channel
      if (*srcR > 0x7FFF) *dst = 0x7FFF;
      else if (*srcR < -0x8000) *dst = -0x8000;
      else *dst = *srcR;
      srcR++;
      dst++;
   } 
}

static void SNDMDFNUpdateAudio(u32 *leftchanbuffer, u32 *rightchanbuffer, u32 num_samples)
{
	int16_t* buffer = Resampler::Buffer(num_samples * 2);

	if(buffer)
	{
		sdlConvert32uto16s((s32*)leftchanbuffer, (s32*)rightchanbuffer, (s16*)buffer, num_samples);
	}
}

//////////////////////////////////////////////////////////////////////////////

static u32 SNDMDFNGetAudioSpace()
{
   return (Resampler::Written() > 1000) ? 0 : audio_size * 2;
}

//////////////////////////////////////////////////////////////////////////////

static void SNDMDFNMuteAudio()
{
}

//////////////////////////////////////////////////////////////////////////////

static void SNDMDFNUnMuteAudio()
{
}

//////////////////////////////////////////////////////////////////////////////

static void SNDMDFNSetVolume(int volume)
{
}

//////////////////////////////////////////////////////////////////////////////

}

#endif
