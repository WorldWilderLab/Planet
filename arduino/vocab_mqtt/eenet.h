// eeprom contents

union eeconfig {
  struct {
    uint8_t gateway[4];
    uint8_t mask[4];
    uint8_t mac[6];
    uint8_t ip[4];
  } mem;
  uint8_t raw[];
} ee = {  {
  // EEPROM block starts here
  { 192,168,1,254 },                    // GWR Gateway IP Address Register
  { 255,255,255,0 },                    // SUBR Subnet Mask Register
  { 0x90,0xa2,0xda,0x00,0xf3,0x33} ,    // SHAR Source Hardware Address Register
  { 192,168,1,86 }                       // SIPR Source IP Address Register
}
  // EEPROM block ends here
}; 

#define EEPROM_SIG_1 ((uint8_t*)0)
#define EEPROM_SIG_2 ((uint8_t*)1)
#define EEPROM_IMG_STAT ((uint8_t*)2)
#define EEPROM_DATA ((uint8_t*)3)
#define EEPROM_SIG_1_VALUE (0x55)
#define EEPROM_SIG_2_VALUE (0xAA)
#define EEPROM_IMG_OK_VALUE (0xBB)
#define EEPROM_IMG_BAD_VALUE (0xFF)

#define EEPROM_GATEWAY ((uint8_t*)2)
#define EEPROM_MAC ((uint8_t*)6)
#define EEPROM_IP ((uint8_t*)12)

