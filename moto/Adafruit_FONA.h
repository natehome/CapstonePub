/***************************************************
  This is a library for our Adafruit FONA Cellular Module

  Designed specifically to work with the Adafruit FONA
  ----> http://www.adafruit.com/products/1946
  ----> http://www.adafruit.com/products/1963

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/
#ifndef ADAFRUIT_FONA_H
#define ADAFRUIT_FONA_H

#include "FONAConfig.h"
#include "FONAExtIncludes.h"
#include "FONAPlatform.h"

#define SIM800L 1
#define SIM800H 6

#define SIM808_V1 2
#define SIM808_V2 3

#define SIM5320A 4
#define SIM5320E 5

#define SIM7000A 7
#define SIM7000C 8
#define SIM7000E 9
#define SIM7000G 10

#define SIM7500A 11
#define SIM7500E 12

// Set the preferred SMS storage.
//   Use "SM" for storage on the SIM.
//   Use "ME" for internal storage on the FONA chip
#define FONA_PREF_SMS_STORAGE "\"SM\""
//#define FONA_PREF_SMS_STORAGE "\"ME\""


#define FONA_DEFAULT_TIMEOUT_MS 500
#define FONA_NO_RST_PIN 99

#define FONA_HTTP_GET   0
#define FONA_HTTP_POST  1
#define FONA_HTTP_HEAD  2

class Adafruit_FONA : public FONAStreamType {
 public:
  Adafruit_FONA(int8_t);
  boolean begin(FONAStreamType &port);
  uint8_t type();

  // Stream
  int available(void);
  size_t write(uint8_t x);
  int read(void);
  int peek(void);
  void flush();

  // FONA 3G requirements
  boolean setBaudrate(uint16_t baud);

  // RTC
  boolean enableRTC(uint8_t i);
  boolean readRTC(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *hr, uint8_t *min, uint8_t *sec);

  // Power, battery and ADC
  boolean powerDown(void);
  boolean getADCVoltage(uint16_t *v);
  boolean getBattPercent(uint16_t *p);
  boolean getBattVoltage(uint16_t *v);

  // SIM query
  uint8_t unlockSIM(char *pin);
  uint8_t getSIMCCID(char *ccid);
  uint8_t getNetworkStatus(void);
  uint8_t getRSSI(void);

  // IMEI
  uint8_t getIMEI(char *imei);

  // SMS handling
  boolean setSMSInterrupt(uint8_t i);
  uint8_t getSMSInterrupt(void);
  int8_t getNumSMS(void);
  boolean readSMS(uint8_t i, char *smsbuff, uint16_t max, uint16_t *readsize);
  boolean sendSMS(char *smsaddr, char *smsmsg);
  boolean deleteSMS(uint8_t i);
  boolean getSMSSender(uint8_t i, char *sender, int senderlen);
  boolean sendUSSD(char *ussdmsg, char *ussdbuff, uint16_t maxlen, uint16_t *readlen);

  // Time
  boolean enableNetworkTimeSync(boolean onoff);
  boolean enableNTPTimeSync(boolean onoff, FONAFlashStringPtr ntpserver=0);
  boolean getTime(char *buff, uint16_t maxlen);

  // GPRS handling
  boolean enableGPRS(boolean onoff);
  uint8_t GPRSstate(void);
  boolean getGSMLoc(uint16_t *replycode, char *buff, uint16_t maxlen);
  boolean getGSMLoc(float *lat, float *lon);
  void setNetworkSettings(FONAFlashStringPtr apn, FONAFlashStringPtr username=0, FONAFlashStringPtr password=0);
  boolean postData(const char *request_type, const char *URL, char *body = "", const char *token = "");
  boolean postData(const char *server, uint16_t port, const char *connType, char *URL, char *body = "");
  void getNetworkInfo(void);

  // GPS handling
  boolean enableGPS(boolean onoff);
  int8_t GPSstatus(void);
  uint8_t getGPS(uint8_t arg, char *buffer, uint8_t maxbuff);
  // boolean getGPS(float *lat, float *lon, float *speed_kph=0, float *heading=0, float *altitude=0);
  boolean getGPS(float *lat, float *lon, float *speed_kph, float *heading, float *altitude,
                              uint16_t *year = NULL, uint8_t *month = NULL, uint8_t *day = NULL, uint8_t *hour = NULL, uint8_t *min = NULL, float *sec = NULL);
  boolean enableGPSNMEA(uint8_t nmea);

  // TCP raw connections
  boolean TCPconnect(char *server, uint16_t port);
  boolean TCPclose(void);
  boolean TCPconnected(void);
  boolean TCPsend(char *packet, uint8_t len);
  uint16_t TCPavailable(void);
  uint16_t TCPread(uint8_t *buff, uint8_t len);

  // MQTT
  boolean MQTTconnect(const char *protocol, const char *clientID, const char *username = "", const char *password = "");
  boolean MQTTdisconnect(void);
  boolean MQTTpublish(const char* topic, const char* message);
  boolean MQTTsubscribe(const char* topic, byte QoS);
  boolean MQTTunsubscribe(const char* topic);
  boolean MQTTreceive(const char* topic, const char* buf, int maxlen);

  // FTP
  boolean FTP_Connect(const char* serverIP, uint16_t port, const char* username, const char* password);
  boolean FTP_Quit();
  boolean FTP_Rename(const char* filePath, const char* oldName, const char* newName);
  boolean FTP_Delete(const char* fileName, const char* filePath);
  boolean FTP_GET(const char* fileName, const char* filePath, uint16_t numBytes);
  boolean FTP_PUT(const char* fileName, const char* filePath, char* content, uint16_t numBytes);

  // HTTP low level interface (maps directly to SIM800 commands).
  boolean HTTP_init();
  boolean HTTP_term();
  void HTTP_para_start(FONAFlashStringPtr parameter, boolean quoted = true);
  boolean HTTP_para_end(boolean quoted = true);
  boolean HTTP_para(FONAFlashStringPtr parameter, const char *value);
  boolean HTTP_para(FONAFlashStringPtr parameter, FONAFlashStringPtr value);
  boolean HTTP_para(FONAFlashStringPtr parameter, int32_t value);
  boolean HTTP_data(uint32_t size, uint32_t maxTime=10000);
  boolean HTTP_action(uint8_t method, uint16_t *status, uint16_t *datalen, int32_t timeout = 10000);
  boolean HTTP_readall(uint16_t *datalen);
  boolean HTTP_ssl(boolean onoff);

  // HTTP high level interface (easier to use, less flexible).
  boolean HTTP_GET_start(char *url, uint16_t *status, uint16_t *datalen);
  void HTTP_GET_end(void);
  boolean HTTP_POST_start(char *url, FONAFlashStringPtr contenttype, const uint8_t *postdata, uint16_t postdatalen,  uint16_t *status, uint16_t *datalen);
  void HTTP_POST_end(void);
  void setUserAgent(FONAFlashStringPtr useragent);

  // HTTPS
  void setHTTPSRedirect(boolean onoff);

  // PWM (buzzer)
  boolean setPWM(uint16_t period, uint8_t duty = 50);

  // Helper functions to verify responses.
  boolean expectReply(FONAFlashStringPtr reply, uint16_t timeout = 10000);
  boolean sendCheckReply(char *send, char *reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(FONAFlashStringPtr send, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(char* send, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);


 protected:
  int8_t _rstpin;
  uint8_t _type;

  char replybuffer[255];
  FONAFlashStringPtr apn;
  FONAFlashStringPtr apnusername;
  FONAFlashStringPtr apnpassword;
  boolean httpsredirect;
  FONAFlashStringPtr useragent;
  FONAFlashStringPtr ok_reply;

  // HTTP helpers
  boolean HTTP_setup(char *url);

  void flushInput();
  uint16_t readRaw(uint16_t b);
  uint8_t readline(uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS, boolean multiline = false);
  uint8_t getReply(char *send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(FONAFlashStringPtr send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(FONAFlashStringPtr prefix, char *suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(FONAFlashStringPtr prefix, int32_t suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(FONAFlashStringPtr prefix, int32_t suffix1, int32_t suffix2, uint16_t timeout); // Don't set default value or else function call is ambiguous.
  uint8_t getReplyQuoted(FONAFlashStringPtr prefix, FONAFlashStringPtr suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);

  boolean sendCheckReply(FONAFlashStringPtr prefix, char *suffix, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(FONAFlashStringPtr prefix, int32_t suffix, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(FONAFlashStringPtr prefix, int32_t suffix, int32_t suffix2, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReplyQuoted(FONAFlashStringPtr prefix, FONAFlashStringPtr suffix, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);

  void mqtt_connect_message(const char *protocol, byte *mqtt_message, const char *client_id, const char *username, const char *password);
  void mqtt_publish_message(byte *mqtt_message, const char *topic, const char *message);
  void mqtt_subscribe_message(byte *mqtt_message, const char *topic, byte QoS);
  void mqtt_disconnect_message(byte *mqtt_message);
  boolean mqtt_sendPacket(byte *packet, byte len);


  boolean parseReply(FONAFlashStringPtr toreply,
          uint16_t *v, char divider  = ',', uint8_t index=0);
  boolean parseReplyFloat(FONAFlashStringPtr toreply,
           float *f, char divider, uint8_t index);
  boolean parseReply(FONAFlashStringPtr toreply,
          char *v, char divider  = ',', uint8_t index=0);
  boolean parseReplyQuoted(FONAFlashStringPtr toreply,
          char *v, int maxlen, char divider, uint8_t index);

  boolean sendParseReply(FONAFlashStringPtr tosend,
       FONAFlashStringPtr toreply,
       uint16_t *v, char divider = ',', uint8_t index=0);

  boolean sendParseReplyFloat(FONAFlashStringPtr tosend,
         FONAFlashStringPtr toreply,
         float *f, char divider = ',', uint8_t index=0);

  static boolean _incomingCall;
  static void onIncomingCall();

  FONAStreamType *mySerial;
};

class Adafruit_FONA_LTE : public Adafruit_FONA {

 public:
  Adafruit_FONA_LTE () : Adafruit_FONA(FONA_NO_RST_PIN) { _type = SIM7000A; _type = SIM7500A;}

  boolean setBaudrate(uint16_t baud);
  boolean hangUp(void);
};

#endif