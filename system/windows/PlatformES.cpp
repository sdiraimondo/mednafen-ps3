#include <es_system.h>
#include <mmsystem.h>
#include <gdiplus.h>

using namespace		Gdiplus;

namespace
{
	bool			want_to_die = false;
	ULONG_PTR		GdiPlusToken;
};

void				SetExit							()
{
	want_to_die = true;
}

void				LibESPlatform::Initialize		()
{
	timeBeginPeriod(1);

	CoInitializeEx(0, COINIT_MULTITHREADED);
	GdiplusStartupInput gdipsi;
	GdiplusStartup(&GdiPlusToken, &gdipsi, NULL);
}

void				LibESPlatform::Shutdown			()
{
	GdiplusShutdown(GdiPlusToken);

	timeEndPeriod(1);
}

bool				LibESPlatform::WantToDie		()
{
	return want_to_die;
}

volatile bool		LibESPlatform::WantToSleep		()
{
	return false;
}

std::string			LibESPlatform::BuildPath		(const std::string& aPath)
{
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#ifndef ES_HOME_PATH
#define ES_HOME_PATH mednafen
#endif
#define ES_DIR TOSTRING(ES_HOME_PATH)

	char path[MAX_PATH];
	SHGetFolderPathA(0, CSIDL_APPDATA, 0, 0, path);
	return std::string(path) + "/" ES_DIR "/" + aPath;
}

void				ESSUB_LoadPNG			(const std::string& aPath, Texture** aTexture)
{
	assert(aTexture);

	wchar_t buffer[1024];
	MultiByteToWideChar(CP_UTF8, 0, aPath.c_str(), -1, buffer, 512);

	Texture* output = 0;

	Bitmap img(buffer, false);
	if(img.GetWidth())
	{
		output = ESVideo::CreateTexture(img.GetWidth(), img.GetHeight(), true);
		output->Clear(0);

		BitmapData BMData;
		BMData.Width = std::min(output->GetWidth(), img.GetWidth());
		BMData.Height = std::min(output->GetHeight(), img.GetHeight());
		BMData.Stride = output->GetWidth() * 4;
		BMData.PixelFormat = PixelFormat32bppARGB;
		BMData.Scan0 = output->Map();
		BMData.Reserved = 0;

		Rect area(0, 0, img.GetWidth(), img.GetHeight());
		img.LockBits(&area, ImageLockModeRead | ImageLockModeUserInputBuf, PixelFormat32bppARGB, &BMData);
		img.UnlockBits(&BMData);

		output->Unmap();
	}

	*aTexture = output;
}

