#define EEPROM_START 0x100 //the name will be put at this adress

void LoadEEPROM(char *pBuffer, char num_bytes, unsigned int EEPROM_START_ADR);
void StoreEEPROM(char *pBuffer, char num_bytes, unsigned int EEPROM_START_ADR);
