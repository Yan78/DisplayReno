#define eeSUMMERHOURCHECK   0x10    // 1 byte

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
int checkSummerHourIsSaved(){
  return EEPROM.read(eeSUMMERHOURCHECK);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
void summerHourIsSaved(){
  EEPROM.write(eeSUMMERHOURCHECK, 1);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
void winterHourIsSaved(){
  EEPROM.write(eeSUMMERHOURCHECK, 0);
}

