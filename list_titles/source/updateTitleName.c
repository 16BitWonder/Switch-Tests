#include <string.h>
#include <stdlib.h>
#include "main.h"

void updateTitleName(char** titleName, u64 TID)
{
	nsInitialize();
	char* name = *titleName;
	memset(name, 0, sizeof(char) * 0x201);
	NacpLanguageEntry* nacpName;
	NsApplicationControlData* titleControl;
	titleControl = malloc(sizeof(NsApplicationControlData));
	u64 length = 0;
	nsGetApplicationControlData(NsApplicationControlSource_Storage, TID, titleControl, sizeof(NsApplicationControlData), &length);
	nacpGetLanguageEntry(&titleControl->nacp, &nacpName);
	strncpy(name, nacpName->name, 0x200);
	nsExit();
	return;
}