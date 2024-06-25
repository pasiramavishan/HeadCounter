#include <NewPing.h>
#include <RH_ASK.h> // Include RadioHead Amplitude Shift Keying Library
#include <SPI.h> // Include dependant SPI Library
 

#define TRIG_PIN_IN 8
#define ECHO_PIN_IN 9
#define TRIG_PIN_OUT 10
#define ECHO_PIN_OUT 11

RH_ASK rf_driver;

NewPing sensorIn(TRIG_PIN_IN, ECHO_PIN_IN);
NewPing sensorOut(TRIG_PIN_OUT, ECHO_PIN_OUT);

int peopleCount = 0;
int distanceIn = 0;
int distanceOut = 0;
String string; 


void setup() {
  Serial.begin(9600);
  rf_driver.init();

} 
  


  

void loop() 
{
  int distanceIn = sensorIn.ping_cm();
  int distanceOut = sensorOut.ping_cm();


  if ((distanceIn < 20) && (distanceIn > 0))
  {
    while (true)
    {
      int distanceOut = sensorOut.ping_cm();
      if ((distanceOut < 20) && (distanceOut > 0) )
      {
        peopleCount++;
        delay(1000);
        
        
        break;
        
      }
    }
  }

  if ((distanceOut < 20) && (distanceOut > 0) ) 
  {
    while (true)
    {
      int distanceIn = sensorIn.ping_cm();
      if ((distanceIn < 20) && (distanceIn > 0) )
      {
        peopleCount--;
        delay(1000);
        break;

      }
    }
  }
  string = String(peopleCount);
  const char *msg = string.c_str() ;
  rf_driver.send((uint8_t *)msg, strlen(msg));

  rf_driver.waitPacketSent();
  
}