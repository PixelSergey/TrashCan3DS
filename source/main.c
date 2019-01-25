#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

Handle ptmSysmHandle;

int main(int argc, char* argv[])
{
    // Initialise services
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
	if (R_FAILED(srvGetServiceHandle(&ptmSysmHandle, "ptm:sysm"))){printf("Fatal error\n"); goto end;}
    fsInit();
    
	printf("Initialised successfully\n");

	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		u32 kDown = hidKeysDown();
        Result res;
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
		if (kDown & KEY_A){ // Dump Launcher.dat (NAND savedata)
            u32 lowID = 0x00020098; // EUR consoles only
			u32 archpath[2] = {MEDIATYPE_NAND, lowID};
			FS_Path fspath = {PATH_BINARY, 8, archpath};
			
			FS_Archive syssave;
			res = FSUSER_OpenArchive(&syssave, ARCHIVE_SYSTEM_SAVEDATA, fspath);
			if(R_FAILED(res)){printf("Opening archive failed\n"); break;}
			printf("Opened archive successfully\n");
			
			Handle save;
			res = FSUSER_OpenFile(&save, syssave, (FS_Path)fsMakePath(PATH_ASCII, "/Launcher.dat"), FS_OPEN_READ, 0);
			if(R_FAILED(res)){printf("Failed to load Launcher.dat\n"); break;}
			printf("Loaded Launcher.dat successfully\n");
			
			u32 size;
			char buf[0x1e47]; // Size of Launcher.dat from 3dbrew.org
			FSFILE_Read(save, &size, 0, &buf, sizeof(buf));
			printf("Read %ld (%#lx) bytes to buffer\n", size, size);
			
			FILE* dump = fopen("/launcher.dat", "wb");
			printf("Dumping Launcher.dat...\n");
			fwrite(&buf, 1, sizeof(buf), dump);
			fclose(dump);
			
			printf("Dumped successfully\n");
			
			FSFILE_Close(save);
			FSUSER_CloseArchive(syssave);
			printf("Closed files successfully\n");
		}
        if(kDown & KEY_B){ // Dump SaveData.dat (SD Extdata)
            FS_Archive extdata;
            u32 lowID = 0x00000098; // EUR consoles only
            u32 archpath[3] = {MEDIATYPE_SD, lowID, 0x00000000};
            FS_Path fspath = {PATH_BINARY, 12, archpath};
            
            res = FSUSER_OpenArchive(&extdata, ARCHIVE_EXTDATA, fspath);
            if(R_FAILED(res)){printf("Opening archive failed\n");break;};
            printf("Opened archive successfully\n");
            
            Handle savedata;
            res = FSUSER_OpenFile(&savedata, extdata, (FS_Path)fsMakePath(PATH_ASCII, "/SaveData.dat"), FS_OPEN_READ, 0);
            if(R_FAILED(res)){printf("Failed to load savadata.dat\n");break;};
            
            u32 size;
			char buf[0x141b]; // Size of SaveData.dat from 3dbrew.org
			FSFILE_Read(savedata, &size, 0, &buf, sizeof(buf));
			printf("Read %ld (%#lx) bytes to buffer\n", size, size);
			
			FILE* dump = fopen("/savedata.dat", "wb");
			printf("Dumping SaveData.dat...\n");
			fwrite(&buf, 1, sizeof(buf), dump);
			fclose(dump);
			
			printf("Dumped successfully\n");
			
			FSFILE_Close(savedata);
			FSUSER_CloseArchive(extdata);
			printf("Closed files successfully\n");
            
        }
	}
	fsExit();
	svcCloseHandle(ptmSysmHandle);
	printf("Closed everything successfully\n");
end:
	gfxExit();
	return 0;
}
