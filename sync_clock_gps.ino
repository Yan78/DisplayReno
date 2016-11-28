//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
void changeHourSummerDaysRO(bool gps_sync){
  sDateTime sDTOut;
  if(checkIfSummerHour() == true ){
   Serial.println("\r\nSummer Hour");
    if(checkSummerHourIsSaved() == 0){
      clockOffSet=3;     
      //functie pentru set clock cu offset
      sDTOut=localDateTimeRo(sDT1,clockOffSet);
      changeDateTimeDS1307(sDTOut);
      summerHourIsSaved();
      //Reset();
    }
    else{
      clockOffSet=3;
      if(gps_sync){
        sDTOut=localDateTimeRo(sDT1,clockOffSet);
        changeDateTimeDS1307(sDTOut);        
      }
      summerHourIsSaved();  
    }     
  }
  else{
    Serial.println("\r\nWinter Hour");
    if(checkSummerHourIsSaved() == 1 ){
      clockOffSet=2;    
      //functie pentru set clock cu offset
      sDTOut=localDateTimeRo(sDT1,clockOffSet);
      changeDateTimeDS1307(sDTOut);      
      winterHourIsSaved();
      //Reset();     
    }
    else{
      clockOffSet=2;
      if(gps_sync){
        sDTOut=localDateTimeRo(sDT1,clockOffSet);
        changeDateTimeDS1307(sDTOut);  
      } 
      winterHourIsSaved();
    } 
  }
//Serial.println(clockOffSet);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
bool checkIfSummerHour(){
  int MonthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
  int summerHourDayStart, summerHourDayEnd, currentDayOfYear=0;
 
// calculeaza numarul de zile la data curenta in an non bisect 
  for(int i=1;i<=sDT1.tMonth-1;i++){
    currentDayOfYear=currentDayOfYear+MonthDays[i-1];
  }
  currentDayOfYear=currentDayOfYear + sDT1.tDay;
  
// calculeaza inceputul si sfarsitul orei de vara    
  if(leapYear(sDT1.tYear)){
    summerHourDayStart=31+29+lastSundayMarch(sDT1.tYear);
    summerHourDayEnd=31+29+31+30+31+30+31+31+30+lastSundayOctober(sDT1.tYear);
    if(sDT1.tMonth>2)
      currentDayOfYear=currentDayOfYear + 1;
  }
  else{
    summerHourDayStart=31+28+lastSundayMarch(sDT1.tYear);
    summerHourDayEnd=31+28+31+30+31+30+31+31+30+lastSundayOctober(sDT1.tYear);    
  }
//  Serial.println();Serial.print(currentDayOfYear);Serial.print("  ");Serial.print(summerHourDayStart);Serial.print("  ");Serial.print(summerHourDayEnd);
// verifica daca trebuie schimbata ora
  if(currentDayOfYear>summerHourDayStart && currentDayOfYear<summerHourDayEnd)
    return true; 
  if(currentDayOfYear<summerHourDayStart)
    return false;
  if(currentDayOfYear>summerHourDayEnd)
    return false;
  if(currentDayOfYear==summerHourDayStart){
    if(sDT1.tHours>=3)
      return true;   
    else
      return false;  
  }
  if(currentDayOfYear==summerHourDayEnd){
    if(sDT1.tHours>=4)
      return false;   
    else
      return true; 
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
struct sDateTime localDateTimeRo(sDateTime _sdt, int _corection){
  int MonthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
  sDateTime _sret;
  int wFirstDay2016=4;
  int i, temp=0;
  long lHours=0;
  //calcul numar de ore de la 01.01.2016
  for(i=2016;i<_sdt.tYear;i++)
  {
    if(leapYear(i))
      lHours=lHours + 366*24;
    else
      lHours=lHours + 365*24;
  }
  if(leapYear(_sdt.tYear))
     MonthDays[1]=29;
  for(i=1; i<_sdt.tMonth; i++)
     lHours=lHours + MonthDays[i-1]*24;
  lHours=lHours + _sdt.tDay*24;
  //aplica corectia pentru ora locala
  lHours=lHours + _sdt.tHours + _corection;
  //calcul numar de zile dupa corectie
  temp=(int)(lHours/24);
  i=0;
  while(temp>0){
    leapYear(2016+i) ? temp=temp-366 : temp=temp-365; 
    i++;   
  }
  //i-1 numarul de ani dupa 2016
  leapYear(2016+i-1) ? temp=temp+366: temp=temp+365;
  leapYear(2016+i-1) ? MonthDays[1]=29: MonthDays[1]=28;
  _sret.tYear=2016+i-1;
  //calcul luna dupa corectie
  for(i=0; i<=11 && temp-MonthDays[i]>0 ;i++)
    temp=temp-MonthDays[i];
  //calcul ore dupa corectie
  _sret.tDay=temp;
  _sret.tMonth=i+1;       
  _sret.wDay=((((int)(lHours/24)))%7+wFirstDay2016-1)%7;
  _sret.tHours=lHours%24;
  _sret.tMinutes=_sdt.tMinutes;
  _sret.tSeconds=_sdt.tSeconds;
  return _sret;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//  
bool leapYear(int _y)
{
  if(( _y % 4 == 0 && _y % 100 != 0 ) || _y % 400 == 0 )
    return true;
  else
    return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
int lastSundayMarch(int _tYear){
  struct sDateTime Si, So;
  int firstSundayYear=0;
  int first3MonthDays=0;
  Si.tSeconds=00; Si.tMinutes=00; Si.tHours=01;
  Si.tDay=1; Si.tMonth=1; Si.tYear=_tYear; Si.wDay=6;

  So=localDateTimeRo(Si,0);
  firstSundayYear=So.tDay+6-So.wDay;
  leapYear(_tYear) ? first3MonthDays=31+29+31 : first3MonthDays=31+28+31 ;
  return 31-(first3MonthDays-firstSundayYear)%7;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
int lastSundayOctober(int _tYear){
  struct sDateTime Si, So;
  int firstSundayYear=0;
  int first10MonthDays=0;
  Si.tSeconds=00; Si.tMinutes=00; Si.tHours=01;
  Si.tDay=1; Si.tMonth=1; Si.tYear=_tYear; Si.wDay=6;

  So=localDateTimeRo(Si,0);
  firstSundayYear=So.tDay+6-So.wDay;
  leapYear(_tYear) ? first10MonthDays=366-31-30 : first10MonthDays=365-31-30 ;
  return 31-(first10MonthDays-firstSundayYear)%7;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
void lockGpsGetTime()
{
  unsigned long gpslockmillis = 0;
    do{
    feedgps();
    if(millis()-gpslockmillis >3000){
        // functie lcd debug gps Set_Clock_Windows.println(String(F("Sat lock: "))+String(gps.satellites.value())+ String(F(" received chars: "))+String(gps.charsProcessed()),LEFT,true);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(gps.charsProcessed());
      gpslockmillis=millis();
      Serial.println(gps.charsProcessed());
    }
  }
  while(!gps.location.isValid() || !gps.date.isValid() || !gps.time.isValid() || gps.date.year()<=2015);
  Serial.println("gps sync");
  // functie lcd debug gps lock time date
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
bool feedgps(){  
  while (mySerial.available()>0){
    if (gps.encode(mySerial.read()))
      return true;  
  }
  return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
struct sDateTime getDateTimeFromGps(){
  struct sDateTime _sret;
  
  _sret.tSeconds=gps.time.second(); _sret.tMinutes=gps.time.minute(); _sret.tHours=gps.time.hour();
  _sret.tDay=gps.date.day(); _sret.tMonth=gps.date.month(); _sret.tYear=gps.date.year(); _sret.wDay=1;

  return _sret;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
void syncDS1307WithGpsSummerDay(){
  lockGpsGetTime();
  sDT1=getDateTimeFromGps();
  changeHourSummerDaysRO(true);
  Serial.println("done");
}
