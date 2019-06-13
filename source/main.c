#include "main.h"

static aptHookCookie aptCookie; // A cookie for the hook to the APT services

int main(int argc, char* argv[]){
	if(R_FAILED(init())) return quit();

	u64 deletionQueue[60] = {0};
	int deletionCount = refreshQueue();
	if(deletionCount < 0) return quit();

	aptHook(&aptCookie, returnAptHook, (void*)deletionQueue);

	while(aptMainLoop()){
		hidScanInput();
		u32 kDown = hidKeysDown();
		if(kDown & KEY_START)
			break; // Break in order to return to menu
		if(kDown & KEY_A){
			if(R_FAILED(deleteTitles(deletionQueue, deletionCount, 0))){error("Could not delete titles"); break;}
			success("Deleted titles successfully");
		}
		if(kDown & KEY_B){
			if(R_FAILED(deleteTitles(deletionQueue, deletionCount, 1))){error("Could not delete titles/tickets"); break;}
			success("Deleted titles and tickets successfully");
		}
		if(kDown & KEY_X){
			deletionCount = refreshQueue(deletionQueue);
			if(deletionCount < 0){error("Error refreshing queue"); return quit();}
			success("Refreshed queue successfully");
		}
		
		draw();
	}
	return quit();
}
