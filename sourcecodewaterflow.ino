#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#define FIREBASE_HOST "arduinouyp.firebaseio.com"
#define FIREBASE_AUTH "I4ypeV1hhiElxSk4JBwUB8qWP33Zcp5GsEm1v1oi"
#define WIFI_SSID "ARDUINO"
#define WIFI_PASSWORD "123456789"

float last = 0 ;
byte sensorInterrupt = D2;  // 0 = digital pin 2
byte sensorPin       = D1;
// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;
volatile byte Count; 
float flowRateMinute;
unsigned int flowMilliLitre;
unsigned long totalMilliLitre;
unsigned long oldTime;
String jml;

void setup()
{
 // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  // Initialize a serial connection for reporting values to the host
  //Serial.begin(9600);
 
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  Count        = 0;
  flowRateMinute    = 0.0;
  flowMilliLitre   = 0;
  totalMilliLitre  = 0;
  oldTime           = 0;

  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

/**
 * Main program loop
 */
void loop()
{

   if((millis() - oldTime) > 1000)    // Only process counters once per second
  {
    detachInterrupt(sensorInterrupt);
    flowRateMinute = ((1000.0 / (millis() - oldTime)) * Count) / calibrationFactor;
    oldTime = millis();
    flowMilliLitre = (flowRateMinute / 60) * 1000;
    totalMilliLitre += flowMilliLitre;
     
    if( abs(last-flowMilliLitre) > 1 ){
       last = flowMilliLitre ;
      
         //Serial.print("Debit Air / Second :"); Serial.print(flowMilliLitre);Serial.println("mL/Sec");
         //Serial.print("Volume Air :"); Serial.print(totalMilliLitre);Serial.println("mL");
         Firebase.setString("alif/sensor1", String(totalMilliLitre)+"|"+String(flowMilliLitre));
         //Firebase.setFloat("arus_air/sensor1", flowMilliLitre);
    }
   
    // Reset the pulse counter so we can start incrementing again
    Count = 0;
   
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
}

/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  Count++;
}
