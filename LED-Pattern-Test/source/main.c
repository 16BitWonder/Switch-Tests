#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	
	consoleInit(NULL);
	
	padConfigureInput(1, HidNpadStyleSet_NpadStandard);
	PadState mainPad;
	padInitializeDefault(&mainPad);
	
	hidsysInitialize();
	
	HidsysNotificationLedPattern ledPattern;
	
	HidsysUniquePadId handheldPadIds[2], npadOnePadIds[2];
	s32 handheldPadIdCnt = 0, npadOnePadIdCnt = 0;
	hidsysGetUniquePadsFromNpad(HidNpadIdType_Handheld, handheldPadIds, 2, &handheldPadIdCnt); // Get Pad IDs for Handheld
	hidsysGetUniquePadsFromNpad(HidNpadIdType_No1, npadOnePadIds, 2, &npadOnePadIdCnt); // Get Pad IDs for P1
	
	printf("\n Some General Controller LED Tests\n\n");
	printf(" (A) Solid LED\n\n");
	printf(" (B) Breathing LED\n\n");
	printf(" (Y) Disable LED\n\n");
	printf(" (+) Exit Application");
	consoleUpdate(NULL);
	
    while(appletMainLoop()) {	
		
		padUpdate(&mainPad);
		u64 kDown = padGetButtonsDown(&mainPad);
		
		if (kDown & HidNpadButton_A) { // Solid LED
			memset(&ledPattern, 0, sizeof(ledPattern));
			// Main Pattern Data
            ledPattern.baseMiniCycleDuration = 0x8; // 100ms miniCycle Duration
			ledPattern.totalMiniCycles = 0x0; // 2 miniCycle
			ledPattern.totalFullCycles = 0x0; // Repeat Forever
			ledPattern.startIntensity = 0xF; // 100% LED Intensity
			// Mini Cycle Data
			ledPattern.miniCycles[0].ledIntensity = 0xF; // 100% LED Intensity
            ledPattern.miniCycles[0].transitionSteps = 0x1; // 1 Steps - Instant
            ledPattern.miniCycles[0].finalStepDuration = 0x0; // 12.5ms Final Step Duration
		}
		
		if (kDown & HidNpadButton_B) { // Breathing LED
			memset(&ledPattern, 0, sizeof(ledPattern));
            // Main Pattern Data
            ledPattern.baseMiniCycleDuration = 0x4; // 50ms miniCycle Duration
            ledPattern.totalMiniCycles = 0x2; // 3 miniCycle
            ledPattern.totalFullCycles = 0x0; // Repeat Forever
            ledPattern.startIntensity = 0x2; // 12.5% LED Intensity
			// Mini Cycle Data
            ledPattern.miniCycles[0].ledIntensity = 0xF; // 100% LED Intensity
            ledPattern.miniCycles[0].transitionSteps = 0xF; // 15 Steps - 750ms
            ledPattern.miniCycles[0].finalStepDuration = 0x0; // 12.5ms Final Step Duration
            ledPattern.miniCycles[1].ledIntensity = 0x2; // 12.5% LED Intensity
            ledPattern.miniCycles[1].transitionSteps = 0xF; // 15 Steps - 750ms
            ledPattern.miniCycles[1].finalStepDuration = 0x0; // 12.5ms Final Step Duration
		}
		
		if (kDown & HidNpadButton_Y) // Disable LED
			memset(&ledPattern, 0, sizeof(ledPattern));
		
		if (kDown & (HidNpadButton_A | HidNpadButton_B | HidNpadButton_Y)) { // Update LED on Keypress
			for (u8 i = 0; i < handheldPadIdCnt; i++)
				hidsysSetNotificationLedPattern(&ledPattern, handheldPadIds[i]);
			for (u8 i = 0; i < npadOnePadIdCnt; i++)
				hidsysSetNotificationLedPattern(&ledPattern, npadOnePadIds[i]);
		}
		
		if (kDown & HidNpadButton_Plus) { // Disable LED and Exit Application
			memset(&ledPattern, 0, sizeof(ledPattern));
			for (u8 i = 0; i < handheldPadIdCnt; i++)
				hidsysSetNotificationLedPattern(&ledPattern, handheldPadIds[i]);
			for (u8 i = 0; i < npadOnePadIdCnt; i++)
				hidsysSetNotificationLedPattern(&ledPattern, npadOnePadIds[i]);
			break;
		}
    }
	
	hidsysExit();
	consoleExit(NULL);
    return 0;
}