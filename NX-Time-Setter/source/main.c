#include "main.h"

TimeServiceType __nx_time_service_type = TimeServiceType_System;
TimeCalendarTime newCalTime;
TimeCalendarAdditionalInfo newInfo;

int main(int argc, char **argv)
{
	consoleInit(NULL);
	
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
	PadState pad;
    padInitializeDefault(&pad);
	
	bool networkSync;
	setsysInitialize();
	setsysIsUserSystemClockAutomaticCorrectionEnabled(&networkSync);
	setsysExit();
	if (!networkSync)
	{
		networkSyncNotEnabled(&pad);
		consoleExit(NULL);
		return 0;
	}
	
	TimeCalendarTime networkCalTime;
	TimeCalendarAdditionalInfo networkInfo;
	
	u64 rawNetworkTime;
	u64 rawNewTime;
	
	timeGetCurrentTime(TimeType_NetworkSystemClock, &rawNetworkTime);
	rawNewTime = rawNetworkTime;
	
	int selection = 0;

	// Main loop
	while(appletMainLoop())
	{
		// Update Current Time
		timeGetCurrentTime(TimeType_NetworkSystemClock, &rawNetworkTime);
		timeToCalendarTimeWithMyRule(rawNetworkTime, &networkCalTime, &networkInfo);
		timeToCalendarTimeWithMyRule(rawNewTime, &newCalTime, &newInfo);

		printf("\n\n  Network Time: %04i/%02i/%02i - %02i:%02i:%02i (%s)", networkCalTime.year, networkCalTime.month, networkCalTime.day, networkCalTime.hour, networkCalTime.minute, networkCalTime.second, networkInfo.timezoneName);
		printf("\n\n      New Time: %04i/%02i/%02i - %02i:%02i:%02i (%s)", newCalTime.year, newCalTime.month, newCalTime.day, newCalTime.hour, newCalTime.minute, newCalTime.second, newInfo.timezoneName);
		
		switch (selection)
		{
			case (0):
				printf("\x1b[6;17H^^^^");
				break;
			case (1):
				printf("\x1b[6;22H^^");
				break;
			case (2):
				printf("\x1b[6;25H^^");
				break;
			case (3):
				printf("\x1b[6;30H^^");
				break;
			case (4):
				printf("\x1b[6;33H^^");
				break;
			case (5):
				printf("\x1b[6;36H^^");
				break;
			default:
				break;
		}
		
		printf("\x1b[8;10H(+) Exit");
		printf("\x1b[10;10H(Y) Set Clock");
		
		// Handle Controls
		if (updateControls(&selection, &rawNewTime, &pad))
			break;

		consoleUpdate(NULL);
		consoleClear();
	}

	consoleExit(NULL);
	return 0;
}