#include <EEPROM.h>

void write_long_EEPROM(int addr, unsigned long val)
{
  EEPROM.write(addr, val );
  EEPROM.write(addr + 1, (val >> 8) );
  EEPROM.write(addr + 2, (val >> 16) );
  EEPROM.write(addr + 3, (val >> 24) );

}

unsigned long read_long_EEPROM(int address)
{
  unsigned long val = 0;
  val = (unsigned long)EEPROM.read(address) & 255 ;
  val = val + ((unsigned long)(EEPROM.read(address + 1) & 255) << 8);
  val = val + ((unsigned long)(EEPROM.read(address + 2) & 255) << 16);
  val = val + ((unsigned long)(EEPROM.read(address + 3) & 255) << 24);
  return val;
}
