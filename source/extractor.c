#include "extractor.h"

// Global variables for easy access
char launcher[0x1e47]; // Size of Launcher.dat from https://3dbrew.org/wiki/Home_Menu
char savedata[0x141b]; // Size of SaveData.dat from https://3dbrew.org/wiki/Home_Menu

int init(){
	// Initialise services
	gfxInitDefault();
	cfguInit();
	amInit();
	romfsInit();
	consoleInit(GFX_TOP, NULL);
	guiInit();
	
	return RL_SUCCESS;
}

int quit(){
	romfsExit();
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

	return res;
}

Result openFile(Handle* file, FS_Archive archive, char* path){
	return FSUSER_OpenFile(file, archive, (FS_Path)fsMakePath(PATH_ASCII, path), FS_OPEN_READ, 0);
}

Result readFile(char* buf, int size, Handle file){
	u32 read;
	Result res;
	res = FSFILE_Read(file, &read, 0, buf, size);
	return res;
}

/*
Result dumpBuffer(void* buf, int size, char* filename){
	FILE* dump = fopen(filename, "wb");
	printf("Dumping %s...\n", filename);
	fwrite(buf, 1, size, dump);
	fclose(dump);
	return RL_SUCCESS;
}
*/

Result loadLauncher(){
	FS_Archive syssave;
	Result res;
	u32 savedataID[] = {0x00020082, 0x0002008f, 0x00020098, 0x00020098, 0x000200a1, 0x000200a9, 0x000200b1};
	res = openArchive(&syssave, savedataID, MEDIATYPE_NAND, ARCHIVE_SYSTEM_SAVEDATA);
	if(R_FAILED(res)){error("Failed to open NAND system save"); return res;}

	Handle launcherFile;
	res = openFile(&launcherFile, syssave, "/Launcher.dat");
	if(R_FAILED(res)){error("Failed to load Launcher.dat"); return res;}

	res = readFile(launcher, sizeof(launcher), launcherFile);
	if(R_FAILED(res)){error("Failed to read Launcher.dat"); return res;}
	
	/*
	res = dumpBuffer(launcher, sizeof(launcher), "/Launcher.dat");
	if(R_FAILED(res)){error("Failed to dump Launcher.dat"); return res;}
	*/

	FSFILE_Close(launcherFile);
	FSUSER_CloseArchive(syssave);
	return RL_SUCCESS;
}

Result loadSaveData(){
	FS_Archive extdata;
	Result res;
	u32 extdataID[] = {0x00000082, 0x0000008f, 0x00000098, 0x00000098, 0x000000a1, 0x000000a9, 0x000000b1};
	res = openArchive(&extdata, extdataID, MEDIATYPE_SD, ARCHIVE_EXTDATA);
	if(R_FAILED(res)){error("Failed to open extdata"); return res;}

	Handle savedataFile;
	res = openFile(&savedataFile, extdata, "/SaveData.dat");
	if(R_FAILED(res)){error("Failed to load savedata.dat"); return res;}

	res = readFile(savedata, sizeof(savedata), savedataFile);
	if(R_FAILED(res)){error("Failed to read SaveData.dat"); return res;}

	/*
	res = dumpBuffer(savedata, sizeof(savedata), "/SaveData.dat");
	if(R_FAILED(res)){error("Failed to dump Launcher.dat\n"); return res;}
	*/

	FSFILE_Close(savedataFile);
	FSUSER_CloseArchive(extdata);
	return RL_SUCCESS;
}

s8 findTrashFolder(){
	u16 folderNames[60][0x11]; // 60 folder names, made up of 0x11 utf-16 chars
	memcpy(folderNames, launcher+0x1560, sizeof(folderNames)); // Extract folder names at 0x1560 from Launcher.dat
	/* dumpBuffer(folderNames, sizeof(folderNames), "/names.dat"); */

	u16 folderstr[0x11] = {0};
	utf8_to_utf16(folderstr, (u8*)"Trash", sizeof(folderstr));
	for(int i=0; i<60; i++){
		if(memcmp(folderstr, folderNames[i], sizeof(folderstr))==0){
			return i;
		}
	}
	error("Could not find Trash folder");
	return -1;
}

int findTitlesInFolder(u64 buf[const 60], s8 folderID){
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
	return position;
}

int refreshQueue(u64 deletionQueue[const 60]){
	if(R_FAILED(loadLauncher())) return -1;
	if(R_FAILED(loadSaveData())) return -1;
	if((u8)launcher[0]<0x2d || (u8)savedata[0]<0x4){error("Outdated HOME menu archives.\nPlease update your system to the latest version!"); return -1;}

	s8 trashID = findTrashFolder();
	if(trashID == -1) return -1;
	int deletionCount = findTitlesInFolder(deletionQueue, trashID);
	return deletionCount;
}

Result deleteTitles(u64* toDelete, int length, int deleteTickets){
	Result res = 0;
	Result deleted = 0;
	u64 pID;
	APT_GetProgramID(&pID);
	for(int i=0; i<length; i++){
		if(toDelete[i] == pID){
			if(textBox("Are you sure you want to\ndelete Trashcan as well?", C2D_Color32(255, 178, 0, 255), 1)==0) continue;
			else deleted = 1;
		}
		
		res = AM_DeleteAppTitle(MEDIATYPE_SD, toDelete[i]);
		if(R_FAILED(res)) return res;
		if(deleteTickets!=0){
			res = AM_DeleteTicket(toDelete[i]);
			if(R_FAILED(res)) return res;
		}
	}
	return deleted;
}

void returnAptHook(APT_HookType hook, void* param){
	if(hook == APTHOOK_ONRESTORE){
		refreshQueue((u64*)param);
	}
}