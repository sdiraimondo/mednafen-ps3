#include <es_system.h>
#include "opengl_common/FrameBuffer.h"

#ifdef GLSL_IS_MY_FRIEND
#include "opengl_glsl/glslProgram.h"

const char* vert = "														\n\
	uniform float screenWidth;												\n\
	uniform float screenHeight;												\n\
	void main()																\n\
	{																		\n\
		gl_Position.x = (gl_Vertex.x / screenWidth) * 2.0 - 1.0;			\n\
		gl_Position.y = 0.0 - ((gl_Vertex.y / screenHeight) * 2.0 - 1.0);	\n\
		gl_FrontColor = gl_Color;											\n\
		gl_TexCoord[0] = gl_MultiTexCoord0;									\n\
	}																		\n\
";

const char* frag = "														\n\
	uniform sampler2D tex;													\n\
	void main()																\n\
	{																		\n\
		vec4 texel = texture2D(tex, gl_TexCoord[0].st);						\n\
		gl_FragColor = texel * gl_Color;									\n\
	}																		\n\
";
#else
#include "opengl_cg/cgProgram.h"

const char* vert = "														\n\
	void main_vertex														\n\
	(																		\n\
		float4 position	: POSITION,											\n\
		float4 color	: COLOR,											\n\
		float2 texCoord : TEXCOORD0,										\n\
																			\n\
		uniform float screenWidth,											\n\
		uniform float screenHeight,											\n\
																			\n\
		out float4 oPosition : POSITION,									\n\
		out float4 oColor    : COLOR,										\n\
		out float2 otexCoord : TEXCOORD										\n\
	)																		\n\
	{																		\n\
		oPosition.x = (position.x / screenWidth) * 2.0 - 1.0;				\n\
		oPosition.y = 0.0 - ((position.y / screenHeight) * 2.0 - 1.0);		\n\
		oColor = color;														\n\
		otexCoord = texCoord;												\n\
	}																		\n\
";

const char* frag = "struct output {float4 color : COLOR;}; output main_fragment(float4 color : COLOR, uniform sampler2D tex : TEXUNIT0, float2 texcoord : TEXCOORD0){output OUT; OUT.color = color * tex2D(tex, texcoord); return OUT;}";

#endif


//Rename tokens for OpenGL ES Support
#ifdef ES_OPENGLES
# define	glGenFramebuffersEXT			glGenFramebuffersOES
# define	glDeleteFramebuffersEXT			glDeleteFramebuffersOES
# define	glBindFramebufferEXT			glBindFramebufferOES
# define	glFramebufferTexture2DEXT		glFramebufferTexture2DOES
# define	GL_FRAMEBUFFER_EXT				GL_FRAMEBUFFER_OES
# define	glFramebufferRenderbufferEXT	glFramebufferRenderbufferOES
# define	GL_DEPTH_ATTACHMENT_EXT			GL_DEPTH_ATTACHMENT_OES
# define	GL_RENDERBUFFER_EXT				GL_RENDERBUFFER_OES
#endif

#if 0
#define glSplat() {uint32_t i = glGetError(); if(i) {printf("%X\n", i); abort();}}
#else
#define glSplat()
#endif

namespace
{
	//HACK:
	inline void							ApplyVertexBuffer	(GLfloat* aBuffer)
	{
		glEnableClientState(GL_VERTEX_ARRAY); glSplat();
		glVertexPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), &aBuffer[0]); glSplat();

		glEnableClientState(GL_TEXTURE_COORD_ARRAY); glSplat();
		glTexCoordPointer(2, GL_FLOAT, 9 * sizeof(GLfloat), &aBuffer[3]); glSplat();

		glEnableClientState(GL_COLOR_ARRAY); glSplat();
		glColorPointer(4, GL_FLOAT, 9 * sizeof(GLfloat), &aBuffer[5]); glSplat();

		glClientActiveTexture(GL_TEXTURE1); glSplat();
		glDisableClientState(GL_TEXTURE_COORD_ARRAY); glSplat();
		glClientActiveTexture(GL_TEXTURE0); glSplat();
	}

	LibESGL::Program*	UIShader;
}

void					ESVideo::Initialize				()
{
	ESVideoPlatform::Initialize(ScreenWidth, ScreenHeight);

	UIShader = new LibESGL::Program(vert, frag, false, false);
	UIShader->Use();

#ifdef GLSL_IS_MY_FRIEND
	glUniform1f(UIShader->ObtainToken("screenWidth"), ScreenWidth);
	glUniform1f(UIShader->ObtainToken("screenHeight"), ScreenHeight);
	glUniform1i(UIShader->ObtainToken("tex"), 0);
#else
	cgGLSetParameter1f((CGparameter)UIShader->ObtainToken("screenWidth"), ScreenWidth);
	cgGLSetParameter1f((CGparameter)UIShader->ObtainToken("screenHeight"), ScreenHeight);
#endif

	//Some settings
	InitializeState();

	// Setup vertex buffer
	VertexBuffer = (GLfloat*)malloc(VertexBufferCount * VertexSize * sizeof(GLfloat));
	ApplyVertexBuffer(VertexBuffer);

	//Texture for FillRectangle
	FillerTexture = new Texture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);

	//Init framebuffer
	glGenFramebuffersEXT(1, &FrameBufferID); glSplat();
}

void					ESVideo::Shutdown				()
{
	SetRenderTarget(0);
	glDeleteFramebuffersEXT(1, &FrameBufferID); glSplat();

	delete FillerTexture;
	free(VertexBuffer);

	ESVideoPlatform::Shutdown();
}

void					ESVideo::SetScreenSize			(uint32_t aX, uint32_t aY)
{
	ScreenWidth = aX;
	ScreenHeight = aY;
	InitializeState();

	FontManager::InitFonts();
}

void					ESVideo::Flip					()
{
	ESVideoPlatform::Flip();
	SetClip(Area(0, 0, GetScreenWidth(), GetScreenHeight()));
	glClear(GL_COLOR_BUFFER_BIT); glSplat();
}

void					ESVideo::SetRenderTarget		(FrameBuffer* aBuffer)
{
	if(aBuffer)
	{
		glBindTexture(GL_TEXTURE_2D, aBuffer->GetID()); glSplat();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glSplat();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); glSplat();
		glBindTexture(GL_TEXTURE_2D, 0); glSplat();

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FrameBufferID); glSplat();
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, aBuffer->GetID(), 0); glSplat();
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, aBuffer->GetDepthID());

		LibESGL::Program::Revert();
	}
	else
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); glSplat();
		glViewport(0, 0, GetScreenWidth(), GetScreenHeight()); glSplat();
		ApplyVertexBuffer(VertexBuffer);

		UIShader->Use();
	}
}

void					ESVideo::PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor)
{
	float r = (float)((aColor >> 24) & 0xFF) / 256.0f;
	float g = (float)((aColor >> 16) & 0xFF) / 256.0f;	
	float b = (float)((aColor >> 8) & 0xFF) / 256.0f;
	float a = (float)((aColor >> 0) & 0xFF) / 256.0f;	

	float xl = ((float)aSource.X + .5f) / (float)aTexture->GetWidth();
	float xr = ((float)aSource.Right() - .5f) / (float)aTexture->GetWidth();
	float yl = ((float)aSource.Y + .5f) / (float)aTexture->GetHeight();
	float yr = ((float)aSource.Bottom() - .5f) / (float)aTexture->GetHeight();

	const Area& Clip = ESVideo::GetClip();
	aTexture->Apply();
	SetVertex(&VertexBuffer[0 * VertexSize], Clip.X + aDestination.X, Clip.Y + aDestination.Y, r, g, b, a, xl, yl);
	SetVertex(&VertexBuffer[1 * VertexSize], Clip.X + aDestination.Right(), Clip.Y + aDestination.Y, r, g, b, a, xr, yl);
	SetVertex(&VertexBuffer[2 * VertexSize], Clip.X + aDestination.Right(), Clip.Y + aDestination.Bottom(), r, g, b, a, xr, yr);
	SetVertex(&VertexBuffer[3 * VertexSize], Clip.X + aDestination.X, Clip.Y + aDestination.Bottom(), r, g, b, a, xl, yr);

	glDrawArrays(GL_QUADS, 0, 4);
}


void					ESVideo::PresentFrame			(Texture* aTexture, const Area& aViewPort)
{
	assert(aTexture);

	aTexture->Apply();
	PresentFrame(aTexture->GetID(), aTexture->GetWidth(), aTexture->GetHeight(), aViewPort);
}

void					ESVideo::PresentFrame			(FrameBuffer* aFrameBuffer, const Area& aViewPort, bool aFlip)
{
	assert(aFrameBuffer);
	PresentFrame(aFrameBuffer->GetID(), aFrameBuffer->GetWidth(), aFrameBuffer->GetHeight(), aViewPort, aFlip);
}


void					ESVideo::UpdatePresentArea		(int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
{
	//Cache size converted to float
	float fwidth = (float)ESVideo::GetScreenWidth();
	float fheight = (float)ESVideo::GetScreenHeight();

	//Calculate underscan adjustment
	int32_t usLeft = (int32_t)(fwidth * ((float)(aUnderscan + aUnderscanFine.X) / 200.0f));
	int32_t usRight = ESVideo::GetScreenWidth() - (int32_t)(fwidth * ((float)(aUnderscan + aUnderscanFine.Width) / 200.0f));
	int32_t usTop = (int32_t)(fheight * ((float)(aUnderscan + aUnderscanFine.Y) / 200.0f));
	int32_t usBottom = ESVideo::GetScreenHeight() - (int32_t)(fheight * ((float)(aUnderscan + aUnderscanFine.Height) / 200.0f));

	//Update float size to underscaned area
	fwidth = (float)(usRight - usLeft);
	fheight = (float)(usBottom - usTop);

	//1:1 mode
	NoAspect = (aAspectOverride == 2);

	//Apply pillarbox
	if(!NoAspect && ((aAspectOverride == 0 && ESVideo::IsWideScreen()) || (aAspectOverride < 0)))
	{
		usLeft += (int32_t)(fwidth * .125f);
		usRight -= (int32_t)(fwidth * .125f);
	}

	PresentArea = Area(usLeft, usTop, usRight - usLeft, usBottom - usTop);
}

void					ESVideo::PresentFrame			(GLuint aID, uint32_t aWidth, uint32_t aHeight, const Area& aViewPort, bool aFlip)
{
	//Enter present state
	EnterPresentState();

	//Calculate output area
	Area presentArea;

	if(NoAspect)
	{
		uint32_t x = PresentArea.X, y = PresentArea.Y, w = PresentArea.Width, h = PresentArea.Height;
		Utility::CenterAndScale(x, y, w, h, aViewPort.Width, aViewPort.Height);

		presentArea = Area(x, y, w, h);
	}
	else
	{
		presentArea = PresentArea;
	}

	//Call kiddie present if possible
	if(SupportsShaders())
	{
		Present(aID, aWidth, aHeight, aViewPort, presentArea, aFlip);

		/* Reset vertex buffer */
		ApplyVertexBuffer(VertexBuffer);
	}
	else
	{
		float xl = (float)aViewPort.X / (float)aWidth;
		float xr = (float)aViewPort.Right() / (float)aWidth;
		float yl = (float)aViewPort.Y / (float)aHeight;
		float yr = (float)aViewPort.Bottom() / (float)aHeight;

		if(aFlip)
		{
			std::swap(yl, yr);
		}

		glBindTexture(GL_TEXTURE_2D, aID); glSplat();

		SetVertex(&VertexBuffer[0 * VertexSize], presentArea.X, 		presentArea.Y, 			1.0f, 1.0f, 1.0f, 1.0f, xl, yl);
		SetVertex(&VertexBuffer[1 * VertexSize], presentArea.Right(), 	presentArea.Y, 			1.0f, 1.0f, 1.0f, 1.0f, xr, yl);
		SetVertex(&VertexBuffer[2 * VertexSize], presentArea.Right(),	presentArea.Bottom(),	1.0f, 1.0f, 1.0f, 1.0f, xr, yr);
		SetVertex(&VertexBuffer[3 * VertexSize], presentArea.X,			presentArea.Bottom(),	1.0f, 1.0f, 1.0f, 1.0f, xl, yr);

		glDrawArrays(GL_QUADS, 0, 4); glSplat();
	}

	//Exit present state
	ExitPresentState();
}


void					ESVideo::SetVertex				(GLfloat* aBase, float aX, float aY, float aR, float aG, float aB, float aA, float aU, float aV)
{
	*aBase++ = aX; *aBase++ = aY; *aBase++ = 0.0f;
	*aBase++ = aU; *aBase++ = aV;
	*aBase++ = aR; *aBase++ = aG; *aBase++ = aB; *aBase++ = aA;
}

void					ESVideo::InitializeState		()
{
	//Some settings
	glEnable(GL_SCISSOR_TEST); glSplat();
	glClearColor(0, 0, 0, 0); glSplat();
	glEnable(GL_BLEND); glSplat();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glSplat();
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight()); glSplat();
}

void					ESVideo::EnterPresentState		()
{
	glDisable(GL_BLEND); glSplat();
	glDisable(GL_SCISSOR_TEST); glSplat();
}

void					ESVideo::ExitPresentState		()
{
	glEnable(GL_BLEND); glSplat();
	glEnable(GL_SCISSOR_TEST); glSplat();

	UIShader->Use();
}


Texture*				ESVideo::FillerTexture;
Area					ESVideo::Clip;

Area					ESVideo::PresentArea;

GLuint					ESVideo::FrameBufferID;

GLfloat*				ESVideo::VertexBuffer;

uint32_t				ESVideo::ScreenWidth;
uint32_t				ESVideo::ScreenHeight;
bool					ESVideo::WideScreen;

bool					ESVideo::NoAspect;
