#include "Adafruit_SleepyDog.h"
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_FONA.h"

#define DEBUGOUTPUT

/*************************** FONA Pins ***********************************/
#define FONA_PWRKEY 6
#define FONA_RST 7
#define FONA_TX 10 
#define FONA_RX 11 

#define LED 13
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA_LTE fona = Adafruit_FONA_LTE(); 
/************************* APN Setup *********************************/

#define FONA_APN       "hologram"
#define FONA_USERNAME  ""
#define FONA_PASSWORD  ""

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "xxxxx.org"
#define AIO_SERVERPORT  8883
#define AIO_USERNAME    ""
#define AIO_KEY         ""


char imei[16] = {0};

float latitude, longitude, speed_kph, heading, altitude, second;
uint16_t year;
uint8_t month, day, hour, minute;
char latBuff[12], longBuff[12], speedBuff[12],
     headBuff[12], altBuff[12],
     yearBuff[6], monthBuff[6], dayBuff[6], hourBuff[6], minuteBuff[6], secondBuff[6];

/************ Global State (you don't need to change this!) ******************/
Adafruit_MQTT_FONA mqtt(&fona, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY, 1);
#define halt(s) { Serial.println(F( s )); while(1);  }
boolean FONAconnect(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password);

/****************************** Feeds ***************************************/
Adafruit_MQTT_Publish* sensor;
Adafruit_MQTT_Subscribe* feed_command;

/*************************** Sketch Code ************************************/

uint8_t txfailures = 0;

char myConcatenation[128];
char myConcatenation2[128];

void(* resetFunc) (void) = 0;

void setup() {
  while (!Serial);

  pinMode(FONA_RST, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(FONA_RST, HIGH); 
  digitalWrite(LED, LOW);
  
  pinMode(FONA_PWRKEY, OUTPUT);
  pinMode(FONA_PWRKEY, LOW);
  delay(100);
  pinMode(FONA_PWRKEY, HIGH);

  Watchdog.enable(8000);

#ifndef DEBUGOUTPUT
  Serial.begin(115200);
  
#endif
  

  Watchdog.reset();
  delay(5000);  
  Watchdog.reset();
  
  // Initialise the FONA module
  while (! FONAconnect(F(FONA_APN), F(FONA_USERNAME), F(FONA_PASSWORD))) {
#ifndef DEBUGOUTPUT
    Serial.println("Retrying");
#endif    
  }
  Watchdog.reset();
  fona.getIMEI(imei);
#ifndef DEBUGOUTPUT  
  Serial.println(F("Connected to cellular!"));
#endif  
  Watchdog.reset();
  char imeitopic[30];
  sprintf(imeitopic,"/todevice/%s",imei);
  sensor = new Adafruit_MQTT_Publish(&mqtt, "/incoming");
  feed_command = new Adafruit_MQTT_Subscribe(&mqtt, "/test");

  Watchdog.reset();
  mqtt.subscribe(feed_command);

  Watchdog.reset();
  delay(5000); 
  Watchdog.reset();
  
  MQTT_connect();
  sprintf(myConcatenation,"/device/%s/status",imei);
  mqtt.publish(myConcatenation, "online", 0);
  Watchdog.reset();

  while (!fona.enableGPS(true)) {
    delay(1000);
  }
  Watchdog.reset();
  MQTT_connect();
  sprintf(myConcatenation,"/device/%s/status",imei);
  mqtt.publish(myConcatenation, "gps online", 0);
  Watchdog.reset();


}

uint32_t x=0;

void loop() {
  Watchdog.reset();
  MQTT_connect();


  Watchdog.reset();
  txfailures = 0;
  while (!fona.enableGPS(true)) {
    if(txfailures > 4){
      txfailures = -1;
      break;      
    }
    txfailures++;
    delay(1000);
  }
  Watchdog.reset();
  if(txfailures >= 0){

    txfailures = 0;
    while (!fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude, &year, &month, &day, &hour, &minute, &second)) {
      if(txfailures > 4){
        txfailures = -1;
        break;      
      }
      txfailures++;
      delay(1000);
    }
    Watchdog.reset();
    if(txfailures >= 0){
      dtostrf(latitude, 1, 6, latBuff);
      dtostrf(longitude, 1, 6, longBuff);
      dtostrf(speed_kph, 1, 0, speedBuff);
      dtostrf(heading, 1, 0, headBuff);
      dtostrf(altitude, 1, 1, altBuff);
      dtostrf(year, 1, 1, yearBuff);
      dtostrf(month, 1, 1, monthBuff);
      dtostrf(day, 1, 1, dayBuff);
      dtostrf(hour, 1, 1, hourBuff);
      dtostrf(minute, 1, 1, minuteBuff);
      dtostrf(second, 1, 1, secondBuff);
      Watchdog.reset();
      MQTT_connect();
      sprintf(myConcatenation,"/device/%s/gps",imei);
      sprintf(myConcatenation2,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",latBuff, longBuff, speedBuff, headBuff, altBuff, yearBuff, monthBuff, dayBuff, hourBuff, minuteBuff, secondBuff);
      mqtt.publish(myConcatenation, myConcatenation2, 0);
      Watchdog.reset();
    }
  }
  Watchdog.reset();
  
  

  
  Watchdog.reset();  

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &feed_command) {
      Watchdog.reset();

    }
  }
  char bufCommand[1];
  bufCommand[1] = '\0';
  strncpy(bufCommand, feed_command->lastread, 1);
  if(bufCommand[0] == 'a'){
    mqtt.publish("/worda", "aa", 0);
    memset(feed_command->lastread, 0, sizeof(feed_command->lastread));
    
  }

  if (strcmp(feed_command->lastread, "reset") == 0) {
    Watchdog.reset();
  
    MQTT_connect();
    sprintf(myConcatenation,"/device/%s/status",imei);
    mqtt.publish(myConcatenation, "reset", 0);
    Watchdog.reset();
    resetFunc();
  }
  


}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
#ifndef DEBUGOUTPUT
  Serial.println("Connecting");
#endif
  while ((ret = mqtt.connect()) != 0) { 
#ifndef DEBUGOUTPUT
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying");
#endif
    mqtt.disconnect();
    delay(5000); 
  }
#ifndef DEBUGOUTPUT
  Serial.println("Connected!");
#endif
}
