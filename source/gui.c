#include "gui.h"

static C3D_RenderTarget* top;
static C3D_RenderTarget* btm;
C2D_TextBuf textBuf;

void guiInit(){
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	
	top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	btm = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	textBuf = C2D_TextBufNew(256);
}

void guiExit(){
	C2D_TextBufDelete(textBuf);
	
	C2D_Fini();
	C3D_Fini();
}

void draw(){
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(btm, C2D_Color32(255, 255, 255, 255));
	C2D_SceneBegin(btm);
	C2D_DrawRectangle(10, 10, 0, 100, 100, C2D_Color32(255, 0, 0, 255),C2D_Color32(0, 255, 0, 255),C2D_Color32(0, 0, 255, 255),C2D_Color32(255, 255, 255, 255));
	C3D_FrameEnd(0);
}

void textBox(char* message, u32 color, float scale){
	C2D_Text text, instr;
	C2D_TextBufClear(textBuf);
	C2D_TextParse(&text, textBuf, message);
	C2D_TextParse(&instr, textBuf, "Press \uE000 to continue");
	C2D_TextOptimize(&text);
	C2D_TextOptimize(&instr);
	float w, h, instw, insth;
	C2D_TextGetDimensions(&text, scale, scale, &w, &h);
	C2D_TextGetDimensions(&instr, 1.0, 1.0, &instw, &insth);
	
	while(aptMainLoop()){
		hidScanInput();
		u32 kDown = hidKeysDown();
		if(kDown & KEY_A) break;
		
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_SceneBegin(btm);
		C2D_DrawRectSolid(20, 20, 0, 280, 200, C2D_Color32(180, 180, 180, 255));
		C2D_DrawText(&text, C2D_WithColor, (int)(160-(w/2)), 40, 0, scale, scale, color);
		C2D_DrawText(&instr, C2D_WithColor, (int)(160-(instw/2)), 210-insth, 0, 1.0, 1.0, C2D_Color32(60, 60, 60, 255));
		C3D_FrameEnd(0);
	}
}

void success(char* message){
	textBox(message, C2D_Color32(20, 255, 133, 255), 0.6f); // Light green
}

void error(char* message){
	textBox(message, C2D_Color32(255, 45, 12, 255), 0.6f); // Annoying red
}