#include "main.h"

void printMetaInfo(NsApplicationContentMetaStatus* currMeta, int i)
{
	/* Identify as a DLC, Update or Base */
	switch (currMeta[i].meta_type)
	{
		case (NcmContentMetaType_Application):
		printf("          [APP] ");
		break;
		
		case (NcmContentMetaType_Patch):
		printf("          [UPD] ");
		break;
		
		case (NcmContentMetaType_AddOnContent):
		printf("          [DLC] ");
		break;
		
		default:
		break;
	}
	/* Print Title ID */
	printf("[%.16lX] ", currMeta[i].application_id);
	/* Print Version */
	printf("[v%d]\n", currMeta[i].version);
	return;
}