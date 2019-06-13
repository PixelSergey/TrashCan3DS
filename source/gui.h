/**
* @file gui.h
* A header file for gui.c. Handles graphics, some helper funcitons, and the GUI.
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <citro2d.h>

/**
* Initialises services.
*/
void guiInit();

/**
* Exits services.
*/
void guiExit();

/**
* Draws the main menu.
*/
void draw();

/**
* Shows a message on-screen until the (A) button is pressed.
* @param message The message to be displayed
* @param color The color of the text to be displayed
* @param type The type of texttbox to show: 0 = OK, 1 = YES/NO
* @return The user's choice: 0=NO, 1=YES/OK
*/
int textBox(char* message, u32 color, int type);

/**
* Shows a textBox with green text on-screen.
* @param message The message to be displayed
*/
void success(char* message);

/**
* Shows a textBox with red text on-screen.
* @param message The message to be displayed
*/
void error(char* message);