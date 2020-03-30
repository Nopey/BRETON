// EZ_IOP IS A "CUT DOWN" (butchered) VERSION OF ULaunchElf
// THANK THEM FOR BEING AWESOME
// https://github.com/AKuHAK/uLaunchELF/blob/8b4d00e47279a9eecb8d66888ba3bc247b41c6f0/main.c
// and some portion of it is (original source by Hermes in BOOT.c - cogswaploader)
//
// About ULaunchElf:
// Project maintainers:");
//   sp193");
//   AKuHAK");
// ");
// uLaunchELF Project maintainers:");
//   Eric Price       (aka: 'E P')");
//   Ronald Andersson (aka: 'dlanor')");
// ");
// Other contributors:");
//   Polo35, radad, Drakonite, sincro");
//   kthu, Slam-Tilt, chip, pixel, Hermes");
//   and others in the PS2Dev community");
// ");
// Main release site:");
//   \"https://github.com/AKuHAK/uLaunchELF/releases\"");
// ");
// Ancestral project: LaunchELF v3.41");
// Created by:        Mirakichi");
// Project maintainers:");
//   sp193");
//   AKuHAK");
// ");
// uLaunchELF Project maintainers:");
//   Eric Price       (aka: 'E P')");
//   Ronald Andersson (aka: 'dlanor')");
// ");
// Other contributors:");
//   Polo35, radad, Drakonite, sincro");
//   kthu, Slam-Tilt, chip, pixel, Hermes");
//   and others in the PS2Dev community");
// ");
// Main release site:");
//   \"https://github.com/AKuHAK/uLaunchELF/releases\"");
// ");
// Ancestral project: LaunchELF v3.41");
// Created by:        Mirakichi");
#define SCECdESRDVD_0 0x15  // ESR-patched DVD, as seen without ESR driver active
#define SCECdESRDVD_1 0x16  // ESR-patched DVD, as seen with ESR driver active

#define usbhdfsd_MAX_DRIVES 10
char usbhdfsd_ix[10] = {'0', 0, 0, 0, 0, 0, 0, 0, 0, 0};
int usbhdfsd_max_drives = usbhdfsd_MAX_DRIVES;
unsigned long long usbhdfsd_scan_time = 0;
int usbhdfsd_scanned = 0;  //0==Not_found_OR_No_Multi 1==found_Multi_mass_once
int usbhdfsd_loaded = 0;   //0==none, 1==internal, 2==external

#include <stdio.h>
#include <tamtypes.h>
#include <sifcmd.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <string.h>
#include <malloc.h>
#include <libhdd.h>
#include <libmc.h>
#include <libpad.h>
#include <sys/stat.h>
#include <iopheap.h>
#include <errno.h>
#include <fileXio_rpc.h>
#include <iopcontrol.h>
#include <stdarg.h>
#include <sbv_patches.h>
#include <slib.h>
#include <smem.h>
#include <smod.h>
#include <sys/fcntl.h>
#include <debug.h>
#include <libcdvd.h>
#include <libjpg.h>
#include <libkbd.h>
#include <floatlib.h>
#include <usbhdfsd-common.h>

static void delay(int count)
{
	int i;
	int ret;
	for (i = 0; i < count; i++) {
		ret = 0x01000000;
		while (ret--)
			asm("nop\nnop\nnop\nnop");
	}
}

enum {
	SCREEN_MARGIN = 16,
	FONT_WIDTH = 8,
	FONT_HEIGHT = 16,
	LINE_THICKNESS = 3,

	MAX_NAME = 256,
	MAX_PART_NAME = 32,
	MAX_PATH_PAD = 30,
	MAX_PATH = 1025,
	MAX_ENTRY = 2048,
	MAX_PARTITIONS = 600,
	MAX_MENU_TITLE = 40,
	MAX_ELF_TITLE = 72,
	MAX_TEXT_LINE = 80
};

//---------------------------------------------------------------------------
//File name:   main.c
//---------------------------------------------------------------------------
extern "C"{
extern u8 iomanx_irx[];
extern int size_iomanx_irx;
extern u8 filexio_irx[];
extern int size_filexio_irx;
extern u8 ps2dev9_irx[];
extern int size_ps2dev9_irx;
extern u8 ps2ip_irx[];
extern int size_ps2ip_irx;
extern u8 ps2atad_irx[];
extern int size_ps2atad_irx;
extern u8 ps2hdd_irx[];
extern int size_ps2hdd_irx;
extern u8 ps2fs_irx[];
extern int size_ps2fs_irx;
extern u8 poweroff_irx[];
extern int size_poweroff_irx;
extern u8 loader_elf;
extern int size_loader_elf;
extern u8 ps2netfs_irx[];
extern int size_ps2netfs_irx;
extern u8 iopmod_irx[];
extern int size_iopmod_irx;
extern u8 usbd_irx[];
extern int size_usbd_irx;
extern u8 usbhdfsd_irx[];
extern int size_usbhdfsd_irx;
extern u8 ps2kbd_irx[];
extern int size_ps2kbd_irx;
extern u8 mcman_irx[];
extern int size_mcman_irx;
extern u8 mcserv_irx[];
extern int size_mcserv_irx;
extern u8 sior_irx[];
extern int size_sior_irx;
}
//#define DEBUG
#ifdef DEBUG
#define dbgprintf(args...) scr_printf(args)
#define dbginit_scr() init_scr()
#else
#define dbgprintf(args...) \
	do {                   \
	} while (0)
#define dbginit_scr() \
	do {              \
	} while (0)
#endif



//State of module collections
static u8 have_NetModules = 0;
static u8 have_HDD_modules = 0;
//State of Uncheckable Modules (invalid header)
static u8 have_cdvd = 0;
static u8 have_usbd = 0;
static u8 have_usbhdfsd = 0;
static u8 have_ps2smap = 0;
static u8 have_ps2ftpd = 0;
static u8 have_ps2kbd = 0;
//State of Checkable Modules (valid header)
static u8 have_poweroff = 0;
static u8 have_ps2dev9 = 0;
static u8 have_ps2ip = 0;
static u8 have_ps2atad = 0;
static u8 have_ps2hdd = 0;
static u8 have_ps2fs = 0;
static u8 have_ps2netfs = 0;
static u8 have_vmc_fs = 0;
//State of whether DEV9 was successfully loaded or not.
static u8 ps2dev9_loaded = 0;

static u8 done_setupPowerOff = 0;
static u8 ps2kbd_opened = 0;

//Static function declarations
static void load_ps2dev9(void);
static void load_ps2ip(void);
static void load_ps2atad(void);
static void load_ps2ftpd(void);
static void load_ps2netfs(void);
static void loadBasicModules(void);
static int loadExternalFile(char *argPath, void **fileBaseP, int *fileSizeP);
static int loadExternalModule(char *modPath, void *defBase, int defSize);
static void loadUsbDModule(void);
static void loadUsbModules(void);
static void loadKbdModules(void);
static void closeAllAndPoweroff(void);
static void poweroffHandler(int i);
static void setupPowerOff(void);
static void loadNetModules(void);
static void startKbd(void);
static void Reset(void);
static void load_ps2dev9(void)
{
	int ret, rcode;

	if (!have_ps2dev9) {
		ret = SifExecModuleBuffer(ps2dev9_irx, size_ps2dev9_irx, 0, NULL, &rcode);
		ps2dev9_loaded = (ret >= 0 && rcode == 0);  //DEV9.IRX must have successfully loaded and returned RESIDENT END.
		have_ps2dev9 = 1;
	}
}
//------------------------------
//endfunc load_ps2dev9
//---------------------------------------------------------------------------
static void load_ps2atad(void)
{
	int ret;
	static char hddarg[] = "-o"
	                       "\0"
	                       "4"
	                       "\0"
	                       "-n"
	                       "\0"
	                       "20";
	static char pfsarg[] = "-m"
	                       "\0"
	                       "4"
	                       "\0"
	                       "-o"
	                       "\0"
	                       "10"
	                       "\0"
	                       "-n"
	                       "\0"
	                       "40";

	load_ps2dev9();
	if (!have_ps2atad) {
		SifExecModuleBuffer(ps2atad_irx, size_ps2atad_irx, 0, NULL, &ret);
		have_ps2atad = 1;
	}
	if (!have_ps2hdd) {
		SifExecModuleBuffer(ps2hdd_irx, size_ps2hdd_irx, sizeof(hddarg), hddarg, &ret);
		have_ps2hdd = 1;
	}
	if (!have_ps2fs) {
		SifExecModuleBuffer(ps2fs_irx, size_ps2fs_irx, sizeof(pfsarg), pfsarg, &ret);
		have_ps2fs = 1;
	}
}
//------------------------------
//endfunc load_ps2atad
//---------------------------------------------------------------------------
static void loadBasicModules(void)
{
	int ret;

	SifExecModuleBuffer(iomanx_irx, size_iomanx_irx, 0, NULL, &ret);
	SifExecModuleBuffer(filexio_irx, size_filexio_irx, 0, NULL, &ret);

	SifLoadModule("rom0:SIO2MAN", 0, NULL);

#ifdef SIO_DEBUG
	int id;
	// I call this just after SIO2MAN have been loaded
	sio_init(38400, 0, 0, 0, 0);
	DPRINTF("Hello from EE SIO!\n");

	SIOR_Init(0x20);

	id = SifExecModuleBuffer(sior_irx, size_sior_irx, 0, NULL, &ret);
	scr_printf("\t sior id=%d _start ret=%d\n", id, ret);
	DPRINTF("sior id=%d _start ret=%d\n", id, ret);
#endif

	SifExecModuleBuffer(mcman_irx, size_mcman_irx, 0, NULL, &ret);  //Home
	//SifLoadModule("rom0:MCMAN", 0, NULL); //Sony
	SifExecModuleBuffer(mcserv_irx, size_mcserv_irx, 0, NULL, &ret);  //Home
	//SifLoadModule("rom0:MCSERV", 0, NULL); //Sony
	SifLoadModule("rom0:PADMAN", 0, NULL);
}
//------------------------------
//endfunc loadBasicModules
//---------------------------------------------------------------------------

#define CDVD_IRX 0xB001337
#define CDVD_FINDFILE 0x01
#define CDVD_GETDIR 0x02
#define CDVD_STOP 0x04
#define CDVD_TRAYREQ 0x05
#define CDVD_DISKREADY 0x06
#define CDVD_FLUSHCACHE 0x07
#define CDVD_GETSIZE 0x08


struct TocEntry
{
	u32 fileLBA;
	u32 fileSize;
	u8 fileProperties;
	u8 padding1[3];
	char filename[128 + 1];
	u8 padding2[3];
} __attribute__((packed));


enum CDVD_getMode {
	CDVD_GET_FILES_ONLY = 1,
	CDVD_GET_DIRS_ONLY = 2,
	CDVD_GET_FILES_AND_DIRS = 3
};

// Macros for TrayReq
#define CdTrayOpen 0
#define CdTrayClose 1
#define CdTrayCheck 2

// Macros for DiskReady
#define CdComplete 0x02
#define CdNotReady 0x06
#define CdBlock 0x00
#define CdNonBlock 0x01
static SifRpcClientData_t cd0;
int cdvd_inited = 0;
int cdmode;      //Last detected disc type
int old_cdmode;  //used for disc change detection
int uLE_cdmode;  //used for smart disc detection
static unsigned sbuff[0x1300] __attribute__((aligned(64)));
static int CDVD_Init()
{
	int i;

	while (1) {
		if (SifBindRpc(&cd0, CDVD_IRX, 0) < 0)
			return -1;  // bind error
		if (cd0.server != 0)
			break;
		i = 0x10000;
		while (i--)
			;
	}

	cdvd_inited = 1;

	return 0;
}
int uLE_cdDiscValid(void)  //returns 1 if disc valid, else returns 0
{
	cdmode = sceCdGetDiskType();

	switch (cdmode) {
		case SCECdPSCD:
		case SCECdPSCDDA:
		case SCECdPS2CD:
		case SCECdPS2CDDA:
		case SCECdPS2DVD:
		//	case SCECdESRDVD_0:
		//	case SCECdESRDVD_1:
		case SCECdCDDA:
		case SCECdDVDV:
			return 1;
		case SCECdNODISC:
		case SCECdDETCT:
		case SCECdDETCTCD:
		case SCECdDETCTDVDS:
		case SCECdDETCTDVDD:
		case SCECdUNKNOWN:
		case SCECdIllegalMedia:
		default:
			return 0;
	}
}
//------------------------------
//endfunc uLE_cdDiscValid
//---------------------------------------------------------------------------
static void CDVD_Stop()
{
	if (!cdvd_inited)
		return;

	SifCallRpc(&cd0, CDVD_STOP, 0, (void *)(&sbuff[0]), 0, (void *)(&sbuff[0]), 0, 0, 0);

	return;
}
static int Check_ESR_Disc(void)
{
	sceCdRMode ReadMode;
	int result;
	char SectorBuffer[2112];

	ReadMode.trycount = 5;
	ReadMode.spindlctrl = SCECdSpinNom;
	ReadMode.datapattern = SCECdSecS2048;
	ReadMode.pad = 0;

	result = sceCdReadDVDV(14, 1, SectorBuffer, &ReadMode);  // read LBA 14
	sceCdSync(0);
	if (result != 0) {
		if (sceCdGetError() == SCECdErNO) {
			result = (!strncmp(SectorBuffer + 37, "+NSR", 4)) ? 1 : 0;
		} else
			result = -1;
	} else
		result = -1;

	return result;
}
int uLE_cdStop(void)
{
	int test;

	old_cdmode = cdmode;
	test = uLE_cdDiscValid();
	uLE_cdmode = cdmode;
	if (test) {                     //if stable detection of a real disc is achieved
		if ((cdmode != old_cdmode)  //if this was a new detection
		    && ((cdmode == SCECdDVDV) || (cdmode == SCECdPS2DVD))) {
			test = Check_ESR_Disc();
			printf("Check_ESR_Disc => %d\n", test);
			if (test > 0) {  //ESR Disc ?
				uLE_cdmode = (cdmode == SCECdPS2DVD) ? SCECdESRDVD_1 : SCECdESRDVD_0;
			}
		}
		CDVD_Stop();
		sceCdSync(0);
	}
	return uLE_cdmode;
}
//------------------------------
//endfunc uLE_cdStop
//---------------------------------------------------------------------------
static void CDVD_FlushCache()
{
	if (!cdvd_inited)
		return;

	SifCallRpc(&cd0, CDVD_FLUSHCACHE, 0, (void *)(&sbuff[0]), 0, (void *)(&sbuff[0]), 0, 0, 0);

	return;
}
static int CDVD_DiskReady(int mode)
{
	if (!cdvd_inited)
		return -1;

	sbuff[0] = mode;

	SifCallRpc(&cd0, CDVD_DISKREADY, 0, (void *)(&sbuff[0]), 4, (void *)(&sbuff[0]), 4, 0, 0);

	return sbuff[0];
}
static void getExternalFilePath(const char *argPath, char *filePath)
{
	char *pathSep;

	pathSep = strchr(argPath, '/');

	if (!strncmp(argPath, "mass", 4)) {
		//Loading some module from USB mass:
		//This won't be for USB drivers, as mass: must first be accessible.
		strcpy(filePath, argPath);
		if (pathSep && (pathSep - argPath < 7) && pathSep[-1] == ':')
			strcpy(filePath + (pathSep - argPath), pathSep + 1);
	} else if (!strncmp(argPath, "cdfs", 4)) {
		strcpy(filePath, argPath);
		CDVD_FlushCache();
		CDVD_DiskReady(0);
	} else {
		// genFixPath(argPath, filePath);
	}
}
//------------------------------
//endfunc getExternalFilePath
//---------------------------------------------------------------------------
// loadExternalFile below will use the given path, and read the
// indicated file into a buffer it allocates for that purpose.
// The buffer address and size are returned via pointer variables,
// and the size is also returned as function value. Both those
// instances of size will be forced to Zero if any error occurs,
// and in such cases the buffer pointer returned will be NULL.
// NB: Release of the allocated memory block, if/when it is not
// needed anymore, is entirely the responsibility of the caller,
// though, of course, none is allocated if the file is not found.
//---------------------------------------------------------------------------
static int loadExternalFile(char *argPath, void **fileBaseP, int *fileSizeP)
{  //The first three variables are local variants similar to the arguments
	char filePath[MAX_PATH];
	void *fileBase;
	int fileSize;
	FILE *File;

	fileBase = NULL;
	fileSize = 0;

	getExternalFilePath(argPath, filePath);

	//Here 'filePath' is a valid path for file I/O operations
	//Which means we can now use generic file I/O
	File = fopen(filePath, "r");
	if (File != NULL) {
		fseek(File, 0, SEEK_END);
		fileSize = ftell(File);
		fseek(File, 0, SEEK_SET);
		if (fileSize) {
			if ((fileBase = memalign(64, fileSize)) > 0) {
				fread(fileBase, 1, fileSize, File);
			} else
				fileSize = 0;
		}
		fclose(File);
	}
	*fileBaseP = fileBase;
	*fileSizeP = fileSize;
	return fileSize;
}
//------------------------------
//endfunc loadExternalFile
//---------------------------------------------------------------------------
// loadExternalModule below will use the given path and attempt
// to load the indicated module. If the file is not
// found, or some error occurs in its reading,
// then the default internal module specified by the 2nd and 3rd
// arguments will be used, except if the base is NULL or the size
// is zero, in which case a value of 0 is returned. A value of
// 0 is also returned if loading of default module fails. But
// normally the value returned will be 1 for an internal default
// module, but 2 for an external module..
//---------------------------------------------------------------------------
static int loadExternalModule(char *modPath, void *defBase, int defSize)
{
	int dummy, def_OK;
    if (defBase && defSize) {
        def_OK = SifExecModuleBuffer(defBase, defSize, 0, NULL, &dummy);
    }
	if (def_OK)
		return 1;
	return 0;
}
//------------------------------
//endfunc loadExternalModule
//---------------------------------------------------------------------------
static void loadUsbDModule(void)
{
	if ((!have_usbd) && (loadExternalModule(NULL, &usbd_irx, size_usbd_irx)))
		have_usbd = 1;
}
//------------------------------
//endfunc loadUsbDModule
//---------------------------------------------------------------------------
static void loadUsbModules(void)
{
	loadUsbDModule();
	if (have_usbd && !have_usbhdfsd && (usbhdfsd_loaded = loadExternalModule(NULL, &usbhdfsd_irx, size_usbhdfsd_irx))) {
		delay(3);
		have_usbhdfsd = 1;
	}
	if (usbhdfsd_loaded == 1)                       //if using the internal mass driver
		usbhdfsd_max_drives = usbhdfsd_MAX_DRIVES;  //allow multiple drives
	else
		usbhdfsd_max_drives = 1;  //else allow only one mass drive
}
//------------------------------
//endfunc loadUsbModules
//---------------------------------------------------------------------------
static void loadKbdModules(void)
{
	loadUsbDModule();
	if ((have_usbd && !have_ps2kbd) && (loadExternalModule(NULL, &ps2kbd_irx, size_ps2kbd_irx)))
		have_ps2kbd = 1;
}
//------------------------------
//endfunc loadKbdModules
//---------------------------------------------------------------------------
static void startKbd(void)
{
	int kbd_fd;
	void *mapBase;
	int mapSize;

	printf("Entering startKbd()\r\n");
    /*loadKbdModules();
    PS2KbdInit();
    ps2kbd_opened = 1;
    if (setting->kbdmap_file[0]) {
        if ((kbd_fd = fileXioOpen(PS2KBD_DEVFILE, O_RDONLY)) >= 0) {
            printf("kbd_fd=%d; Loading Kbd map file \"%s\"\r\n", kbd_fd, setting->kbdmap_file);
            if (loadExternalFile(setting->kbdmap_file, &mapBase, &mapSize)) {
                if (mapSize == 0x600) {
                    fileXioIoctl(kbd_fd, PS2KBD_IOCTL_SETKEYMAP, mapBase);
                    fileXioIoctl(kbd_fd, PS2KBD_IOCTL_SETSPECIALMAP, mapBase + 0x300);
                    fileXioIoctl(kbd_fd, PS2KBD_IOCTL_SETCTRLMAP, mapBase + 0x400);
                    fileXioIoctl(kbd_fd, PS2KBD_IOCTL_SETALTMAP, mapBase + 0x500);
                }
                printf("Freeing buffer after setting Kbd maps\r\n");
                free(mapBase);
            }
            fileXioClose(kbd_fd);
        }
    }*/

}
//------------------------------
//endfunc startKbd
//---------------------------------------------------------------------------
// reboot IOP (original source by Hermes in BOOT.c - cogswaploader)
// dlanor: but changed now, as the original was badly bugged
static void Reset()
{
	SifInitRpc(0);
	while (!SifIopReset("", 0)) {
	};
	while (!SifIopSync()) {
	};
	SifInitRpc(0);
	SifLoadFileInit();
	sbv_patch_enable_lmb();
	sbv_patch_disable_prefix_check();

	have_cdvd = 0;
	have_usbd = 0;
	have_usbhdfsd = 0;
	have_ps2smap = 0;
	have_ps2ftpd = 0;
	have_ps2kbd = 0;
	have_NetModules = 0;
	have_HDD_modules = 0;

	loadBasicModules();

	fileXioInit();
	//Increase the FILEIO R/W buffer size to reduce overhead.
	fileXioSetRWBufferSize(128 * 1024);
	mcInit(MC_TYPE_MC);
	//	setupPad();
}
//------------------------------
//endfunc Reset
//---------------------------------------------------------------------------

void ez_iop()
{
	Reset();
    loadUsbModules();
}
//------------------------------
//endfunc main
//---------------------------------------------------------------------------
//End of file: main.c
//---------------------------------------------------------------------------
