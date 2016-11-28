#define DS1307_I2C_ADDRESS 0x68
char  *Day[] = {"","Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
//char  *Day[] = {"","Duminica","Luni","Marti","Miercuri","Joi","Vineri","Sambata"};
char  *Mon[] = {"xxx","ian","feb","mar","apr","mai","iun","iul","aug","sep","oct","nov","dec"};

struct sDateTime getDateTimeDS1307Struct(){
  struct sDateTime _sret;

  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
  
  _sret.tSeconds     = bcd_int_conv(Wire.read() & 0x7f);
  _sret.tMinutes     = bcd_int_conv(Wire.read());
  _sret.tHours       = bcd_int_conv(Wire.read() & 0x3f);  // Need to change this if 12 hour am/pm
  _sret.wDay         = bcd_int_conv(Wire.read());
  _sret.tDay         = bcd_int_conv(Wire.read());
  _sret.tMonth       = bcd_int_conv(Wire.read());
  _sret.tYear        = bcd_int_conv(Wire.read());

  return _sret;
}

char *getDateEEDs1307()
{
  byte  _dayW, _day, _month, _year=0;  
 char *buffer="   00-00-0000   ";
 Wire.beginTransmission(DS1307_I2C_ADDRESS);
 Wire.write(0x03);
 Wire.endTransmission();
 Wire.requestFrom(DS1307_I2C_ADDRESS, 4);

 _dayW       =bcd_int_conv(Wire.read());
 _day        =bcd_int_conv(Wire.read());
 _month      =bcd_int_conv(Wire.read());
 _year       =bcd_int_conv(Wire.read());
 
 sprintf(buffer,"%s %02d %s %04d ",Day[_dayW], _day, Mon[_month], _year+2000);
 return buffer;
}

char *getTimeDs1307()
{
 byte minute=0;
 byte seconds=0;
 char *buffer="    00:00:00    ";
 Wire.beginTransmission(DS1307_I2C_ADDRESS);
 Wire.write(0x00);
 Wire.endTransmission();
 Wire.requestFrom(DS1307_I2C_ADDRESS, 3);
 
 seconds=bcd_int_conv(Wire.read());
 minute= bcd_int_conv(Wire.read());
 sprintf(buffer,"    %02d:%02d:%02d    ", bcd_int_conv(Wire.read() & 0x3f), minute, seconds);
 return buffer;
}

char *getShortTimeDs1307()
{
 byte minute=0;
 byte seconds=0;
 char *buffer="00:00";
 Wire.beginTransmission(DS1307_I2C_ADDRESS);
 Wire.write(0x00);
 Wire.endTransmission();
 Wire.requestFrom(DS1307_I2C_ADDRESS, 3);
 
 seconds=bcd_int_conv(Wire.read());
 minute= bcd_int_conv(Wire.read());
 sprintf(buffer,"%02d %02d", bcd_int_conv(Wire.read() & 0x3f), minute);
 return buffer;
}

void setDateClock(){
  int old_bbuton1=ibuton1;
  int lcd_cursor[]={5,0,8,0,11,0,1,1,5,1,8,1,14,1};  // 5,8,11
  int lcd_cursor_index=0;
  lcd.setCursor(lcd_cursor[lcd_cursor_index],lcd_cursor[lcd_cursor_index+1]);
  lcd.cursor();
  while(bSetClock){
    delay(100);
    if(bbuton1_change){
      lcd_cursor_index=lcd_cursor_index+2;
      if(lcd_cursor_index>13){
        bSetClock=false;
      }
    }
    lcd.setCursor(lcd_cursor[lcd_cursor_index],lcd_cursor[lcd_cursor_index+1]);
    bbuton1_change=false;
    if(bbuton2){
      changeValue(lcd_cursor_index);
      bbuton2=false;
    }
  }
  lcd.noCursor();
  ibuton1=old_bbuton1;
}
  
void changeValue(int index){
  int second,minute,hour,wDay, mDay, month, year,temp_value=0; 
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  
  switch(index){
    case 0: {
      Wire.write(0x02);
      Wire.endTransmission(); 
      Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
      temp_value=bcd_int_conv(Wire.read() & 0x3f);
      temp_value=(temp_value+1)%24;
      Wire.beginTransmission(DS1307_I2C_ADDRESS);
      Wire.write(0x02);
      Wire.write(decToBcd(temp_value));
      Wire.endTransmission();
      lcdPrintDateTime();
      break;
    }
    case 2: {
      Wire.write(0x01);
      Wire.endTransmission(); 
      Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
      temp_value=bcd_int_conv(Wire.read());
      temp_value=(temp_value+1)%60;
      Wire.beginTransmission(DS1307_I2C_ADDRESS);
      Wire.write(0x01);
      Wire.write(decToBcd(temp_value));
      Wire.endTransmission();
      lcdPrintDateTime();
      break;
    }
    case 4: {
      Wire.write(0x00);
      Wire.endTransmission(); 
      Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
      temp_value=bcd_int_conv(Wire.read());
      temp_value=(temp_value+1)%60;
      Wire.beginTransmission(DS1307_I2C_ADDRESS);
      Wire.write(0x00);
      Wire.write(decToBcd(temp_value));
      Wire.endTransmission();
      lcdPrintDateTime();
      break;
    }
    case 6: {
      Wire.write(0x03);
      Wire.endTransmission(); 
      Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
      temp_value=bcd_int_conv(Wire.read());
      temp_value=((temp_value+1)%8);
      Serial.println(temp_value);
      if(temp_value==0)
        temp_value=1;
      Serial.println(temp_value);  
      Wire.beginTransmission(DS1307_I2C_ADDRESS);
      Wire.write(0x03);
      Wire.write(decToBcd(temp_value));
      Wire.endTransmission();
      lcdPrintDateTime();
      break;
    }
    case 8: {
      Wire.write(0x04);
      Wire.endTransmission(); 
      Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
      temp_value=bcd_int_conv(Wire.read());
      temp_value=((temp_value+1)%32);
      if(temp_value==0)
        temp_value=1;
      Wire.beginTransmission(DS1307_I2C_ADDRESS);
      Wire.write(0x04);
      Wire.write(decToBcd(temp_value));
      Wire.endTransmission();
      lcdPrintDateTime();
      break;
    }
    case 10: {
      Wire.write(0x05);
      Wire.endTransmission(); 
      Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
      temp_value=bcd_int_conv(Wire.read());
      temp_value=((temp_value+1)%13);
      if(temp_value==0)
        temp_value=1;
      Wire.beginTransmission(DS1307_I2C_ADDRESS);
      Wire.write(0x05);
      Wire.write(decToBcd(temp_value));
      Wire.endTransmission();
      lcdPrintDateTime();
      break;
    }
    case 12: {
      Wire.write(0x06);
      Wire.endTransmission(); 
      Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
      temp_value=bcd_int_conv(Wire.read());
      temp_value=((temp_value+1)%100);
      Wire.beginTransmission(DS1307_I2C_ADDRESS);
      Wire.write(0x06);
      Wire.write(decToBcd(temp_value));
      Wire.endTransmission();
      lcdPrintDateTime();
      break;
    }      
    default: {break;}
  }
}

byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

int bcd_int_conv(byte data)
{
 int temp = 0 ;
 temp = data >> 4 ; 
 return temp*10+(data & B00001111);
}

void lcdPrintDateTime(){
  lcd.setCursor(0,0);  
  lcd.print(getTimeDs1307());
  lcd.setCursor(0,1);  
  lcd.print(getDateEEDs1307());
}
void changeDateTimeDS1307(struct sDateTime _SDT){
    Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write(0x00);
    Wire.write(decToBcd(_SDT.tSeconds & 0x7f));    // 0 to bit 7 starts the clock
    Wire.write(decToBcd(_SDT.tMinutes));
    Wire.write(decToBcd((_SDT.tHours)%24));    
    Wire.write(decToBcd((_SDT.wDay+1)%8));
    Wire.write(decToBcd(_SDT.tDay));
    Wire.write(decToBcd(_SDT.tMonth));
    Wire.write(decToBcd(_SDT.tYear-2000));
    Wire.endTransmission();
}
