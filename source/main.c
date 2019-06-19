#include "main.h"

static aptHookCookie aptCookie; // A cookie for the hook to the APT services

int main(int argc, char* argv[]){
	init();
	if(refreshQueue() < 0) return quit();
	
	volatile int pleaseRefresh = 0;
	aptHook(&aptCookie, returnAptHook, (void*)&pleaseRefresh);

	while(aptMainLoop()){
		if(pleaseRefresh == 1){
			if(refreshQueue() < 0){error("Error refreshing queue"); return quit();}
			success("Refreshed queue successfully");
			pleaseRefresh = 0;
		}
		
		hidScanInput();
		u32 kDown = hidKeysDown();
		if(kDown & KEY_START)
			break; // Break in order to return to menu
		if(kDown & KEY_A){
			Result res = deleteTitles(0);
			if(res == 1) break;
			if(R_FAILED(res)){error("Could not delete titles"); break;}
			success("Deleted titles successfully");
		}
		if(kDown & KEY_B){
			Result res = deleteTitles(1);
			if(res == 1) break;
			if(R_FAILED(res)){error("Could not delete titles/tickets"); break;}
			success("Deleted titles and tickets successfully");
		}
		if(kDown & KEY_X){
			if(refreshQueue() < 0){error("Error refreshing queue"); return quit();}
			success("Refreshed queue successfully");
		}
		
		draw();
	}
	return quit();
}
