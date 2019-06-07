#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "extractor.h"

static aptHookCookie aptCookie; // A cookie for the hook to the APT services

int main(int argc, char* argv[]){
    if(R_FAILED(init())) return quit();
	
	u64 deletionQueue[60] = {0};
	int deletionCount = refreshQueue();
	if(deletionCount < 0) return quit();
	
	aptHook(&aptCookie, returnAptHook, (void*)deletionQueue);
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget* btm = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    
    printf("Press (A) to delete found titles\n");
    printf("Press (B) to delete found titles and tickets\n");
	printf("Press (X) to refresh the deletion queue\n");
    printf("Press (START) to exit\n\n");
	// Main loop
	while(aptMainLoop()){
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();
        
        u32 kDown = hidKeysDown();
		if(kDown & KEY_START)
			break; // Break in order to return to menu
        if(kDown & KEY_A){
            if(R_FAILED(deleteTitles(deletionQueue, deletionCount, 0))){printf("Could not delete titles!\n"); break;}
            printf("Deleted titles successfully\n");
			break;
        }
        if(kDown & KEY_B){
            if(R_FAILED(deleteTitles(deletionQueue, deletionCount, 1))){printf("Could not delete titles/tickets!\n"); break;}
            printf("Deleted titles and tickets successfully\n");
			break;
        }
		if(kDown & KEY_X){
			deletionCount = refreshQueue(deletionQueue);
			if(deletionCount < 0) return quit();
			printf("Refreshed queue successfully\n\n");
		}
		
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(btm, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(btm);
		C2D_DrawRectangle(10, 10, 0, 100, 100, C2D_Color32f(1.0f, 0.0f, 0.0f, 1.0f),C2D_Color32f(0.0f, 1.0f, 0.0f, 1.0f),C2D_Color32f(0.0f, 0.0f, 1.0f, 1.0f),C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
		C3D_FrameEnd(0);
	}
    return quit();
}
