#include "PietteTech_DHT.h"

// system defines
#define DHTTYPE  DHT22           // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN   2              // Digital pin for communications

#define REPORT_INTERVAL 5000 // in msec

// to check dht
unsigned long startMills;
float t, h, d;
int acquireresult;

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

void setup()
{
  startMills = millis();
  Serial.begin(115200);

  Serial.println("");
  Serial.println("DHT Example program using DHT.acquire and DHT.acquireAndWait");
  Serial.println("");

  // blocking method
  acquireresult = DHT.acquireAndWait(1000);
  if ( acquireresult == 0 ) {
    t = DHT.getCelsius();
    h = DHT.getHumidity();
    d = DHT.getDewPoint();
  } else {
    t = h = d = 0;
  }
}

void loop()
{      
  if (bDHTstarted) {
    if (!DHT.acquiring()) {
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
    Serial.println("");
    
    Serial.print("Humidity (%): ");
    Serial.println(h);

    Serial.print("Temperature (oC): ");
    Serial.println(t);

    Serial.print("Dew Point (oC): ");
    Serial.println(d);
    
    startMills = millis();

    // to remove lock
    if (acquireresult == 1) {
      detachInterrupt(DHTPIN);
    }

    if (!bDHTstarted) {
      // non blocking method
      DHT.acquire();
      bDHTstarted = true;
    }
  }
}
