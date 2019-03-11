#include "Adafruit_MQTT_Client.h"

bool Adafruit_MQTT_Client::connectServer() {
  // Grab server name from flash and copy to buffer for name resolution.
  memset(buffer, 0, sizeof(buffer));
  strcpy((char *)buffer, servername);
  // Connect and check for success (0 result).
  int r = client->connect((char *)buffer, portnum);
  return r != 0;
}

bool Adafruit_MQTT_Client::disconnectServer() {
  // Stop connection if connected and return success (stop has no indication of
  // failure).
  if (client->connected()) {
    client->stop();
  }
  return true;
}

bool Adafruit_MQTT_Client::connected() {
  // Return true if connected, false if not connected.
  return client->connected();
}

uint16_t Adafruit_MQTT_Client::readPacket(uint8_t *buffer, uint16_t maxlen,
                                          int16_t timeout) {
  /* Read data until either the connection is closed, or the idle timeout is reached. */
  uint16_t len = 0;
  int16_t t = timeout;


  while (client->connected() && (timeout >= 0)) {
    while (client->available()) {
      char c = client->read();
      timeout = t;  // reset the timeout
      buffer[len] = c;
      len++;
      if (maxlen == 0) { // handle zero-length packets
        return 0;
      }
      if (len == maxlen) {  // we read all we want, bail
        return len;
      }
    }
    timeout -= MQTT_CLIENT_READINTERVAL_MS;
    delay(MQTT_CLIENT_READINTERVAL_MS);
  }
  return len;
}

bool Adafruit_MQTT_Client::sendPacket(uint8_t *buffer, uint16_t len) {
  uint16_t ret = 0;

  while (len > 0) {
    if (client->connected()) {
      // send 250 bytes at most at a time, can adjust this later based on Client

      uint16_t sendlen = len > 250 ? 250 : len;
      //Serial.print("Sending: "); Serial.println(sendlen);
      ret = client->write(buffer, sendlen);
      len -= ret;

      if (ret != sendlen) {
		return false;
      }
    } else {
      return false;
    }
  }
  return true;
}
