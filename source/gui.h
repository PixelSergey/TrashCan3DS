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
*/
void textBox(char* message);