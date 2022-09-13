#include "main.h"

int monthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
extern TimeCalendarTime newCalTime;

bool updateControls(int *selection, u64 *rawTime, PadState *pad)
{
	
	int leapYear;
	int targetMonth;
	int targetMonthDays;
	int daysToAdd;
	int daysToSub;
	
	padUpdate(pad);
	u64 kDown = padGetButtonsDown(pad);
	
	if (kDown & KEY_PLUS) return true;
	
	if ((kDown & KEY_DLEFT) || (kDown & KEY_LSTICK_LEFT) || (kDown & KEY_RSTICK_LEFT))
	{
		*selection = *selection - 1;
		if (*selection < 0)
			*selection = 5;
	}
	if ((kDown & KEY_DRIGHT) || (kDown & KEY_LSTICK_RIGHT) || (kDown & KEY_RSTICK_RIGHT))
	{
		*selection = *selection + 1;
		if (*selection > 5)
			*selection = 0;
	}
	if ((kDown & KEY_DUP) || (kDown & KEY_LSTICK_UP) || (kDown & KEY_RSTICK_UP))
	{
		switch (*selection)
		{
			case (0):
				// In leap year add 366 days
				// In normal year add 365
				leapYear = (newCalTime.year+1) % 4;
				if (leapYear != 0){
					*rawTime = *rawTime + 31536000;
				} else {
					*rawTime = *rawTime + 31622400;
				}
				break;
			case (1):
				leapYear = newCalTime.year % 4;
				targetMonth = newCalTime.month + 1;
				if (targetMonth == 13){
					targetMonth = 1;
				}
				targetMonthDays = monthDays[targetMonth-1];
				if ((leapYear == 0) && (targetMonth == 2)){
					targetMonthDays++;
				}
				
				if (newCalTime.day > targetMonthDays){
					// Get to the end of this month
					daysToAdd = monthDays[newCalTime.month-1] - newCalTime.day;
					if ((leapYear == 0) && (newCalTime.month == 2)){
						daysToAdd++;
					}
					*rawTime = *rawTime + (86400 * daysToAdd);
					
					// Now add the size of the next month
					*rawTime = *rawTime + (86400 * targetMonthDays);
				} else {
					// Just add the size of this month
					daysToAdd = monthDays[newCalTime.month-1];
					if ((leapYear == 0) && (newCalTime.month == 2)){
						daysToAdd++;
					}
					*rawTime = *rawTime + (86400 * daysToAdd);
				}
				break;
			case (2):
				*rawTime = *rawTime + 86400;
				break;
			case (3):
				*rawTime = *rawTime + 3600;
				break;
			case (4):
				*rawTime = *rawTime + 60;
				break;
			case (5):
				*rawTime = *rawTime + 1;
				break;
			default:
				break;
		}
	}
	if ((kDown & KEY_DDOWN) || (kDown & KEY_LSTICK_DOWN) || (kDown & KEY_RSTICK_DOWN))
	{
		switch (*selection)
		{
			case (0):
				// In leap year sub 366 days
				// In normal year sub 365
				leapYear = (newCalTime.year-1) % 4;
				if (leapYear != 0){
					*rawTime = *rawTime - 31536000;
				} else {
					*rawTime = *rawTime - 31622400;
				}
				break;
			case (1):			
				leapYear = newCalTime.year % 4;
				targetMonth = newCalTime.month - 1;
				if (targetMonth < 1){
					targetMonth = 12;
				}
				targetMonthDays = monthDays[targetMonth-1];
				if ((leapYear == 0) && (targetMonth == 2)){
					targetMonthDays++;
				}
				
				if (newCalTime.day > targetMonthDays){
					// Get to the end of the target month
					daysToSub = newCalTime.day;
					*rawTime = *rawTime - (86400 * daysToSub);
				} else {
					// Sub to the same day
					daysToSub = newCalTime.day +(targetMonthDays-newCalTime.day);
					if ((leapYear == 0) && (newCalTime.month == 2)){
						daysToSub++;
					}
					*rawTime = *rawTime - (86400 * daysToSub);
				}
				break;
			case (2):
				*rawTime = *rawTime - 86400;
				break;
			case (3):
				*rawTime = *rawTime - 3600;
				break;
			case (4):
				*rawTime = *rawTime - 60;
				break;
			case (5):
				*rawTime = *rawTime - 1;
				break;
			default:
				break;
		}
	}
	if (kDown & KEY_Y)
	{
		timeSetCurrentTime(TimeType_NetworkSystemClock, *rawTime);
	}
	
	return false;
}