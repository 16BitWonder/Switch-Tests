#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum ledState { Off, Breathing, Solid };

int main(int argc, char **argv) {
	
	consoleInit(NULL);
	
	padConfigureInput(1, HidNpadStyleSet_NpadStandard);
	PadState mainPad;
	padInitializeDefault(&mainPad);
	
	psmInitialize(); // Init psm
	
	PsmChargerType chargerState = PsmChargerType_Unconnected;
	psmGetChargerType(&chargerState); // Get Initial Charger Type
	PsmChargerType prevChargerState = chargerState;
	u32 systemCharge = 0;
	u8 led = (chargerState != PsmChargerType_Unconnected) ? Breathing : Off; // Set LED based on initial charger state
	bool ledOn = false;
	
	hidsysInitialize(); // Init hidsys
	
	HidsysNotificationLedPattern ledPattern;
	
	HidsysUniquePadId handheldPadIds[2], npadOnePadIds[2];
	s32 handheldPadIdCnt = 0, npadOnePadIdCnt = 0;
	hidsysGetUniquePadsFromNpad(HidNpadIdType_Handheld, handheldPadIds, 2, &handheldPadIdCnt); // Get Pad IDs for Handheld
	hidsysGetUniquePadsFromNpad(HidNpadIdType_No1, npadOnePadIds, 2, &npadOnePadIdCnt); // Get Pad IDs for P1
	
	while(appletMainLoop()) {
		
		psmGetChargerType(&chargerState); // Update chargerState
		psmGetBatteryChargePercentage(&systemCharge); // Update systemCharge
		
		if (chargerState != prevChargerState) { // Update led if Charger State Changed
			if (chargerState != PsmChargerType_Unconnected) { // Charger Connected
				led = Breathing;
			} else { // Charger Disconnected
				led = Off;
				ledOn = false;
				memset(&ledPattern, 0, sizeof(ledPattern));
				for (u8 i = 0; i < handheldPadIdCnt; i++) // Update handheld LED
					hidsysSetNotificationLedPattern(&ledPattern, handheldPadIds[i]);
				for (u8 i = 0; i < npadOnePadIdCnt; i++) // Update P1 LED
					hidsysSetNotificationLedPattern(&ledPattern, npadOnePadIds[i]);
			}
			prevChargerState = chargerState;
		}
		
		if (led != Off) { // Update LED while charging
			if (led == Breathing) {
				if (!ledOn) {
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
					
					for (u8 i = 0; i < handheldPadIdCnt; i++) // Update handheld LED
						hidsysSetNotificationLedPattern(&ledPattern, handheldPadIds[i]);
					for (u8 i = 0; i < npadOnePadIdCnt; i++) // Update P1 LED
						hidsysSetNotificationLedPattern(&ledPattern, npadOnePadIds[i]);
						
					ledOn = true;
				} else if (systemCharge > 94) { // Update to Solid LED if above 94%
					led = Solid;
					ledOn = false;
				}
			} else if (led == Solid) {
				if (!ledOn) {
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
					
					for (u8 i = 0; i < handheldPadIdCnt; i++) // Update handheld LED
						hidsysSetNotificationLedPattern(&ledPattern, handheldPadIds[i]);
					for (u8 i = 0; i < npadOnePadIdCnt; i++) // Update P1 LED
						hidsysSetNotificationLedPattern(&ledPattern, npadOnePadIds[i]);
						
					ledOn = true;
				} else if (systemCharge < 95) { // Update to Breathing LED if below 95%
					led = Breathing;
					ledOn = false;
				}
			}
		}
		
		printf("\n LED Charging Test\n\n");
		printf(" Not Charging  = Disabled LED\n");
		printf(" Charging	   = Breathing LED\n");
		printf(" Charging >95%% = Solid LED\n\n");
		printf(" Charging: ");
		printf("%s\n", (chargerState != PsmChargerType_Unconnected) ? "True" : "False");
		printf(" System Charge: %d%%\n\n", systemCharge);
		printf(" led: %d\n", led);
		printf(" ledOn: %s\n\n", ledOn ? "True" : "False");
		printf(" (+) Exit Application");
		consoleUpdate(NULL);
		consoleClear();
		
		padUpdate(&mainPad);
		u64 kDown = padGetButtonsDown(&mainPad);
		
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
	psmExit();
	consoleExit(NULL);
	return 0;
}