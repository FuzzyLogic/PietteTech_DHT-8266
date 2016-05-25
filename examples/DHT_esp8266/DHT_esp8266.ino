#include "PietteTech_DHT.h"

// system defines
#define DHTTYPE  DHT22           // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN   2              // Digital pin for communications

#define REPORT_INTERVAL 5000 // in msec must > 2000

// to check dht
unsigned long startMills;
float t, h, d;
int acquireresult;
int acquirestatus;

//declaration
void dht_wrapper(); // must be declared before the lib initialization

// Lib instantiate
PietteTech_DHT DHT(DHTPIN, DHTTYPE, dht_wrapper);

// globals
bool bDHTstarted;       // flag to indicate we started acquisition

// This wrapper is in charge of calling
// must be defined like this for the lib work
void dht_wrapper() {
  DHT.isrCallback();
}

void setup() {
  startMills = millis();
  Serial.begin(115200);

  /*
    while (!Serial) {
    yield(); // wait for serial port to connect.
    }
  */

  Serial.println("");
  Serial.println("DHT Example program using DHT.acquire and DHT.acquireAndWait");
  Serial.println("");

  // delay 2 sec before start acquire
  while ( (millis() - startMills) < 2000 ) {
    yield();
  }

  // blocking method
  acquirestatus = 0;
  acquireresult = DHT.acquireAndWait(1000);
  if ( acquireresult == 0 ) {
    t = DHT.getCelsius();
    h = DHT.getHumidity();
    d = DHT.getDewPoint();
  } else {
    t = h = d = 0;
  }
}

void loop() {
  if (bDHTstarted) {
    acquirestatus = DHT.acquiring();
    if (!acquirestatus) {
      acquireresult = DHT.getStatus();
      if ( acquireresult == 0 ) {
        t = DHT.getCelsius();
        h = DHT.getHumidity();
        d = DHT.getDewPoint();
      }
      bDHTstarted = false;
    }
  }

  if ((millis() - startMills) > REPORT_INTERVAL) {
    if ( acquireresult == 0 ) {
      Serial.println("");

      Serial.print("Humidity (%): ");
      Serial.println(h);

      Serial.print("Temperature (oC): ");
      Serial.println(t);

      Serial.print("Dew Point (oC): ");
      Serial.println(d);

    } else {
      Serial.println("Is dht22 connected");
    }
    startMills = millis();

    // to remove lock
    if (acquirestatus == 1) {
      DHT.reset();
    }

    if (!bDHTstarted) {
      // non blocking method
      DHT.acquire();
      bDHTstarted = true;
    }
  }
}
