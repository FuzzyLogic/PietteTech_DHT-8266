// CPU 80MHZ, FLASH 4M/1M
/*
  *** Sample using esp-01, D16 is connected to RST
  *** If DHT22 is powered by a gpio( VCC of DHT22 is connected to a gpio) and OUTPUT of DHT22 is connected to D2, boot will fail.
  *** Power off ----> D2 is in LOW( == DHT22 is in LOW) ==> SDIO boot mode.


  Temperature and humidity values are each read out the results of the last measurement.
  For real-time data that need continuous read twice, we recommend repeatedly to read sensors,
  and each read sensor interval is greater than 2 seconds to obtain accuratethe data.
  --> read twice !!!!
  --> sampling period : 2 sec


  
  AM2302 have to wait for the power (on AM2302 power 2S crossed the unstable state,
  the device can not send any instructions to read during this period),
  the test environment temperature and humidity data, and record data,
  since the sensor into a sleep state automatically.
  --> need 2 sec after power on

  3.3V - 5.5V is needed, min is 3.3V
  So if DHT22 is powered by battery(2 * AA), boost module is needed.
  http://www.ebay.com/itm/2-in-1-DC-DC-Step-Down-Step-Up-Converter-1-8V-5V-3V-3-7V-to-3-3V-Power-Module-/271873956073
  
*/

#include <ESP8266WiFi.h>
#include "PietteTech_DHT.h"

extern "C" {
#include "user_interface.h"
}

#define DHT_DEBUG_TIMING

// to check battery voltage using adc
ADC_MODE(ADC_VCC);

// wifi
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

//
int vdd;

WiFiClient wifiClient;

// system defines
#define DHTTYPE  DHT22          // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN   2              // Digital pin for communications
#define REPORT_INTERVAL 300     // in sec

// to check dht
unsigned long startMills;
float t, h;
int acquireresult;

//declaration
void dht_wrapper(); // must be declared before the lib initialization

// Lib instantiate
PietteTech_DHT DHT(DHTPIN, DHTTYPE, dht_wrapper);

// This wrapper is in charge of calling
// must be defined like this for the lib work
void dht_wrapper() {
  DHT.isrCallback();
}

void goingToSleep() {
  Serial.println(" -----> goingToSleep");
  delay(100);
  pinMode(DHTPIN, INPUT);
  ESP.deepSleep((REPORT_INTERVAL * 1000 * 1000 ));
  delay(250);
}


void wifi_connect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.hostname("esp-dht22-deepsleeptest");

  int Attempt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(". ");
    Serial.print(Attempt);
    delay(100);
    Attempt++;
    if (Attempt == 300)
    {
      pinMode(DHTPIN, INPUT);
      ESP.reset();;
    }
  }
}

void setup() {
  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, HIGH);

  startMills = millis();
  Serial.begin(115200);
  Serial.println("");

  // https://blog.adafruit.com/2016/03/03/zephyr-labs-tests-the-esp8266-with-ads1015-adc/
  vdd = ESP.getVcc() * 0.96;
  Serial.println(vdd);

  // read twice
  delay(2100);
  acquireresult = DHT.acquireAndWait(500);
  Serial.println(acquireresult);
  if (acquireresult == 0) {
    t = DHT.getCelsius();
    h = DHT.getHumidity();
    Serial.println(t);
    Serial.println(h);
  } else {
    pinMode(DHTPIN, INPUT);
    ESP.reset();
  }

  delay(2100);
  acquireresult = DHT.acquireAndWait(500);
  Serial.println(acquireresult);

  if (acquireresult == 0) {
    t = DHT.getCelsius();
    h = DHT.getHumidity();
    Serial.println(t);
    Serial.println(h);
  } else {
    pinMode(DHTPIN, INPUT);
    ESP.reset();
  }

  Serial.println("wifi on");
  wifi_connect();
  Serial.println("wifi conn done");

}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("");
    Serial.println("reporting ---> : ");

    // report 
    Serial.println(t);
    Serial.println(h);

    goingToSleep();
  } else {
    goingToSleep();
  }  
}
