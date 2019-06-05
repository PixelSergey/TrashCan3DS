#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "extractor.h"

int main(int argc, char* argv[]){
    if(R_FAILED(init())) return quit();
	
	u64 deletionQueue[60] = {0};
	int deletionCount = refreshQueue();
	if(deletionCount <= 0) return quit();
    
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
	}
    return quit();
}
