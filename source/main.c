#include "main.h"

static aptHookCookie aptCookie; // A cookie for the hook to the APT services

int main(int argc, char* argv[]){
	if(R_FAILED(init())) return quit();

	u64 deletionQueue[60] = {0};
	int deletionCount = refreshQueue();
	if(deletionCount < 0) return quit();

	aptHook(&aptCookie, returnAptHook, (void*)deletionQueue);

	printf("Press (A) to delete found titles\n");
	printf("Press (B) to delete found titles and tickets\n");
	printf("Press (X) to refresh the deletion queue\n");
	printf("Press (START) to exit\n\n");
	// Main loop
	while(aptMainLoop()){
		hidScanInput();
		u32 kDown = hidKeysDown();
		if(kDown & KEY_START)
			break; // Break in order to return to menu
		if(kDown & KEY_A){
			if(R_FAILED(deleteTitles(deletionQueue, deletionCount, 0))){textBox("Could not delete titles!"); break;}
			textBox("Deleted titles successfully");
			break;
		}
		if(kDown & KEY_B){
			if(R_FAILED(deleteTitles(deletionQueue, deletionCount, 1))){textBox("Could not delete titles/tickets!"); break;}
			textBox("Deleted titles and tickets successfully");
			break;
		}
		if(kDown & KEY_X){
			deletionCount = refreshQueue(deletionQueue);
			if(deletionCount < 0) return quit();
			textBox("Refreshed queue successfully");
		}
		
		draw();
	}
	return quit();
}
