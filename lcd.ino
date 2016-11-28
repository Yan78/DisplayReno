void lcdSetContrast(int &_contrast_value){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(String(F("    Contrast    ")));
  for(int i=0; i<=15; i++){
    lcd.setCursor(i,1);
    lcd.write(61);
  }
  if(bbuton2){
    _contrast_value=_contrast_value+5;
    bbuton2=false;
  }
  if(_contrast_value>=80)
    _contrast_value=0; 
  analogWrite(LCD_CONTRAST, 80-_contrast_value);

  lcd.setCursor((_contrast_value/5)%16,1);
  lcd.write(17);
  while(!bbuton2 && !bbuton1_change) 
    delay(100);
  bbuton1_change=false;
  lcd.clear(); 

}
void lcdMeniuSetContrast(){
  lcdSetContrast(icontrast);
}

void lcdMeniuOraData(){
  
  if(!bSetClock){
    lcdPrintDateTime();
  }
  delay(50);
  if(bbuton2){
    // setare ceas
    bSetClock=true;
    bbuton2=false;
    setDateClock();
  }
}

void lcdMeniuTemp3_Update_Voltage(){
  int zecimala=VoltageRead(VOLT_PIN, iVoltage);
  lcd.setCursor(0,0);
  lcd.print("     ");
  lcd.setCursor(0,0);
  lcd.print((int)iVoltage); lcd.write((byte)zecimala); lcd.print("V ");
  delay(50);
}

void lcdMeniuTemp3(){
  lcdClear();
  int zecimala=VoltageRead(VOLT_PIN, iVoltage); 
  if(!bbuton1_change)
    izec1=DS18x20_Read_Tmp(TEMP1, itemp1);  
  
  lcd.setCursor(0,0); 
  lcd.print((int)iVoltage); lcd.write((byte)zecimala); lcd.print("V ");
  lcd.setCursor(5,0);
  itemp1>=0 ? lcd.print(" ") : lcd.print("-");
  lcd.print(abs(itemp1)); lcd.write((byte)izec1); lcd.write(5); lcd.print(" ");
  lcd.setCursor(11,0);
  lcd.print(getShortTimeDs1307()); 

  secundarBlink();
  if(!bbuton1_change)
    izec2=DS18x20_Read_Tmp(TEMP2, itemp2);
  secundarBlink();
  if(!bbuton1_change)
    izec3=DS18x20_Read_Tmp(TEMP3, itemp3);
  secundarBlink();
  if(!bbuton1_change)
    izec4=DS18x20_Read_Tmp(TEMP4, itemp4);  
  secundarBlink();  
  
  lcd.setCursor(0,1);
  itemp2>=0 ? lcd.print("+") : lcd.print("-");
  lcd.print(abs(itemp2)); lcd.write((byte)izec2); lcd.print(" ");
  lcd.setCursor(5,1);
  itemp3>=0 ? lcd.print(" ") : lcd.print("-");
  lcd.print(abs(itemp3)); lcd.write((byte)izec3); lcd.write(5); lcd.print(" ");
  lcd.setCursor(11,1);
  itemp4>=0 ? lcd.print(" ") : lcd.print("-");
  lcd.print(abs(itemp4)); lcd.write((byte)izec4); lcd.write(5); lcd.print(" ");
   
  bbuton1_change=false;
  bbuton2=false;
  last_millis_obd=0;
}

void lcdMeniuOBD2(){
  lcdClear();
  if(!bbuton1_change){
    Elm327_getVoltage(volt);
    VoltageRead(VOLT_PIN, iVoltage);
  }
  sprintf(linie1,"%c%02d.%02uV  %c%02d.%02uV", 127, (byte)volt, (byte)(volt*100.00-((byte)volt*100)), 6, (byte)iVoltage, (byte)(iVoltage*100.00-((byte)iVoltage*100)));
  lcd.setCursor(0,0);
  lcd.print(linie1);
  
  while((((long)(millis()-last_millis_obd))>=5000) || last_millis_obd==0 && elm327_installed==ELM_SUCCESS){
    if(!bbuton1_change)
      Elm327_coolantTemperature(coolant);
    if(!bbuton1_change)
      Elm327_intakeAirTemperature(intake_temp);
    if(!bbuton1_change)
      Elm327_intakeManifoldAbsolutePressure(pressure);
    bbuton1_change=false;
    last_millis_obd=millis(); 
  }
  if(elm327_installed==ELM_SUCCESS){
    sprintf(linie2,"%c% 02d%c %c% 02d%c %c%3d", 207, coolant, 5, 246, intake_temp, 5, 255, pressure);
    lcd.setCursor(0,1);
    lcd.print(linie2);
  }
 }

