#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <switch.h>
#include "main.h"

void printLocalVersionList(int*, NsApplicationContentMetaStatus**, int);

int main(int argc, char **argv)
{
	/* List of Installed Titles */
	int recordsEntry = 0;
	int recordsLength = 0;
	NsApplicationRecord* titleRecords;
	
	/* Meta Status Lists for Installed Titles */
	int* metaLength;
	NsApplicationContentMetaStatus** metaStatusList;
	
	/* Init titleRecords and metaStatusList */
	initLists(&titleRecords, &recordsLength, &metaStatusList, &metaLength);
	
	/* Meta Status List of Current Title */
	NsApplicationContentMetaStatus* currMeta = *(metaStatusList);
	
	/* Name of Current Title */
	char* titleName;
	titleName = malloc(sizeof(char) * 0x201);
	updateTitleName(&titleName, titleRecords[recordsEntry].application_id);
	
	consoleInit(NULL);
	// Main loop
	while(appletMainLoop())
	{
		
		/* Print out current title info */
		printf("Index: %.3d of %.3d\n\n", recordsEntry+1, recordsLength);
		printf("Name:  %s\n\n", titleName);
		printf("Title ID: %.16lX\n\n", titleRecords[recordsEntry].application_id);
		printf("Meta IDs:\n");
		int i = 0;
		while ((i < 10) && (i < *(metaLength+recordsEntry))) {
			printMetaInfo(currMeta, i);
			i++;
		}
		if (i == 10) {
			int remainder = *(metaLength+recordsEntry) - 10;
			if (remainder > 0) {
				printf("          ... %d more entries ...\n", remainder);
			}
		}
		
		int controllerState = updateController(&recordsEntry, recordsLength);
		if (controllerState == 0) break; /* Quit Application */
		else if (controllerState == 1) /* We had input, update stuff */
		{
			currMeta = *(metaStatusList+recordsEntry);
			updateTitleName(&titleName, titleRecords[recordsEntry].application_id);
		}
		else if (controllerState == 2) /* We were asked to print out a local version-list */
		{
			printLocalVersionList(metaLength, metaStatusList, recordsLength);
			currMeta = *(metaStatusList+recordsEntry);
			
		}
		consoleUpdate(NULL);
		consoleClear();
	}
	
	free(titleRecords);
	free(metaLength);
	free(metaStatusList);
	free(titleName);
	consoleExit(NULL);
	return 0;
}

void printLocalVersionList(int *metaLength, NsApplicationContentMetaStatus **metaStatusList, int recordsLength)
{
	FILE *listFile;
	listFile = fopen("/Local-Version-List.txt", "w");
	
	int index = 0;
	NsApplicationContentMetaStatus* currMeta;
	
	fprintf(listFile,"id|version\n");
	while (index < recordsLength)
	{
		currMeta = *(metaStatusList+index);
		for (int i = 0; i < *(metaLength+index); i++)
		{
			/* Print Title ID */
			fprintf(listFile,"%.16lX|", currMeta[i].application_id);
			/* Print Version */
			fprintf(listFile,"%d\n", currMeta[i].version);
		}
		index++;
	}
	fclose(listFile);
	return;
}
