int VoltageRead(int _pin, float &_iValue){
  unsigned long intVoltRead=0;
  int zecimala=0;    
  
  for(int i=0; i<=254; i++){
    intVoltRead=intVoltRead+analogRead(VOLT_PIN);
    delay(1);
  }
  intVoltRead=intVoltRead/255;
  _iValue=(155.7*intVoltRead/1024.0*1.1)/10.0;
  
  zecimala=_iValue*10.0- (int)_iValue*10;
  return CustomCharZecimala(zecimala);
}

int CustomCharZecimala( int _value){
  switch(abs(_value)){
    case 0: {return 7; break;}
    case 1: {return 7; break;}
    case 2: {return 1; break;}
    case 3: {return 1; break;}
    case 4: {return 2; break;}
    case 5: {return 2; break;}
    case 6: {return 3; break;}
    case 7: {return 3; break;}
    case 8: {return 4; break;}
    case 9: {return 4; break;}
    default: break;
  }
}

int DS18x20_Read_Tmp(byte addr[8], int &_temperatura){
  byte present = 0;
  float celsius =0.0;
  int zecimala=0;
  byte data[12];
  byte type_s;
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);                      // start conversion, with parasite power on at the end
  delay(1000);  
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);                       // Read Scratchpad
  
  for ( int i = 0; i < 9; i++)              // we need 9 bytes
      data[i] = ds.read();
 
  int16_t raw = (data[1] << 8) | data[0];
  
  byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  
  celsius = raw / 16.0;
  zecimala=celsius*10.0-(int)celsius*10;
  _temperatura=(int)celsius;
  return CustomCharZecimala(zecimala); 
}

void secundarBlink(){
  if(secundar_blink){
    lcd.setCursor(13,0);
    lcd.print(":");
  }
  else{
    lcd.setCursor(13,0);
    lcd.print(" ");
  }
  secundar_blink=!secundar_blink;  
}

void buton1(){
//  last_micros_button=micros();
  if(((long)(micros()-last_micros_button))>=debouncing_time){
    ibuton1++;
    bbuton1_change=true;
    lcd_clear_flag=true;
    last_micros_button=micros();  
  }
  if(ibuton1>4)
    ibuton1=0;
}

void buton2(){
//  last_micros_button=micros();
  if(((long)(micros()-last_micros_button))>=debouncing_time){
    bbuton2=true;
    last_micros_button=micros();   
  }  
}

void lcdClear(){
  if(lcd_clear_flag)
    lcd.clear();
  lcd_clear_flag=false;
  strcpy(linie1,"                ");
  strcpy(linie2,"                ");   
}  
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
void Reset(){
 asm volatile ("  jmp 0");
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//



