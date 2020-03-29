/*
  Plant Watering Sytem
  @auther Jeremy Bohrer
  @date 2/28/2020
  @github https://github.com/jjbskir/ArduinoPlantWateringSystem
  
  The circuit:
  - Temperature/moisture sensor
    Power supply: 3.3-5v

  - Moisture sensor
    Power supply: 3.3-5v
*/


#include "DHT.h"
#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the Wifi radio's status

// NOTE: change to your own server
char server[] = "hydroponics-monitor.herokuapp.com";  
char observationRoute[] = "/hydroponics";
// NOTE: use WiFiSSLClient if connecting to https
WiFiClient client;

const int SOIL_MOISTURE_SENSOR_PIN = A0;

const int dry = 520;
const int wet = 270;
const int moistureLevels = (dry - wet) / 3;

// 60 seconds
const long sensorReadIntervals = 1000L * 60L;
// 1 hr
const long recordIntervals = 1000L * 60L * 60L;
long lastRecordTime = -recordIntervals - 1;

int deviceId = 555;

void setup() {
  Serial.begin(9600);
  dht.begin();
  initWifi();
}

void loop() {
  mainLoop();
}

void mainLoop() {
  // if there's incoming data from the net connection.
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  Serial.println();

  float temperature = getTemperature();
  float humidity = getHumidity();
  long soilMoisture = analogRead(SOIL_MOISTURE_SENSOR_PIN);
  Serial.println("Soil Moisture: " + readableSoilMoisture(soilMoisture) + ", " + soilMoisture);
  Serial.println("Temperature: " + String(temperature) + " *F");
  Serial.println("Humidity: " + String(humidity) + " %");
    
  if (millis() - lastRecordTime > recordIntervals) {
    // int deviceId, float temperature, float ph, float humidity, float 
    String data = createObservationJson(deviceId, temperature, 0, humidity, soilMoisture, 0);
    postObservation(data);
    lastRecordTime = millis();
  }

  delay(sensorReadIntervals);
}

void initWifi() {
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printWifiStatus();
}

String readableSoilMoisture(int soilMoisture){
  if (soilMoisture <= wet) {
    return "Water";
  } else if (soilMoisture > wet && soilMoisture < (wet + moistureLevels)) {
    return "Very Wet";
  } else if (soilMoisture > (wet + moistureLevels) && soilMoisture < (dry - moistureLevels)) {
     return "Wet";
  } else if (soilMoisture < dry && soilMoisture > (dry - moistureLevels)) {
    return "Dry";
  } else {
    return "Air";
  }
}

float getTemperature() {
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float temperature = dht.readTemperature(true);
  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  return temperature;
}

float getHumidity() {
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  return humidity;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void postObservation(String data) {
  // close any connection before send a new request.
  client.stop();
  
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    Serial.println(data);

    client.println("POST " + String(observationRoute) + " HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Content-Type: application/x-www-form-urlencoded");  
    client.println("Cache-Control: no-cache");
    client.println("Content-Length: " + String(data.length()));
    client.println();
    client.println(data);
  } else {
    Serial.println("connection failed");
  }
}

String createObservationJson(int deviceId, float temperature, float ph, float humidity, float soilmoisture, float waterLevel) {
  String data = "";
  if (deviceId) {
    data += "DeviceId=" + String(deviceId) + "&";
  }
  if (temperature) {
    data += "temperature=" + String(temperature) + "&";
  }
  if (ph) {
    data += "ph=" + String(ph) + "&";
  }
  if (humidity) {
    data += "humidity=" + String(humidity) + "&";
  }
  if (soilmoisture) {
    data += "soilmoisture=" + String(soilmoisture) + "&";
  }
  if (waterLevel) {
    data += "waterlevel=" + String(waterLevel) + "&";
  }
  return data;
}
