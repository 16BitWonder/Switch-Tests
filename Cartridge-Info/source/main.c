#include "main.h"

PadState pad;

bool gameCardHasBundledUpdate();
Result getGameCardApplicationControl(NsApplicationControlData **gcControl, u64 application_id);
Result getGameCardApplicationIDAndVersion(u64 *application_id, s32 *version);
Result getGameCardMetaKey(NcmApplicationContentMetaKey *metaKey, NcmContentMetaType metaType);
int getGameCardBundledFirmware();

int main(int argc, char **argv) {
	
	ncmInitialize();
	nsInitialize();
	consoleInit(NULL);
	
	padConfigureInput(1, HidNpadStyleSet_NpadStandard);
	padInitializeDefault(&pad);
	
	bool cardInserted = false;
	NsApplicationControlData *gcControl = malloc(sizeof(NsApplicationControlData));
	u64 tid = 0;
	s32 version = 0;
	u32 fw_version = 0;
	
	// Check if a cart is inserted
	nsIsGameCardInserted(&cardInserted);
	
	if (cardInserted) {
		
		// Get game card title ID and version
		getGameCardApplicationIDAndVersion(&tid, &version);
		
		// Get game card control
		getGameCardApplicationControl(&gcControl, tid);
		
		// Get bundled FW version
		fw_version = getGameCardBundledFirmware();
		
	}
	
	// Print if cart was inserted
	printf("\n\n  Cartridge Inserted: ");
	if (cardInserted) printf("True\n\n");
	else printf("False\n\n");
	
	// Print cart title
	printf("  Title: ");
	if (cardInserted) printf("%s\n\n", gcControl->nacp.lang->name);
	else printf("N/A\n\n");
	
	// Print cart author
	printf("  Author: ");
	if (cardInserted) printf("%s\n\n", gcControl->nacp.lang->author);
	else printf("N/A\n\n");
	free(gcControl);
	
	// Print cart title
	printf("  Title ID: ");
	if (cardInserted) printf("%.16lX\n\n", tid);
	else printf("N/A\n\n");
	
	// Print cart version
	printf("  Version: ");
	if (cardInserted) printf("v%d\n\n", version);
	else printf("N/A\n\n");
	
	// Print bundled FW version
	printf("  Bundled Firmware: ");
	if (cardInserted) printf("%d.%d.%d.%.4d (v%d)\n\n", (fw_version>>26)&0x3F, (fw_version>>20)&0x3F, (fw_version>>16)&0xF, fw_version&0xFFFF, fw_version);
	else printf("N/A\n\n");
	
	printf("  Press (+) to exit");
	
    while(appletMainLoop()) {
	
		padUpdate(&pad);
		u64 kDown = padGetButtonsDown(&pad);
		if (kDown & HidNpadButton_Plus) break;
		consoleUpdate(NULL);
    }

	consoleExit(NULL);
	ncmExit();
	nsExit();
    return 0;
}

// Returns true if a game card has a bundled update, false otherwise
bool gameCardHasBundledUpdate() {
	
	NcmContentMetaDatabase db;
	NcmApplicationContentMetaKey tmpMetaKey;
	s32 total = 0, written = 0;
	ncmOpenContentMetaDatabase(&db, NcmStorageId_GameCard);
	
	// Grab the application meta key
	ncmContentMetaDatabaseListApplication(&db, &total, &written, &tmpMetaKey, 1, NcmContentMetaType_Patch);
	
	// Return true if an update was found
	return total != 0;
}

// Returns game card NsApplicationControlData
Result getGameCardApplicationControl(NsApplicationControlData **gcControl, u64 application_id) {
	
	Result rc;
	
	u64 length = 0;
	NsApplicationControlData *titleControl = *gcControl;
	rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, application_id, titleControl, sizeof(NsApplicationControlData), &length);
	
	return rc;
}

// Returns the game cards version number and base title ID
Result getGameCardApplicationIDAndVersion(u64 *application_id, s32 *version) {
	
	NcmApplicationContentMetaKey baseMetaKey;
	Result rc_baseMetaKey = getGameCardMetaKey(&baseMetaKey, NcmContentMetaType_Application);
	
	if (gameCardHasBundledUpdate()) {
		
		NcmApplicationContentMetaKey updateMetaKey;
		Result rc_updateMetaKey = getGameCardMetaKey(&updateMetaKey, NcmContentMetaType_Patch);
		
		// Stop if we failed to retrieve either key
		if (R_FAILED(rc_baseMetaKey) || R_FAILED(rc_updateMetaKey)) return -1;
		
		// Figure out the largest version number between base/update and return it
		s32 tmpVersion = updateMetaKey.key.version;
		if (tmpVersion < baseMetaKey.key.version) tmpVersion = baseMetaKey.key.version;
		*version = tmpVersion;
	} else {
		
		// Stop if we failed to retrieve base key
		if (R_FAILED(rc_baseMetaKey)) return -1;
		
		// Return version
		*version = baseMetaKey.key.version;
	}
	
	// Return title ID
	*application_id = baseMetaKey.application_id;
	
	return 0;
}

// Returns a NcmApplicationContentMetaKey for the defined NcmContentMetaType
Result getGameCardMetaKey(NcmApplicationContentMetaKey *metaKey, NcmContentMetaType metaType) {
	
	NcmContentMetaDatabase db;
	NcmApplicationContentMetaKey tmpMetaKey;
	s32 total = 0, written = 0;
	Result rc = ncmOpenContentMetaDatabase(&db, NcmStorageId_GameCard);
	
	// Grab the application meta key
	rc = ncmContentMetaDatabaseListApplication(&db, &total, &written, &tmpMetaKey, 1, metaType);
	
	// Return meta key
	*metaKey = tmpMetaKey;
	
	ncmContentMetaDatabaseClose(&db);
	return rc;
}

// Returns the bundled FW version in the inserted game card
int getGameCardBundledFirmware() {
	
	FsDeviceOperator cardOperator;
	fsOpenDeviceOperator(&cardOperator);
	FsGameCardHandle cartHandle;
	fsDeviceOperatorGetGameCardHandle(&cardOperator, &cartHandle);
		
	struct {
		u32 ret_fw_version;
		u64 ret_tid;
	} ret;
		
	// Call GetGameCardUpdatePartitionInfo and store ret
	serviceDispatchInOut(&cardOperator.s, 203, cartHandle.value, ret);
	
	// Close operator
	fsDeviceOperatorClose(&cardOperator);
	
	// Return aquired FW version
	return ret.ret_fw_version;
}
