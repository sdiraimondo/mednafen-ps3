#include "psxcommon.h"
#include "plugins.h"

int				PluginStub				()
{
	/*Nothing*/
	return 0;
}

void*			SysLoadLibrary		(const char *lib)
{
	/*Nothing*/
	return (void*)1;
}

const char*		SysLibError			()
{
	/*Nothing*/
	return 0;
}

void			SysCloseLibrary		(void *lib)
{
	/*Nothing*/
}

//TODO: So ugly, theres more code after this too!
void			pkGPUinit				();
long			pkGPUopen				(unsigned long * disp, char * CapText, char * CfgFile);
long			pkGPUclose				();
long			pkGPUshutdown			();
void			pkGPUupdateLace			(void); // VSYNC
uint32_t		pkGPUreadStatus			(void); // READ STATUS
void			pkGPUwriteStatus		(uint32_t gdata);
void			pkGPUreadDataMem		(uint32_t * pMem, int iSize);
uint32_t		pkGPUreadData			(void);
void			pkGPUwriteDataMem		(uint32_t * pMem, int iSize);
void			pkGPUwriteData			(uint32_t gdata);
long			pkGPUconfigure			(void);
long			pkGPUdmaChain			(uint32_t * baseAddrL, uint32_t addr);
void			pkGPUabout				(void); // ABOUT
long			pkGPUfreeze				(uint32_t ulGetFreezeData, GPUFreeze_t * pF);
void			pkGPUshowScreenPic		(unsigned char * pMem);
void			pkGPUvBlank				(int val);
void			pkGPUkeypressed			(int keycode){}
long			pkGPUtest				();

long			pkPADinit				(long flags);
long			pkPADshutdown			(void);
long			pkPADopen				(unsigned long *Disp);
long			pkPADclose				(void);
long			pkPADquery				(void);
unsigned char	pkPADstartPoll			(int pad);
unsigned char	pkPADpoll				(unsigned char value);
long			pkPADreadPort			(int num, PadDataS *pad);
long			pkPADreadPort1			(PadDataS *pad);
long			pkPADreadPort2			(PadDataS *pad);
long			pkPADkeypressed			(void);
long			pkPADconfigure			(void);
void			pkPADabout				(void);
long			pkPADtest				(void);

long            pkCDRinit               (void);
long            pkCDRshutdown           (void);
long            pkCDRopen               (void);
long            pkCDRclose              (void);
long            pkCDRgetTN              (unsigned char * aBuffer);
long            pkCDRgetTD              (unsigned char aTrack, unsigned char* aBuffer);
long            pkCDRreadTrack          (unsigned char* aPosition);
unsigned char*  pkCDRgetBuffer          (void);
unsigned char*  pkCDRgetBufferSub       (void);
long            pkCDRconfigure          (void);
long            pkCDRtest               (void);
void            pkCDRabout              (void);
long            pkCDRplay               (unsigned char* aTime);
long            pkCDRstop               (void);
long            pkCDRsetfilename        (char* aFileName);
long            pkCDRgetStatus          (struct CdrStat* aStatus);
char*           pkCDRgetDriveLetter     (void);
long            pkCDRreadCDDA           (unsigned char aMinutes, unsigned char aSeconds, unsigned char aFrames, unsigned char* aBuffer);
long            pkCDRgetTE              (unsigned char, unsigned char *, unsigned char *, unsigned char *);


void*			SysLoadSym			(void *lib, const char *sym)
{
	if(strcmp(sym, "GPUinit") == 0) return pkGPUinit;
    if(strcmp(sym, "GPUshutdown") == 0) return pkGPUshutdown;
    if(strcmp(sym, "GPUopen") == 0) return pkGPUopen;
    if(strcmp(sym, "GPUclose") == 0) return pkGPUclose;
    if(strcmp(sym, "GPUreadData") == 0) return pkGPUreadData;
    if(strcmp(sym, "GPUreadDataMem") == 0) return pkGPUreadDataMem;
    if(strcmp(sym, "GPUreadStatus") == 0) return pkGPUreadStatus;
    if(strcmp(sym, "GPUwriteData") == 0) return pkGPUwriteData;
    if(strcmp(sym, "GPUwriteDataMem") == 0) return pkGPUwriteDataMem;
    if(strcmp(sym, "GPUwriteStatus") == 0) return pkGPUwriteStatus;
    if(strcmp(sym, "GPUdmaChain") == 0) return pkGPUdmaChain;
    if(strcmp(sym, "GPUupdateLace") == 0) return pkGPUupdateLace;
    if(strcmp(sym, "GPUkeypressed") == 0) return pkGPUkeypressed;
    if(strcmp(sym, "GPUfreeze") == 0) return pkGPUfreeze;
    if(strcmp(sym, "GPUshowScreenPic") == 0) return pkGPUshowScreenPic;
    if(strcmp(sym, "GPUvBlank") == 0) return pkGPUvBlank;
    if(strcmp(sym, "GPUconfigure") == 0) return pkGPUconfigure;
    if(strcmp(sym, "GPUtest") == 0) return pkGPUtest;
    if(strcmp(sym, "GPUabout") == 0) return pkGPUabout;

    if(strcmp(sym, "PADinit") == 0) return pkPADinit;
    if(strcmp(sym, "PADshutdown") == 0) return pkPADshutdown;
    if(strcmp(sym, "PADopen") == 0) return pkPADopen;
    if(strcmp(sym, "PADclose") == 0) return pkPADclose;
    if(strcmp(sym, "PADquery") == 0) return pkPADquery;
    if(strcmp(sym, "PADstartPoll") == 0) return pkPADstartPoll;
    if(strcmp(sym, "PADpoll") == 0) return pkPADpoll;
    if(strcmp(sym, "PADreadPort") == 0) return pkPADreadPort;
    if(strcmp(sym, "PADreadPort1") == 0) return pkPADreadPort1;
    if(strcmp(sym, "PADreadPort2") == 0) return pkPADreadPort2;
    if(strcmp(sym, "PADkeypressed") == 0) return pkPADkeypressed;
    if(strcmp(sym, "PADconfigure") == 0) return pkPADconfigure;
    if(strcmp(sym, "PADabout") == 0) return pkPADabout;
    if(strcmp(sym, "PADtest") == 0) return pkPADtest;

    if(strcmp(sym, "CDRinit") == 0) return pkCDRinit;
    if(strcmp(sym, "CDRshutdown") == 0) return pkCDRshutdown;
    if(strcmp(sym, "CDRopen") == 0) return pkCDRopen;
    if(strcmp(sym, "CDRclose") == 0) return pkCDRclose;
    if(strcmp(sym, "CDRgetTN") == 0) return pkCDRgetTN;
    if(strcmp(sym, "CDRgetTD") == 0) return pkCDRgetTD;
    if(strcmp(sym, "CDRreadTrack") == 0) return pkCDRreadTrack;
    if(strcmp(sym, "CDRgetBuffer") == 0) return pkCDRgetBuffer;
    if(strcmp(sym, "CDRgetBufferSub") == 0) return pkCDRgetBufferSub;
    if(strcmp(sym, "CDRconfigure") == 0) return pkCDRconfigure;
    if(strcmp(sym, "CDRtest") == 0) return pkCDRtest;
    if(strcmp(sym, "CDRabout") == 0) return pkCDRabout;
    if(strcmp(sym, "CDRplay") == 0) return pkCDRplay;
    if(strcmp(sym, "CDRstop") == 0) return pkCDRstop;
    if(strcmp(sym, "CDRsetfilename") == 0) return pkCDRsetfilename;
    if(strcmp(sym, "CDRgetStatus") == 0) return pkCDRgetStatus;
    if(strcmp(sym, "CDRgetDriveLetter") == 0) return pkCDRgetDriveLetter;
    if(strcmp(sym, "CDRreadCDDA") == 0) return pkCDRreadCDDA;
    if(strcmp(sym, "CDRgetTE") == 0) return pkCDRgetTE;

	return PluginStub;
}
//TODO: Now that THATS over, here's the rest of the useful code!

int					OpenPlugins		()
{
	//Tell libpcsxcore to load the plugins (Thats that mess up there)
	LoadPlugins();

	//Open all of the plugins and register any callbacks
	CDR_open();

	pkGPUopen(0, 0, 0);
	GPU_registerCallback(GPUbusy);

	pkSPUopen();
	pkSPUregisterCallback(SPUirq);

	PAD1_open(0);
	PAD2_open(0);

	return 0;
}

void                ClosePlugins    ()
{
	//Close all of the plugins
	CDR_close();
	pkSPUclose();
	PAD1_close();
	PAD2_close();
	GPU_close();

	//Tell libpcsxcore to unload the plugins
	ReleasePlugins();
}

