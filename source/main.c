#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

/**
* Opens a system archive.
* @param archive a pointer to where to write the archive to
* @param lowID an array of low titleIDs (per region: JPN, USA, EUR...) of the archive to be opened
* @param mediatype the type of media to be opened (MEDIATYPE_NAND or MEDIATYPE_SD)
* @param archiveID the type of archive (extdata, savedata etc.) ( https://smealum.github.io/ctrulib/fs_8h.html#a24416208e0f4c6c6efc3b2118a2d8803 )
* @return A success value
*/
Result openArchive(FS_Archive* archive, u32* lowID, FS_MediaType mediatype, FS_ArchiveID archiveID){
    u8 region;
    CFGU_SecureInfoGetRegion(&region);
    
    Result res;
    if(archiveID == ARCHIVE_SYSTEM_SAVEDATA){
        u32 archpath[2] = {mediatype, lowID[region]};
        FS_Path fspath = {PATH_BINARY, 8, archpath};
        res = FSUSER_OpenArchive(archive, archiveID, fspath);
    }else{
        u32 archpath[3] = {mediatype, lowID[region], 0x00000000};
        FS_Path fspath = {PATH_BINARY, 12, archpath};
        res = FSUSER_OpenArchive(archive, archiveID, fspath);
    }
    
    if(R_FAILED(res)) return res;
    return RL_SUCCESS;
}

/**
* Opens a file from a previously opened system archive.
* @param file pointer to the handle of the file to open
* @param archive the archive to open the file from
' @param path[] the path to the file in the archive
* @return A success value
*/
Result openFile(Handle* file, FS_Archive archive, char path[]){
    return FSUSER_OpenFile(file, archive, (FS_Path)fsMakePath(PATH_ASCII, path), FS_OPEN_READ, 0);
}

int quit(){
    gfxExit();
    return 0;
}

/**Main function*/
int main(int argc, char* argv[]){
    // Initialise services
	gfxInitDefault();
    cfguInit();
    consoleInit(GFX_TOP, NULL);
	printf("Initialised successfully\n");

	// Main loop
	while (aptMainLoop()){
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		u32 kDown = hidKeysDown();
        Result res;
		if (kDown & KEY_START)
			break; // break in order to return to menu
		if (kDown & KEY_A){ // Dump Launcher.dat (NAND savedata)
			u32 savedataID[] = {0x00020082, 0x0002008f, 0x00020098, 0x00020098, 0x000200a1, 0x000200a9, 0x000200b1};
            FS_Archive syssave;
            res = openArchive(&syssave, savedataID, MEDIATYPE_NAND, ARCHIVE_SYSTEM_SAVEDATA);
            if(R_FAILED(res)){printf("Failed to open NAND archive\n"); break;};
			printf("Opened archive successfully\n");
			
			Handle launcher;
			res = openFile(&launcher, syssave, "/Launcher.dat");
			if(R_FAILED(res)){printf("Failed to load Launcher.dat\n"); break;}
			printf("Loaded Launcher.dat successfully\n");
			
			char buf[0x1e47]; // Size of Launcher.dat from 3dbrew.org
			u32 size;
            FSFILE_Read(launcher, &size, 0, &buf, sizeof(buf));
            printf("Read %ld (%#lx) bytes to buffer\n", size, size);
			
			FILE* dump = fopen("/launcher.dat", "wb");
			printf("Dumping Launcher.dat...\n");
			fwrite(&buf, 1, sizeof(buf), dump);
			fclose(dump);
			
			printf("Dumped successfully\n");
			
			FSFILE_Close(launcher);
			FSUSER_CloseArchive(syssave);
			printf("Closed files successfully\n");
		}
        if(kDown & KEY_B){ // Dump SaveData.dat (SD Extdata)
            FS_Archive extdata;
            u32 extdataID[] = {0x00000082, 0x0000008f, 0x00000098, 0x00000098, 0x000000a1, 0x000000a9, 0x000000b1};
            res = openArchive(&extdata, extdataID, MEDIATYPE_SD, ARCHIVE_EXTDATA);
            if(R_FAILED(res)){printf("Failed to open extdata archive\n"); break;};
            printf("Opened archive successfully\n");
            
            Handle savedata;
            res = openFile(&savedata, extdata, "/SaveData.dat");
            if(R_FAILED(res)){printf("Failed to load savadata.dat\n"); break;};
            
			char buf[0x141b]; // Size of SaveData.dat from 3dbrew.org
			u32 size;
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
    return quit();
}
