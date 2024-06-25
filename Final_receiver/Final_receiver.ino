#include <EEPROM.h>
#include <NewPing.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <RH_ASK.h> // Include RadioHead Amplitude Shift Keying Library
#include <SPI.h> // Include dependant SPI Library

#define TRIG_PIN_IN 8
#define ECHO_PIN_IN 9
#define TRIG_PIN_OUT 10
#define ECHO_PIN_OUT 11
#define BUZZER_PIN 12

NewPing sensorIn(TRIG_PIN_IN, ECHO_PIN_IN);
NewPing sensorOut(TRIG_PIN_OUT, ECHO_PIN_OUT);
RH_ASK rf_driver(2000, 3);
const byte ROWS = 4;
const byte COLS = 3;

byte rowPins[ROWS] = {7, 6, 5, 4};
byte colPins[COLS] = {0, 2, 1};

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2); 

int TotalPeopleCount = 0;
int PreviousTotalPeopleCount = 0;
int peopleCountData;
int ExspeopleCountData;
int peopleCount = 0;
int distanceIn = 0;
int distanceOut = 0;
int DoorDistance = 0;
int i, k = 0;
int a, b= 0;
int other = 0;
int oth = 0;

char Data[16];
char DataExt[16];
char Distance[16];
byte data_count = 0;
byte DataExt_count = 0;
byte Distance_count = 0;
char customKey;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  rf_driver.init();
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(3, 0); // Set cursor to character 2 on line 0
  lcd.print("Teletronic");
  lcd.setCursor(5, 1);
  lcd.print("Squad");
  delay(4000);
  lcd.clear();
  while ((a<10) || (b<10))
  {
    a = sensorIn.ping_cm();
    b = sensorOut.ping_cm();
    if (a<10)
    {
      lcd.setCursor(0, 0);
      lcd.print("SensorIn X");
      delay(100);
      lcd.clear();
    } 
    else if (b<10)
    {
      lcd.setCursor(0, 1);
      lcd.print("SensorOut X");
      delay(100);
      lcd.clear();
    }
    
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("capacity:");
  lcd.setCursor(0, 1);
  


  while (true && customKey!='*' )
  {
    customKey = customKeypad.getKey();
    if (customKey && customKey!= '*' && customKey!='#') 
    {
      // Enter keypress into array and increment counter
      DataExt[DataExt_count] = customKey;
      lcd.setCursor(DataExt_count, 1);
      lcd.print(customKey);
      DataExt_count++;
    }
    else if (customKey=='#')
    {
      DataExt_count--;
      DataExt[DataExt_count] =' ';
      lcd.setCursor(DataExt_count, 1);
      lcd.print(' ');
      
    }

  }
  lcd.clear();
  peopleCountData  = atoi(DataExt);
  lcd.setCursor(0, 0);
  lcd.print("people inside:"); 
  lcd.setCursor(0, 1);
  while (true && customKey!='#' )
  {
    customKey = customKeypad.getKey();
    if (customKey && customKey!= '#' && customKey!='*') 
    {
      // Enter keypress into array and increment counter
      Data[data_count] = customKey;
      lcd.setCursor(data_count, 1);
      lcd.print(customKey);
      data_count++;
    }
    else if (customKey=='*')
    {
      data_count--;
      Data[data_count] = ' ';
      lcd.setCursor(data_count, 1);
      lcd.print(' ');
      
    }
  }
  lcd.clear();
  ExspeopleCountData = atoi(Data);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Previous memory?");
  lcd.setCursor(0, 1);
  lcd.print("*.YES    #.NO");
  while (true)
  {
    customKey = customKeypad.getKey();
    if (customKey == '*') 
    {
      if (EEPROM.read(0) == 255)
      {
        ExspeopleCountData = 0;
      }
      else
      {
        EEPROM.get(0, ExspeopleCountData);
      }
    lcd.clear();
    break;
    }
    else if (customKey == '#') 
    {
    lcd.clear();
    break;
    }
  }
}

void loop() 
{
  uint8_t buf[6];
  uint8_t buflen = sizeof(buf);
  if (rf_driver.recv(buf, &buflen))
  {
  // Message received with valid che-cksum
  String string = (char*)buf;

  oth = string.toInt();
  other = oth - 500;
  }
  int distanceIn = sensorIn.ping_cm();
  int distanceOut = sensorOut.ping_cm();
  if ((distanceIn < 20) && (distanceIn > 0))
  {
    while (true)
    {
      int distanceOut = sensorOut.ping_cm();

      if ((distanceOut < 20) && (distanceOut > 0))
      {
        peopleCount++;
        delay(1000);
        break;
        
      }
    }
  }

  if ((distanceOut < 20) && (distanceOut > 0)) 
  {
    while (true)
    {
      int distanceIn = sensorIn.ping_cm();
      if ((distanceIn < 20) && (distanceIn > 0))
      {
        peopleCount--;
        delay(1000);
        break;
      }
    }
  }
  TotalPeopleCount = peopleCount + ExspeopleCountData + other;
  if (PreviousTotalPeopleCount != TotalPeopleCount)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(TotalPeopleCount);
  }
  
  PreviousTotalPeopleCount = TotalPeopleCount;
  
  if (ExspeopleCountData + peopleCount + other >= peopleCountData )
  {
    lcd.clear();
    lcd.print("maximum capacity");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(20);
  }
  EEPROM.put(0,peopleCount + ExspeopleCountData + other);
}