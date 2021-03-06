#include <mednafen/mednafen.h>
#include <mednafen/git.h>
#include <mednafen/driver.h>
#include <mednafen/general.h>

#define MODULENAMESPACE vbam
#define SECONDARYINCLUDE
#include <module_helper.h>
using namespace vbam;

#include "Util.h"
#include "common/Port.h"
#include "common/Patch.h"
#include "gba/Flash.h"
#include "gba/RTC.h"
#include "gba/Sound.h"
#include "gba/Cheats.h"
#include "gba/GBA.h"
#include "gba/agbprint.h"
#include "gb/gb.h"
#include "gb/gbGlobals.h"
#include "gb/gbCheats.h"
#include "gb/gbSound.h"
#include "common/SoundDriver.h"

static uint32_t		start_time = 0;
void				InitSystem								()
{
	start_time = MDFND_GetTime();

	//GBA Colors
	utilUpdateSystemColorMaps();

	//DMG Colors
	for(int i = 0; i < 24;)
	{
		systemGbPalette[i++] = (0x1f) | (0x1f << 5) | (0x1f << 10);
		systemGbPalette[i++] = (0x15) | (0x15 << 5) | (0x15 << 10);
		systemGbPalette[i++] = (0x0c) | (0x0c << 5) | (0x0c << 10);
		systemGbPalette[i++] = 0;
	}
}

namespace vbam
{
	extern bool						GBAMode;
	extern EmulateSpecStruct*		ESpec;
	extern uint32_t					SoundFrame;
	extern uint32_t					FrameCount;
	extern uint32_t					SkipHack;
}
using namespace vbam;

class soundy : public SoundDriver
{
	public:
								~soundy			()								{};
	
		bool					init			(long sampleRate)				{};
		void					pause			()								{};
		void					reset			()								{};
		void					resume			()								{};
		void					setThrottle		(unsigned short throttle)		{}
	
		void					write			(u16* finalWave, int length)
		{
			if(ESpec->SoundBufMaxSize > 0 && ESpec->SoundRate > 0)
			{
				memcpy(&ESpec->SoundBuf[SoundFrame * 2], finalWave, length);
				SoundFrame += length / 4;
			}	
		}
};

//ROBO: From vbagx
void rtcEnableWarioRumble(bool e);
void 				sdlApplyPerImagePreferences				()
{
	FILE *f = fopen(MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, MDFN_GetSettingS("vbamgba.vbaover").c_str()).c_str(), "r");
	if(!f)
	{
		return;
	}

	char buffer[7];
	buffer[0] = '[';
	buffer[1] = rom[0xac];
	buffer[2] = rom[0xad];
	buffer[3] = rom[0xae];
	buffer[4] = rom[0xaf];
	buffer[5] = ']';
	buffer[6] = 0;
	
	char readBuffer[2048];
	
	bool found = false;
	
	while(1)
	{
		char *s = fgets(readBuffer, 2048, f);
		
		if(s == NULL)
			break;

		char *p  = strchr(s, ';');

		if(p)
			*p = 0;

		char *token = strtok(s, " \t\n\r=");

		if(!token)
			continue;
		if(strlen(token) == 0)
			continue;

		if(!strcmp(token, buffer))
		{
			found = true;
			break;
		}
	}

	if(found)
	{
		while(1)
		{
			char *s = fgets(readBuffer, 2048, f);
			
			if(s == NULL)
				break;

			char *p = strchr(s, ';');
			if(p)
				*p = 0;

			char *token = strtok(s, " \t\n\r=");
			if(!token)
				continue;
			if(strlen(token) == 0)
				continue;

			if(token[0] == '[') // starting another image settings
				break;
			char *value = strtok(NULL, "\t\n\r=");
			if(value == NULL)
				continue;

			if(!strcmp(token, "rtcEnabled"))
				rtcEnable(atoi(value) == 0 ? false : true);
			else if(!strcmp(token, "flashSize")) {
				int size = atoi(value);
				if(size == 0x10000 || size == 0x20000)
		        	flashSetSize(size);
				} else if(!strcmp(token, "saveType")) {
					int save = atoi(value);
					if(save >= 0 && save <= 5)
						cpuSaveType = save;
			} else if(!strcmp(token, "mirroringEnabled")) {
				mirroringEnable = (atoi(value) == 0 ? false : true);
			}
		}
	}
	fclose(f);

//ROBO: Copy from VBAGX
	// In most cases this is already handled in GameSettings, but just to make sure:
	switch (rom[0xac])
	{
		case 'F': // Classic NES
			cpuSaveType = 1; // EEPROM
			mirroringEnable = 1;
			break;
		case 'K': // Accelerometers
			cpuSaveType = 4; // EEPROM + sensor
			break;
		case 'R': // WarioWare Twisted style sensors
		case 'V': // Drill Dozer
			rtcEnableWarioRumble(true);
			break;
		case 'U': // Boktai solar sensor and clock
			rtcEnable(true);
			break;
	}
//ROBO: End copy
}


struct EmulatedSystem emulator = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, false, 0}; 


u16					systemColorMap16[0x10000];
u32					systemColorMap32[0x10000];
u16					systemGbPalette[24];
int					systemRedShift = 19;
int					systemGreenShift = 11;
int					systemBlueShift = 3;
int					systemColorDepth = 32;
int					systemDebug = 0;
int					systemVerbose = 0;
int					systemFrameSkip = 0;
int					systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
u32					RomIdCode = 0;
int					emulating = 0;

void				debugOutputStub							(const char*, u32)						{}
void				(*dbgOutput)							(const char *s, u32 addr) = debugOutputStub;



bool				systemPauseOnFrame						()										{return true;}
void				systemGbPrint							(u8 *,int,int,int,int)					{}
void				systemScreenCapture						(int)									{}
void				systemShowSpeed							(int)									{}
void				systemGbBorderOn						()										{}
void				systemFrame								()										{}
void				system10Frames							(int)									{}
void				systemMessage							(int, const char *, ...)				{}


//Timing
u32					systemGetClock							()										{return MDFND_GetTime() - start_time;}

//Input
bool				systemReadJoypads						()										{return true;}
u32					systemReadJoypad						(int)									{return Input::GetPort<0, 2>();}
bool rtcWarioRumbleEnabled;
#ifndef MLDLL
void				systemCartridgeRumble					(bool aRumble)							{MDFND_Rumble(aRumble ? 1 : 0, aRumble ? 128 : 0);}
#else
void				systemCartridgeRumble					(bool aRumble)							{}
#endif
void				systemUpdateMotionSensor				()										{}
int					systemGetSensorX						()										{return 0;}
int					systemGetSensorY						()										{return 0;}
int					systemGetSensorZ						()										{return 0;}
u8					systemGetSensorDarkness					()										{return 0;}

//Visual
void				systemDrawScreen						()
{
	Video::BlitRGB(ESpec, (uint32_t*)pix, GBAMode ? 240 : 256, GBAMode ? 160 : 224, GBAMode ? 241 : 257);
}

//Sound
SoundDriver*		systemSoundInit							()
{
	soundShutdown();
	return new soundy();
}

void				systemOnWriteDataToSoundBuffer			(const u16 * finalWave, int length)		{}
void				systemOnSoundShutdown					()										{}
bool				systemCanChangeSoundQuality				()										{return true;}

