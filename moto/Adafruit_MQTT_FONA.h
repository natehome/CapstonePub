#ifndef _ADAFRUIT_MQTT_FONA_H_
#define _ADAFRUIT_MQTT_FONA_H_

#include "Adafruit_FONA.h"
#include "Adafruit_MQTT.h"

#define MQTT_FONA_INTERAVAILDELAY 100
#define MQTT_FONA_QUERYDELAY 500

// FONA-specific version of the Adafruit_MQTT class.
// Note that this is defined as a header-only class to prevent issues with using
// the library on non-FONA platforms (since Arduino will include all .cpp files
// in the compilation of the library).
class Adafruit_MQTT_FONA : public Adafruit_MQTT {
 public:
  Adafruit_MQTT_FONA(Adafruit_FONA *f, const char *server, uint16_t port,
                       const char *cid, const char *user, const char *pass):
    Adafruit_MQTT(server, port, cid, user, pass),
    fona(f)
  {}

  Adafruit_MQTT_FONA(Adafruit_FONA *f, const char *server, uint16_t port,
                     const char *user="", const char *pass=""):
    Adafruit_MQTT(server, port, user, pass),
    fona(f)
  {}

  bool connectServer() {
    char server[40];
    strncpy(server, servername, 40);
#ifdef ADAFRUIT_SLEEPYDOG_H
    Watchdog.reset();
#endif

    // connect to server
    return fona->TCPconnect(server, portnum);
  }

  bool disconnectServer() {
    return fona->TCPclose();
  }

  bool connected() {
    // Return true if connected, false if not connected.
    return fona->TCPconnected();
  }

  uint16_t readPacket(uint8_t *buffer, uint16_t maxlen, int16_t timeout) {
    uint8_t *buffp = buffer;
    if (!fona->TCPconnected()) return 0;
    /* Read data until either the connection is closed, or the idle timeout is reached. */
    uint16_t len = 0;
    int16_t t = timeout;
    uint16_t avail;
    while (fona->TCPconnected() && (timeout >= 0)) {
      while (avail = fona->TCPavailable()) {
        if (len + avail > maxlen) {
	  avail = maxlen - len;
	  if (avail == 0) return len;
        }
        // try to read the data into the end of the pointer
        if (! fona->TCPread(buffp, avail)) return len;
        // read it! advance pointer
        buffp += avail;
        len += avail;
        timeout = t;  // reset the timeout
        if (len == maxlen) {  // we read all we want, bail
	  return len;
        }
      }
#ifdef ADAFRUIT_SLEEPYDOG_H
      Watchdog.reset();
#endif
      timeout -= MQTT_FONA_INTERAVAILDELAY;
      timeout -= MQTT_FONA_QUERYDELAY; // this is how long it takes to query the FONA for avail()
      delay(MQTT_FONA_INTERAVAILDELAY);
    }
    return len;
  }

  bool sendPacket(uint8_t *buffer, uint16_t len) {
    if (fona->TCPconnected()) {
      boolean ret = fona->TCPsend((char *)buffer, len);
      if (!ret) {
        return false;
      }
    } else {
      return false;
    }
    return true;
  }

 private:
  uint32_t serverip;
  Adafruit_FONA *fona;
};

#endif
