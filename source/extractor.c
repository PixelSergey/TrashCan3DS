#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "extractor.h"

// Global variables for easy access
char launcher[0x1e47]; // Size of Launcher.dat from https://3dbrew.org/wiki/Home_Menu
char savedata[0x141b]; // Size of SaveData.dat from https://3dbrew.org/wiki/Home_Menu

int init(){
	// Initialise services
	gfxInitDefault();
    cfguInit();
    amInit();
    consoleInit(GFX_TOP, NULL);
	printf("Initialised successfully\n\n");
    
	// Initalise variables
    if(R_FAILED(loadLauncher())) return -1;
    if(R_FAILED(loadSaveData())) return -1;
    
    if((u8)launcher[0]<0x2d || (u8)savedata[0]<0x4){printf("Outdated HOME menu archives. Please update your system to the latest version!"); return -1;}
	return RL_SUCCESS;
}

int quit(){
    amExit();
    cfguExit();
    gfxExit();
    return 0;
}

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

Result openFile(Handle* file, FS_Archive archive, char* path){
    return FSUSER_OpenFile(file, archive, (FS_Path)fsMakePath(PATH_ASCII, path), FS_OPEN_READ, 0);
}

Result readFile(char* buf, int size, Handle file){
    u32 read;
    Result res;
    res = FSFILE_Read(file, &read, 0, buf, size);
    printf("Read %ld (%#lx) bytes to buffer\n", read, read);
    return res;
}

Result dumpBuffer(void* buf, int size, char* filename){
    FILE* dump = fopen(filename, "wb");
	printf("Dumping %s...\n", filename);
	fwrite(buf, 1, size, dump);
	fclose(dump);
    return RL_SUCCESS;
}

Result loadLauncher(){
    FS_Archive syssave;
    Result res;
    u32 savedataID[] = {0x00020082, 0x0002008f, 0x00020098, 0x00020098, 0x000200a1, 0x000200a9, 0x000200b1};
    res = openArchive(&syssave, savedataID, MEDIATYPE_NAND, ARCHIVE_SYSTEM_SAVEDATA);
    if(R_FAILED(res)){printf("Failed to open NAND system save\n"); return res;}
    printf("Opened system save successfully\n");
    
    Handle launcherFile;
    res = openFile(&launcherFile, syssave, "/Launcher.dat");
    if(R_FAILED(res)){printf("Failed to load Launcher.dat\n"); return res;}
    printf("Loaded Launcher.dat successfully\n");
    
    res = readFile(launcher, sizeof(launcher), launcherFile);
    if(R_FAILED(res)){printf("Failed to read Launcher.dat\n"); return res;}
    printf("Read Launcher.dat successfully\n");
    
    /*
    res = dumpBuffer(launcher, sizeof(launcher), "/Launcher.dat");
    if(R_FAILED(res)){printf("Failed to dump Launcher.dat\n"); return res;}
    printf("Dumped Launcher.dat successfully\n");
    */
    
    FSFILE_Close(launcherFile);
    FSUSER_CloseArchive(syssave);
    printf("Closed files successfully\n\n");
    return RL_SUCCESS;
}

Result loadSaveData(){
    FS_Archive extdata;
    Result res;
    u32 extdataID[] = {0x00000082, 0x0000008f, 0x00000098, 0x00000098, 0x000000a1, 0x000000a9, 0x000000b1};
    res = openArchive(&extdata, extdataID, MEDIATYPE_SD, ARCHIVE_EXTDATA);
    if(R_FAILED(res)){printf("Failed to open extdata\n"); return res;}
    printf("Opened extdata successfully\n");
    
    Handle savedataFile;
    res = openFile(&savedataFile, extdata, "/SaveData.dat");
    if(R_FAILED(res)){printf("Failed to load savedata.dat\n"); return res;}
    printf("Loaded SaveData.dat successfully\n");
    
    res = readFile(savedata, sizeof(savedata), savedataFile);
    if(R_FAILED(res)){printf("Failed to read SaveData.dat\n"); return res;}
    printf("Read SaveData.dat successfully\n");
    
    /*
    res = dumpBuffer(savedata, sizeof(savedata), "/SaveData.dat");
    if(R_FAILED(res)){printf("Failed to dump Launcher.dat\n"); return res;}
    printf("Dumped successfully\n");
    */
    
    FSFILE_Close(savedataFile);
    FSUSER_CloseArchive(extdata);
    printf("Closed files successfully\n\n");
    return RL_SUCCESS;
}

s8 findTrashFolder(){
    printf("Extracting folder names\n");
    u16 folderNames[60][0x11]; // 60 folder names, made up of 0x11 utf-16 chars
    memcpy(folderNames, launcher+0x1560, sizeof(folderNames)); // Extract folder names at 0x1560 from Launcher.dat
    /* dumpBuffer(folderNames, sizeof(folderNames), "/names.dat"); */
    printf("Extracted names succcessfully\n");
    
    u16 folderstr[0x11] = {0};
    utf8_to_utf16(folderstr, (u8*)"Trash", sizeof(folderstr));
    for(int i=0; i<60; i++){
        if(memcmp(folderstr, folderNames[i], sizeof(folderstr))==0){
			printf("Found trash folder at index %d\n\n", i);
            return i;
        }
    }
	printf("Could not find Trash folder\n");
    return -1;
}

int findTitlesInFolder(u64 buf[const 60], s8 folderID){
    printf("Finding titles in folder\n");
    s8 folderStatus[360];
    u64 titleIDs[360];
    memcpy(folderStatus, savedata+0xf80, sizeof(folderStatus)); // Extract which folder titles belong to from 0xf80 of SaveData.dat
    memcpy(titleIDs, savedata+0x8, sizeof(titleIDs)); // Extract corresponding TitleIDs from 0x8 of SaveData.dat
    
    int position = 0;
    for(int i=0; i<360; i++){
        if(folderStatus[i] == folderID){
            buf[position] = titleIDs[i];
            position++;
        }
    }
	if(position == 0) printf("No titles in folder!\n");
	else{
		printf("Titles found:\n");
		for(int i=0; i<position; i++){
			printf("%016llx\n", titleIDs[i]);
		}
	}
    return position;
}

int refreshQueue(u64 deletionQueue[const 60]){
	s8 trashID = findTrashFolder();
    if(trashID == -1) return 0;
    int deletionCount = findTitlesInFolder(deletionQueue, trashID);
	return deletionCount;
}

Result deleteTitles(u64* toDelete, int length, int deleteTickets){
    Result res;
    for(int i=0; i<length; i++){
        res = AM_DeleteAppTitle(MEDIATYPE_SD, toDelete[i]);
        if(R_FAILED(res)) return res;
        if(deleteTickets!=0){
            res = AM_DeleteTicket(toDelete[i]);
            if(R_FAILED(res)) return res;
        }
    }
    return RL_SUCCESS;
}
