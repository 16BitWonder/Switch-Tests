#include "main.h"

int updateController(int* entry, int length)
{	
	hidScanInput();
    u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
	u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
	
	/* Exit application if plus is pressed */
	if (kDown & KEY_PLUS) return 0;
	
	int state = -1;
	
	switch (kDown)
	{
		case (KEY_DUP):
		*entry += 1;
		if (*entry == length)
			*entry = 0;
		state = 1;
		break;
		
		case (KEY_DDOWN):
		*entry -= 1;
		if (*entry == -1)
			*entry = length - 1;
		state = 1;
		break;
		
		case (KEY_DLEFT):
		*entry -= 10;
		if (*entry < 0)
			*entry = length + *entry;
		state = 1;
		break;
		
		case (KEY_DRIGHT):
		*entry += 10;
		if (*entry > (length - 1))
			*entry = 0 + (*entry - length);
		state = 1;
		break;
		
		case (KEY_Y):
		state = 2;
		break;
		
		default:
		break;
	}
	
	switch (kHeld)
	{
		case (KEY_LSTICK_UP):
		*entry += 1;
		if (*entry == length)
			*entry = 0;
		state = 1;
		break;
		
		case (KEY_LSTICK_DOWN):
		*entry -= 1;
		if (*entry == -1)
			*entry = length - 1;
		state = 1;
		break;
		
		case (KEY_LSTICK_LEFT):
		*entry -= 10;
		if (*entry < 0)
			*entry = length + *entry;
		state = 1;
		break;
		
		case (KEY_LSTICK_RIGHT):
		*entry += 10;
		if (*entry > (length - 1))
			*entry = 0 + (*entry - length);
		state = 1;
		break;
		
		default:
		break;
	}

	return state;
}