#include "main.h"

void networkSyncNotEnabled(PadState *pad)
{
	printf("\n\n  Synchronize Clock via Internet is not enabled!");
	printf("\n\n  To enable, go to:");
	printf("\n\tSystem Settings -> System -> Date and Time -> Synchronize Clock via Internet");
	printf("\n\n  Press (+) to exit");
	consoleUpdate(NULL);
	while (1)
	{
		padUpdate(pad);
		u64 kDown = padGetButtonsDown(pad);
		if (kDown & KEY_PLUS) return;
	}
}