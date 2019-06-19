/**
* @file extractor.h
* A header file for extractor.c. Contains all of the "brains" of the program.
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <citro2d.h>

#include "gui.h"

/**
* Initialises services and global varibles.
* @return A success value
*/
int init();

/**Exits services
* @return 0
*/
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


/*
* Dumps a buffer to a file on the SD. Used for debugging purposes.
* @param buf The buffer to dump
* @param size The size of the buffer
* @param filename The file to output to
* @return A success value
Result dumpBuffer(void* buf, int size, char* filename);
*/

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
* Finds the index of the trash folder in Launcher.dat and loads it into its global variable.
* Launcher.dat must be loaded into its global variable.
*/
void findTrashFolder();

/**
* Finds the titleIDs of the apps in the folder loaded by findTrashFolder() adn loads them into the deletionQueue.
*/
void findTitlesInFolder();

/**
* Refreshes the queue of titles to delete into the deletionQueue global variable.
* @return the number of TitleIDs in the queue, or -1 on failure
*/
int refreshQueue();

/**
* Deletes titles (and tickets) from the global deletionQueue variable.
* @param deleteTickets Delete tickets or not? 0=false, 1=true
* @return 0 on regular success, 1 if Trashcan itself was deleted, and a result value on failure
*/
Result deleteTitles(int deleteTickets);

/**
* A hook function to the APT service used to reload titles automatically when the app is restored from the HOME menu.
* @param hook The hook type (automatically passed)
* @param param Extra parameters (automatically passed)
*/
void returnAptHook(APT_HookType hook, void* param);
