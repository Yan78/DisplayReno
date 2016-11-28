#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <Wire.h>
#include <TinyGPS++.h>

// arduino 1.7.1
// nano 328
// d6,7 rx tx
// a1 dac
// a0 ds18x20
// d2,3 butoane
// d12,11,5,4,9,8 lcd

#define DS18x20_PIN        A0
#define VOLT_PIN           A1
#define LCD_CONTRAST       10
#define debouncing_time    350000

struct sDateTime{
  int tSeconds;
  int tMinutes;
  int tHours;
  int tDay;
  int tMonth;
  int tYear;
  int wDay;
};

byte TEMP1[]={0x28, 0xDB, 0x76, 0x35, 0x05, 0x00, 0x00, 0x2F};
byte TEMP2[]={0x28, 0xFF, 0x47, 0x34, 0x65, 0x14, 0x01, 0xAF};
byte TEMP3[]={0x28, 0xFF, 0x85, 0x31, 0x65, 0x14, 0x01, 0x8F};
byte TEMP4[]={0x28, 0xFF, 0xF3, 0xCD, 0x69, 0x14, 0x04, 0x99};

byte unu[8] = {0, 0, 6, 2, 2, 2, 23, 0};
byte trei[8] = {0, 0, 7, 1, 7, 1, 23, 0};
byte cinci[8] = {0, 0, 7, 4, 7, 1, 23, 0};
byte sapte[8] = {0, 0, 7, 1, 2, 2, 18, 0};
byte noua[8] = {0, 0, 7, 5, 7, 1, 23, 0};
byte degree[8] = {22, 4, 4, 6, 0, 0, 0, 0};
byte battery[8]={14, 31, 17, 17, 17, 31, 31, 0};
byte smiley[8] = {0, 10, 31, 31, 31, 14, 4, 0}; 

LiquidCrystal lcd(12, 11, 5, 4, 9, 8);
SoftwareSerial mySerial(6, 7); 
OneWire ds(DS18x20_PIN);

char linie1[17]="                ";
char linie2[17]="                "; 
char data_ora[6]="00:00";
unsigned int clockOffSet=2;
float volt, iVoltage=0.0;
int coolant, intake_temp=0;
byte pressure=0;
int itemp1, itemp2, itemp3, itemp4=0;
int izec1, izec2, izec3, izec4=0;
int icontrast=60;
bool bbuton1_change, bbuton2, secundar_blink, bSetClock, lcd_clear_flag=false;
byte elm327_installed=false;
volatile int ibuton1=0;
volatile unsigned long last_micros_button, last_millis_obd=0;
sDateTime sDT1;
TinyGPSPlus gps;

void setup(){
  analogReference(INTERNAL);
  pinMode(2,INPUT_PULLUP); 
  pinMode(3,INPUT_PULLUP);  
  attachInterrupt(0, buton2, FALLING );
  attachInterrupt(1, buton1, FALLING ); 
  pinMode(LCD_CONTRAST,OUTPUT);
  pinMode(A0,INPUT);
  Serial.begin(57600);
  Wire.begin();
  lcd.begin(16,2);
  mySerial.begin(38400);
  analogWrite(LCD_CONTRAST,80-icontrast);
  lcd.createChar(7, unu);
  lcd.createChar(1, trei);
  lcd.createChar(2, cinci);
  lcd.createChar(3, sapte);  
  lcd.createChar(4, noua);  
  lcd.createChar(5, degree);
  lcd.createChar(6, battery);
  lcd.createChar(0,smiley);
  lcd.setCursor(0, 0); 
  lcdMeniuTemp3();
//  for(int i=0; i<=254; i++)
//    lcdMeniuTemp3_Update_Voltage();
  elm327_installed=Elm327_begin();
  sDT1 =  getDateTimeDS1307Struct();
  changeHourSummerDaysRO(false);
}

void loop(){
  switch (ibuton1){
    case 0: {lcdMeniuTemp3(); break;}
    case 1: {lcdMeniuOBD2(); break;}
    case 2: {lcdMeniuOraData(); break;}
    case 3: {lcdMeniuSetContrast(); break;}
    case 4: {lcd.clear(); lcd.print(String(F("Ver 1.1 11-05-15"))); 
      lcd.setCursor(7,1); 
      lcd.write(byte(0)); lcd.write(byte(0)); lcd.write(byte(0));
      while(!bbuton1_change){
        if(bbuton2){
          //functie lcd gps debug
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("GPS SYNC");
          delay(2000);
          bbuton2=false;
          syncDS1307WithGpsSummerDay();
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("GPS SYNC DONE");  
          delay(2000);
          ibuton1=0;
          bbuton1_change=true;        
        }
        delay(100); 
      }
      bbuton1_change=false; 
      lcd.clear(); 
      break;
    }
    default: {break;}
  }
}
