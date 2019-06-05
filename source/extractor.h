/**
* @file extractor.h
* A header file for extractor.c. Contains all of the "brains" of the program. 
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

/**
* Initialises services and global varibles.
* @return A success value
*/
int init();

/**Exits services*/
int quit();

/**
* Opens a system archive.
* @param archive A pointer to where to write the archive to
* @param lowID An array of low titleIDs (per region: JPN, USA, EUR...) of the archive to be opened
* @param mediatype The type of media to be opened (MEDIATYPE_NAND or MEDIATYPE_SD)
* @param archiveID The type of archive (extdata, savedata etc.) ( https://smealum.github.io/ctrulib/fs_8h.html#a24416208e0f4c6c6efc3b2118a2d8803 )
* @return A success value
*/
Result openArchive(FS_Archive* archive, u32* lowID, FS_MediaType mediatype, FS_ArchiveID archiveID);

/**
* Opens a file from a previously opened system archive.
* @param file A pointer to the handle of the file to open
* @param archive The archive to open the file from
* @param path A string of the path to the file in the archive
* @return A success value
*/
Result openFile(Handle* file, FS_Archive archive, char* path);

/**
* Reads data from a previously opened file.
* @param buf A buffer to write the data to
* @param size The size of the buffer
* @param file The file to read
* @return A success value
*/
Result readFile(char* buf, int size, Handle file);

/**
* Dumps a buffer to a file on the SD. Used for debugging purposes.
* @param buf The buffer to dump
* @param size The size of the buffer
* @param filename The file to output to
* @return A success value
*/
Result dumpBuffer(void* buf, int size, char* filename);

/**
* Reads the Launcher.dat file to the launcher global variable.
* @return A success value
*/
Result loadLauncher();

/**
* Reads the SaveData.dat file to the savedata global variable.
* @return A success value
*/
Result loadSaveData();

/**
* Finds the index of the trash folder in Launcher.dat. Launcher.dat must be loaded into its global variable.
* @return The index of the trash folder, or -1 if it does not exist
*/
s8 findTrashFolder();

/**
* Finds the TitleIDs of the titles in provided folder.
* This function trusts that there are no more than 60 titles in one folder, which is what the 3DS normally allows.
* (Technically, if SaveData.dat is modified externally, up to 360 titles can point to one folder, but the 3DS will not allow this)
* @param buf A u64 array of length (at least) 60 to write the matched TitleIDs to
* @param folderID The ID of the folder to match titles to
* @return The number of titleIDs written
*/
int findTitlesInFolder(u64 buf[const 60], s8 folderID);

/**
* Refreshes the queue of titles to delete into the deletionQueue global variable.
* @return the number of TitleIDs added to the queue, or -1 on failure
*/
int refreshQueue();

/**
* Deletes titles (and tickets) from an array of titleIDs.
* @param toDelete An array of u64 TitleIDs of apps to be deleted
* @param length The number of elements in the array
* @param deleteTickets Delete tickets or not? 0=false, 1=true
* @return A success value
*/
Result deleteTitles(u64* toDelete, int length, int deleteTickets);

/**
* A hook function to the APT service used to reload titles automatically when the app is restored from the HOME menu.
* @param hook The hook type (automatically passed)
* @param param Extra parameters (automatically passed)
*/
void returnAptHook(APT_HookType hook, void* param);