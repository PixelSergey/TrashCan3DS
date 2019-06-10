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
	C2D_TargetClear(btm, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
	C2D_SceneBegin(btm);
	C2D_DrawRectangle(10, 10, 0, 100, 100, C2D_Color32f(1.0f, 0.0f, 0.0f, 1.0f),C2D_Color32f(0.0f, 1.0f, 0.0f, 1.0f),C2D_Color32f(0.0f, 0.0f, 1.0f, 1.0f),C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
	C3D_FrameEnd(0);
}

void textBox(char* message){
	C2D_Text text;
	C2D_TextBufClear(textBuf);
	C2D_TextParse(&text, textBuf, message);
	C2D_TextOptimize(&text);
	float w, h;
	C2D_TextGetDimensions(&text, 1.0f, 1.0f, &w, &h);
	while(aptMainLoop()){
		hidScanInput();
		u32 kDown = hidKeysDown();
		if(kDown & KEY_A) break;
		
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_SceneBegin(btm);
		C2D_DrawRectSolid(20, 20, 0, 280, 200, C2D_Color32f(0.75f, 0.75f, 0.75f, 1.0f));
		C2D_DrawText(&text, C2D_WithColor, (int)(160-(w/2)), (int)(120-(h/2)), 0, 1.0f, 1.0f, C2D_Color32f(0.0f, 1.0f, 0.0f, 1.0f));
		C3D_FrameEnd(0);
	}
}