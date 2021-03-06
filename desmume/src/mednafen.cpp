//Includes from mednafen
#include <mednafen/mednafen.h>
#include <mednafen/cdrom/cdromif.h>
#include <mednafen/mednafen-driver.h>
#include <mednafen/git.h>
#include <mednafen/general.h>
#include <mednafen/md5.h>
#include <mednafen/driver.h>

#include <stdarg.h>
#include <algorithm>

//Desmume includes
#define SFORMAT dsSFORMAT

#include "MMU.h"
#include "NDSSystem.h"
#include "debug.h"
#include "sndsdl.h"
#include "render3D.h"
#include "rasterize.h"
#include "saves.h"
#include "firmware.h"
#include "GPU_osd.h"
#include "addons.h"

#define MODULENAMESPACE desmume
#include <module_helper.h>
using namespace desmume;

//Desume interface
volatile bool								execute = true;

#define SNDCORE_MDFN	10010
extern SoundInterface_struct				SNDMDFN;

SoundInterface_struct*						SNDCoreList[] =
{
	&SNDDummy,
	&SNDMDFN,
	NULL
};

GPU3DInterface*								core3DList[] =
{
	&gpu3DRasterize,
	NULL
};

static bool									OneScreen;
static bool									HoldingScreenButton;
static bool									NeedScreenClear;
static int32_t								TouchX;
static int32_t								TouchY;
static int32_t								MaxCursorTime;
static int32_t								CursorFrames;
static int32_t								TopScreenLine;
static int32_t								BottomScreenLine;

static uint8_t								CursorImage[16*16] =
{
	2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 1, 1, 1, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 1, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 2, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0
};

template<int min, int max>
static int32_t								ClampedMove				(int32_t aTarget, int32_t aValue)
{
	return std::max(min, std::min(max, aTarget + aValue));
}

static void									ReadSettings			(const char* aName)
{
	//Force screen refersh
	NeedScreenClear = true;

	//Read desmume core settings
	CommonSettings.num_cores = MDFN_GetSettingUI("desmume.num_cores");
	CommonSettings.GFX3D_HighResolutionInterpolateColor = MDFN_GetSettingB("desmume.highresintp");
	CommonSettings.GFX3D_EdgeMark = MDFN_GetSettingB("desmume.edgemark");
	CommonSettings.GFX3D_Fog = MDFN_GetSettingB("desmume.fog");
	CommonSettings.GFX3D_Texture = MDFN_GetSettingB("desmume.texture");
	CommonSettings.GFX3D_LineHack = MDFN_GetSettingB("desmume.linehack");
	CommonSettings.GFX3D_Zelda_Shadow_Depth_Hack = MDFN_GetSettingB("desmume.zeldashadowhack");
	CommonSettings.rigorous_timing = MDFN_GetSettingB("desmume.rigorous_timing");
	CommonSettings.advanced_timing = MDFN_GetSettingB("desmume.advanced_timing");

	//Read local settings
	MaxCursorTime = MDFN_GetSettingI("desmume.cursor_time") * 60;
	CursorFrames = MaxCursorTime;

	OneScreen = MDFN_GetSettingB("desmume.one_screen");
	bool useGap = MDFN_GetSettingB("desmume.screen_gap");

	//Calculate the new screen positions
	if(!OneScreen)
	{
		TopScreenLine = 0;
		BottomScreenLine = 192 + (useGap ? 32 : 0);
	}
	else if((TopScreenLine >= 0) && (BottomScreenLine >= 0)) //Don't reset the screen positions if not needed
	{
		TopScreenLine = 0;
		BottomScreenLine = -1;
	}
}

namespace MODULENAMESPACE
{
	extern MDFNGI							ModuleInfo;

	static const InputDeviceInputInfoStruct	GamepadIDII[] =
	{
		{ "r",			"Right Shoulder",		11,	IDIT_BUTTON,			NULL	},
		{ "l",			"Left Shoulder",		10,	IDIT_BUTTON,			NULL	},
		{ "x",			"X (center, upper)",	8,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "y",			"Y (left)",				6,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "a",			"A (right)",			9,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "b",			"B (center, lower)",	7,	IDIT_BUTTON_CAN_RAPID,	NULL	},
		{ "start",		"START",				5,	IDIT_BUTTON,			NULL	},
		{ "select",		"SELECT",				4,	IDIT_BUTTON,			NULL	},
		{ "up",			"UP ↑",					0,	IDIT_BUTTON,			"down"	},
		{ "down",		"DOWN ↓",				1,	IDIT_BUTTON,			"up"	},
		{ "left",		"LEFT ←",				2,	IDIT_BUTTON,			"right"	},
		{ "right",		"RIGHT →",				3,	IDIT_BUTTON,			"left"	},
		{ "lid",		"Lid",					12,	IDIT_BUTTON,			NULL	},
		{ "screen",		"Toggle Screens",		13, IDIT_BUTTON,			NULL	},
		{ "cur_up",		"Touch Screen Up",		14, IDIT_BUTTON,			NULL	},
		{ "cur_down",	"Touch Screen Down",	15, IDIT_BUTTON,			NULL	},
		{ "cur_left",	"Touch Screen Left",	16, IDIT_BUTTON,			NULL	},
		{ "cur_right",	"Touch Screen Right",	17, IDIT_BUTTON,			NULL	},
		{ "touch",		"Touch Screen",			18, IDIT_BUTTON,			NULL	}
	};

	static InputDeviceInfoStruct 			InputDeviceInfoPort[] =
	{
		{"gamepad", "Gamepad",	NULL,	19,	GamepadIDII},
	};


	static const InputPortInfoStruct 		PortInfo[] =
	{
		{0, "port1", "Port 1", 1, InputDeviceInfoPort, "gamepad"},
	};

	static InputInfoStruct					ModuleInput =
	{
		1,
		PortInfo
	};


	static FileExtensionSpecStruct			ModuleExtensions[] = 
	{
		{".nds", "NDS ROM"},
		{0, 0}
	};


	static MDFNSetting						ModuleSettings[] =
	{
		{"desmume.num_cores",		MDFNSF_NOFLAGS,	"Number of CPU cores to use. (1, 2 or 4 (3=2))",				NULL,	MDFNST_UINT,	"1",	"1",	"4",	0,	ReadSettings},
		{"desmume.highresintp",		MDFNSF_NOFLAGS,	"Use High resolution color interpolation",						NULL,	MDFNST_BOOL,	"1",	"0",	"1",	0,	ReadSettings},
		{"desmume.edgemark",		MDFNSF_NOFLAGS,	"Edge Mark?",													NULL,	MDFNST_BOOL,	"1",	"0",	"1",	0,	ReadSettings},
		{"desmume.fog",				MDFNSF_NOFLAGS,	"Enable Fog",													NULL,	MDFNST_BOOL,	"1",	"0",	"1",	0,	ReadSettings},
		{"desmume.texture",			MDFNSF_NOFLAGS,	"Enable Texturing",												NULL,	MDFNST_BOOL,	"1",	"0",	"1",	0,	ReadSettings},
		{"desmume.linehack",		MDFNSF_NOFLAGS,	"Enable Line Hack?",											NULL,	MDFNST_BOOL,	"1",	"0",	"1",	0,	ReadSettings},
		{"desmume.zeldashadowhack",	MDFNSF_NOFLAGS,	"Enable Zelda Shadow Depth Hack",								NULL,	MDFNST_BOOL,	"0",	"0",	"1",	0,	ReadSettings},
		{"desmume.rigorous_timing",	MDFNSF_NOFLAGS,	"Enable Rigorous Timing",										NULL,	MDFNST_BOOL,	"0",	"0",	"1",	0,	ReadSettings},
		{"desmume.advanced_timing",	MDFNSF_NOFLAGS,	"Enable Advanced Timing",										NULL,	MDFNST_BOOL,	"1",	"0",	"1",	0,	ReadSettings},
		{"desmume.one_screen",		MDFNSF_NOFLAGS,	"Display only one screen",										NULL,	MDFNST_BOOL,	"0",	"0",	"1",	0,	ReadSettings},
		{"desmume.screen_gap",		MDFNSF_NOFLAGS,	"Display 32 pixel gap between screens.",						NULL,	MDFNST_BOOL,	"0",	"0",	"1",	0,	ReadSettings},
		{"desmume.cursor_time",		MDFNSF_NOFLAGS,	"Time to display touch screen cursor. (-1 never, 0 always)",	NULL,	MDFNST_INT,		"0",	"-1",	"10",	0,	ReadSettings},
		{NULL}
	};

	static int								ModuleLoad				(const char *name, MDFNFILE *fp)
	{
		//Get Game MD5
		md5_context md5;
		md5.starts();
		md5.update(fp->data, fp->size);
		md5.finish(MDFNGameInfo->MD5);

		//Fetch settings
		ReadSettings(0);

		//Setup desmume
		struct NDS_fw_config_data fw_config;
		NDS_FillDefaultFirmwareConfigData(&fw_config);

		addonsChangePak(NDS_ADDON_NONE);
		NDS_Init();
		NDS_CreateDummyFirmware(&fw_config);
		NDS_3D_ChangeCore(0);
		SPU_ChangeSoundCore(SNDCORE_MDFN, 735 * 2);		
		backup_setManualBackupType(MC_TYPE_AUTODETECT);

		//Load ROM
		NDS_LoadROM(fp->data, fp->size);

		return 1;
	}

	static bool								ModuleTestMagic			(const char *name, MDFNFILE *fp)
	{
		return !strcasecmp(fp->ext, "nds");
	}

	static void								ModuleCloseGame			()
	{
		NDS_DeInit();

		//Clean up
		Resampler::Kill();
	}


	static int								ModuleStateAction		(StateMem *sm, int load, int data_only)
	{
		if(!load)
		{
			EMUFILE_MEMORY data;
			if(savestate_save(&data, 0))
			{
				smem_write32le(sm, data.size());
				smem_write(sm, data.buf(), data.size());
				return 1;
			}

			return 0;
		}
		else
		{
			uint32_t len;
			smem_read32le(sm, &len);

			if(len)
			{
				EMUFILE_MEMORY data(len);
				smem_read(sm, data.buf(), len);

				return savestate_load(&data) ? 1 : 0;
			}

			return 0;
		}
	}

	static void								ModuleEmulate			(EmulateSpecStruct *espec)
	{
		//AUDIO PREP
		Resampler::Init(espec, 44100.0);

		//INPUT: desmume has a rather intricate input scheme, something here is probably not the 'right' way to do it.
		uint32_t portData = Input::GetPort<0, 3>() << 1; //input.array[0] will never be set

		//Touch Screen
		uint32_t touchData = (portData >> 15) & 0x1F;

		TouchX = ClampedMove<0, 255>(TouchX, ((touchData & 4) ? -1 : 0) + ((touchData & 8) ? 1 : 0));
		TouchY = ClampedMove<0, 191>(TouchY, ((touchData & 1) ? -1 : 0) + ((touchData & 2) ? 1 : 0));
		if(touchData & 16)
		{
			NDS_setTouchPos(TouchX, TouchY);
		}
		else
		{
			NDS_releaseTouch();
		}

		CursorFrames = (touchData) ? MaxCursorTime : (CursorFrames - 1);

		//Buttons
		NDS_beginProcessingInput();
		UserButtons& input = NDS_getProcessingUserInput().buttons;
		for(int i = 0; i != 14; i ++, portData >>= 1)
		{
			input.array[i] = portData & 1;
		}

		NDS_endProcessingInput();

		//Screen toggle button
		bool ScreenButtonDown = (portData & 1) ? true : false;

		//CHEATS
		//EMULATE
		if(espec->skip)
		{
			NDS_SkipNextFrame();
		}

		NDS_exec<false>();
		SPU_Emulate_user();

		//VIDEO: TODO: Support other color formats
		if(NeedScreenClear)
		{
			Video::Clear<uint32_t>(espec, 256, 192 * 2 + 32);
			NeedScreenClear = false;
		}

		//Screen toogle
		if(OneScreen && (ScreenButtonDown && !HoldingScreenButton))
		{
			bool gotoTop = TopScreenLine < 0;

			TopScreenLine = (gotoTop) ? 0 : -1;
			BottomScreenLine = (gotoTop) ? -1 : 0;
			MDFND_DispMessage((UTF8*)strdup((TopScreenLine == 0) ? "Showing Top Screen" : "Showing Bottom Screen"));
		}

		HoldingScreenButton = ScreenButtonDown;

		//Top screen
		if(TopScreenLine >= 0)
		{
			Video::BlitRGB15<0, 1, 2, 2, 1, 0, -1>(espec, (const uint16_t*)GPU_screen, 256, 192, 256, 0, TopScreenLine);
		}

		//Bottom screen
		if(BottomScreenLine >= 0)
		{
			Video::BlitRGB15<0, 1, 2, 2, 1, 0, -1>(espec, (const uint16_t*)GPU_screen + (256 * 192), 256, 192, 256, 0, BottomScreenLine);

			//Draw cursor
			if(((MaxCursorTime >= 0 && (CursorFrames >= 0)) || MaxCursorTime == 0))
			{
				uint32_t* bottomScreenLocation = (uint32_t*)&espec->surface->pixels[espec->surface->pitchinpix * BottomScreenLine];

				for(int i = 0; i != 16; i ++)
				{
					if((TouchY + i) >= 192)
					{
						break;
					}

					for(int j = 0; j != 16; j ++)
					{
						if((TouchX + j) >= 256)
						{
							break;
						}

						if(CursorImage[i * 16 + j])
						{
							bottomScreenLocation[(TouchY + i) * espec->surface->pitchinpix + (TouchX + j)] = (CursorImage[i * 16 + j] == 1) ? 0xFFFFFFFF : 0;
						}
					}
				}
			}
		}

		//Set output rectangle
		Video::SetDisplayRect(espec, 0, 0, 256, OneScreen ? 192 : (BottomScreenLine + 192));

		//AUDIO
		Resampler::Fetch(espec);

		//TIMING
		espec->MasterCycles = 1LL * 100;
	}

	static void								ModuleSetInput			(int port, const char *type, void *ptr)
	{
		Input::SetPort(port, (uint8_t*)ptr);
	}

	static void								ModuleDoSimpleCommand	(int cmd)
	{
		if(cmd == MDFN_MSC_RESET || cmd == MDFN_MSC_POWER)
		{
			NDS_Reset();
		}
	}

	MDFNGI									ModuleInfo = 
	{
	/*	shortname:			*/	"desmume",
	/*	fullname:			*/	"Nintendo DS (desmume)",
	/*	FileExtensions:		*/	ModuleExtensions,
	/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
	/*	Debugger:			*/	0,
	/*	InputInfo:			*/	&ModuleInput,

	/*	Load:				*/	ModuleLoad,
	/*	TestMagic:			*/	ModuleTestMagic,
	/*	LoadCD:				*/	0,
	/*	TestMagicCD:		*/	0,
	/*	CloseGame:			*/	ModuleCloseGame,
	/*	ToggleLayer:		*/	0,
	/*	LayerNames:			*/	0,
	/*	InstallReadPatch:	*/	0,
	/*	RemoveReadPatches:	*/	0,
	/*	MemRead:			*/	0,
	/*	StateAction:		*/	ModuleStateAction,
	/*	Emulate:			*/	ModuleEmulate,
	/*	SetInput:			*/	ModuleSetInput,
	/*	DoSimpleCommand:	*/	ModuleDoSimpleCommand,
	/*	Settings:			*/	ModuleSettings,
	/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
	/*	fps:				*/	0,
	/*	multires:			*/	true,
	/*	lcm_width:			*/	256,
	/*	lcm_height:			*/	192 * 2 + 32,
	/*	dummy_separator:	*/	0,
	/*	nominal_width:		*/	256,
	/*	nominal_height:		*/	192 * 2 + 32,
	/*	fb_width:			*/	256,
	/*	fb_height:			*/	192 * 2 + 32,
	/*	soundchan:			*/	2
	};
}

#ifdef MLDLL
#define VERSION_FUNC GetVersion
#define GETEMU_FUNC GetEmulator
#ifdef __WIN32__
#define DLL_PUBLIC __attribute__((dllexport))
#else
#define DLL_PUBLIC __attribute__ ((visibility("default")))
#endif
#else
#define VERSION_FUNC desmumeGetVersion
#define GETEMU_FUNC desmumeGetEmulator
#define	DLL_PUBLIC
#endif

extern "C" DLL_PUBLIC	uint32_t		VERSION_FUNC()
{
	return 0x918;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC(uint32_t aIndex)
{
	return (aIndex == 0) ? &MODULENAMESPACE::ModuleInfo : 0;
}


