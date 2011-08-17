#include <es_system.h>
#include "SummerfaceWindow.h"

uint32_t ptpp(uint32_t aIn, bool aX)
{
	uint32_t s = aX ? ESVideo::GetScreenWidth() : ESVideo::GetScreenHeight();
	return s * ((double)aIn / 100.0);
}


											SummerfaceWindow::SummerfaceWindow					(const Area& aRegion, bool aBorder) :
	Interface(Summerface_WeakPtr()),
	Region(Area(ptpp(aRegion.X, 1), ptpp(aRegion.Y, 0), ptpp(aRegion.Width, 1), ptpp(aRegion.Height, 0))),
	Client(Area(Region.X + BorderWidth, Region.Y + BorderWidth, Region.Width - BorderWidth * 2, Region.Height - BorderWidth * 2)),
	UseBorder(aBorder)
{
}

//Draw the border and background, leave clip set to the windows client area
bool										SummerfaceWindow::PrepareDraw						()
{
	if(UseBorder)
	{
#if 0
		if(ImageManager::GetImage("Window"))
		{
			Texture* tex = ImageManager::GetImage("Window");
			tex->SetFilter(false);

			Area scorner = Area(Region.X, Region.Y, 8, 8);
			Area tcorner = Area(0, 0, 8, 8);
			ESVideo::PlaceTexture(tex, scorner, tcorner, 0xFFFFFFFF);

			scorner.X += Region.Width - 8;
			tcorner.X += tex->GetWidth() - 8;
			ESVideo::PlaceTexture(tex, scorner, tcorner, 0xFFFFFFFF);

			scorner.Y += Region.Height - 8;
			tcorner.Y += tex->GetHeight() - 8;
			ESVideo::PlaceTexture(tex, scorner, tcorner, 0xFFFFFFFF);

			scorner.X -= Region.Width - 8;
			tcorner.X -= tex->GetWidth() - 8;
			ESVideo::PlaceTexture(tex, scorner, tcorner, 0xFFFFFFFF);

			Area swidther = Area(Region.X + 8, Region.Y, Region.Width - 16, 8);
			Area twidther = Area(8, 0, tex->GetWidth() - 16, 8);
			ESVideo::PlaceTexture(tex, swidther, twidther, 0xFFFFFFFF);

			swidther.Y += Region.Height - 8;
			twidther.Y += tex->GetHeight() - 8;
			ESVideo::PlaceTexture(tex, swidther, twidther, 0xFFFFFFFF);

			swidther = Area(Region.X, Region.Y + 8, 8, Region.Height - 16);
			twidther = Area(0, 8, 8, tex->GetHeight() - 16);
			ESVideo::PlaceTexture(tex, swidther, twidther, 0xFFFFFFFF);

			swidther.X += Region.Width - 8;
			twidther.X += tex->GetWidth() - 8;
			ESVideo::PlaceTexture(tex, swidther, twidther, 0xFFFFFFFF);

			Area sinner = Area(Region.X + 8, Region.Y + 8, Region.Width - 16, Region.Height - 16);
			Area tinner = Area(8, 8, tex->GetWidth() - 16, tex->GetHeight() - 16);
			ESVideo::PlaceTexture(tex, sinner, tinner, 0xFFFFFFFF);

		}
		else
#endif
		{
			ESVideo::SetClip(Area(0, 0, ESVideo::GetScreenWidth(), ESVideo::GetScreenHeight()));

		//TODO: Make border color
			ESVideo::FillRectangle(Area(Region.X, Region.Y, Region.Width, BorderWidth), 0xFFFFFFFF);
			ESVideo::FillRectangle(Area(Region.X + BorderWidth, Region.Bottom(), Region.Width, BorderWidth), 0x00000080);
			ESVideo::FillRectangle(Area(Region.X, Region.Bottom() - BorderWidth, Region.Width, BorderWidth), 0xFFFFFFFF);

			ESVideo::FillRectangle(Area(Region.X, Region.Y + BorderWidth, BorderWidth, Region.Height - BorderWidth * 2), 0xFFFFFFFF);
			ESVideo::FillRectangle(Area(Region.Right(), Region.Y + BorderWidth, BorderWidth, Region.Height - BorderWidth), 0x00000080);
			ESVideo::FillRectangle(Area(Region.Right() - BorderWidth, Region.Y + BorderWidth, BorderWidth, Region.Height - BorderWidth * 2), 0xFFFFFFFF);

			ESVideo::FillRectangle(Client, Colors::BackGround);
		}

		ESVideo::SetClip(Client);

		std::string header = GetHeader();
		if(!header.empty())
		{
			FontManager::GetBigFont()->PutString(header.c_str(), 1, 1, Colors::Normal, true);
			ESVideo::FillRectangle(Area(0, FontManager::GetBigFont()->GetHeight() + 1, Client.Width, 1), 0xFFFFFFFF);
			ESVideo::SetClip(Area(Client.X, Client.Y + FontManager::GetBigFont()->GetHeight() + 3, Client.Width, Client.Height - (FontManager::GetBigFont()->GetHeight() + 3)));
		}
	}
	else
	{
		ESVideo::SetClip(Region);
	}

	return Draw();
}

void										SummerfaceWindow::SetHeader							(const std::string& aHeader, ...)
{
	char array[1024];
	va_list args;
	va_start (args, aHeader);
	vsnprintf(array, 1024, aHeader.c_str(), args);
	va_end(args);

	Header = array;
}
